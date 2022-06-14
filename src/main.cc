#include <getopt.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wrapper/logger.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "macros.h"

namespace {

const char *const ENV_RUNTIME_PATH = "WRAPPER_MAKE_RUNTIME_PATH";
const char *const COMPILATION_DATABASE = "compile_commands.json";

int ParseArguments(int &argc, char **&argv);
void Usage(const char *self);
void Version(const char *self);
int ExecuteCommand(char **argv);
const char *ParseSourceFile(int argc, char **argv);
rapidjson::Document GenerateJSON(int argc, char **argv, const char *source_file);
void WriteToFile(const std::string &filename, const rapidjson::Document &document);

bool g_no_exec = false;

}  // namespace

int main(int argc, char *argv[]) {
  int status = ParseArguments(argc, argv);
  if (status != 0) {
    return status;
  }

  const char *runtime_path = getenv(ENV_RUNTIME_PATH);
  LOG(INFO) << "Runtime path: " << (runtime_path != nullptr ? runtime_path : "");
  if (runtime_path != nullptr) {
    std::ostringstream stream;
    stream << runtime_path << "/" << COMPILATION_DATABASE << ".tmp";
    std::string filename = stream.str();
    if (!std::filesystem::exists(filename)) {
      LOG(ERROR) << filename << " doesn't exist.";
      return EXIT_FAILURE;
    }

    const auto *source_file = ParseSourceFile(argc, argv);
    LOG(INFO) << "Source file: " << (source_file != nullptr ? source_file : "");
    if (source_file != nullptr) {
      const auto &document = GenerateJSON(argc, argv, source_file);
      WriteToFile(filename, document);
    }
  }

  if (g_no_exec) {
    LOG(INFO) << "No execution.";
    return EXIT_SUCCESS;
  }
  return ExecuteCommand(argv);
}

namespace {

int ParseArguments(int &argc, char **&argv) {
  if (argc <= 1) {
    Usage(argv[0]);
    return EXIT_FAILURE;
  }
  static struct option options[] = {
      {"help", no_argument, nullptr, 'h'},
      {"version", no_argument, nullptr, 'v'},
      {"no_exec", no_argument, nullptr, 'n'},
      {"debug", no_argument, nullptr, 'd'},
      {nullptr, 0, nullptr, 0},
  };
  int opt;
  while ((opt = getopt_long(argc, argv, "hvnd", options, nullptr)) != -1) {
    switch (opt) {
      case 'h':
        Usage(argv[0]);
        exit(EXIT_SUCCESS);
      case 'v':
        Version(argv[0]);
        exit(EXIT_SUCCESS);
      case 'n':
        g_no_exec = true;
        break;
      case 'd':
        SET_LOG_LEVEL(INFO);
        break;
      default:
        Usage(argv[0]);
        return EXIT_FAILURE;
    }
  }
  argc -= optind;
  argv += optind;
  return EXIT_SUCCESS;
}

void Usage(const char *self) {
  std::cerr << "Usage: " << self << " [OPTION]... -- compiler" << std::endl;
  std::cerr << std::endl;
  std::cerr << "OPTION" << std::endl;
  std::cerr << "\t-h, --help\tHelp" << std::endl;
  std::cerr << "\t-v, --version\tVersion" << std::endl;
  std::cerr << "\t-n, --no_exec\tDon't execute the command for the compiler." << std::endl;
  std::cerr << "\t-d, --debug\tActivate the logging." << std::endl;
}

void Version(const char *self) {
  std::cerr << self << " " << VERSION << std::endl;
  std::cerr << std::endl;
  std::cerr << "Build type:    " << BUILD_TYPE << std::endl;
  std::cerr << "Compiled time: " << COMPILED_TIME << std::endl;
  std::cerr << "Commit:        " << COMMIT_ID << std::endl;
}

int ExecuteCommand(char **argv) {
  const char *compiler = argv[0];
  pid_t pid = fork();
  if (pid == 0) {
    return execvp(compiler, argv);
  } else {
    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
  }
}

const char *ParseSourceFile(int argc, char **argv) {
  bool should_not_link = false;
  for (int i = 0; i < argc && !should_not_link; ++i) {
    should_not_link = (strcmp(argv[i], "-c") == 0);
  }
  if (!should_not_link) {
    LOG(WARN) << "Skip parsing due to the command will link the objects.";
    return nullptr;
  }

  std::ostringstream stream;
  int num_arguments = argc;
  std::unique_ptr<char *[]> arguments(new char *[num_arguments + 1]);
  for (int i = 0; i < num_arguments; ++i) {
    arguments[i] = argv[i];
    stream << (i != 0 ? " " : "") << argv[i];
  }
  arguments[num_arguments] = nullptr;

  LOG(INFO) << "Command: " << stream.str();

  static struct option options[] = {
      {"MF", required_argument, nullptr, 0},
      {"MT", required_argument, nullptr, 0},
      {"MQ", required_argument, nullptr, 0},
      {nullptr, 0, nullptr, 0},
  };

  optind = 1;
  while (getopt_long_only(num_arguments, arguments.get(), ":o:", options, nullptr) != -1) {
  }
  return (optind == num_arguments - 1) ? arguments[optind] : nullptr;
}

rapidjson::Document GenerateJSON(int argc, char **argv, const char *source_file) {
  rapidjson::Document document;
  auto &object = document.SetObject();

  char path[PATH_MAX];
  const auto *directory = getcwd(path, PATH_MAX);
  object.AddMember("directory",
                   rapidjson::Value(rapidjson::kStringType)
                       .SetString(directory, strlen(directory), document.GetAllocator())
                       .Move(),
                   document.GetAllocator());

  object.AddMember("file", rapidjson::StringRef(source_file), document.GetAllocator());

  rapidjson::Value arguments(rapidjson::kArrayType);
  for (int i = 0; i < argc; ++i) {
    arguments.PushBack(rapidjson::StringRef(argv[i]), document.GetAllocator());
  }
  object.AddMember("arguments", arguments, document.GetAllocator());
  return document;
}

void WriteToFile(const std::string &filename, const rapidjson::Document &document) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);

  std::string content = buffer.GetString();
  content += ",\n";
  std::ofstream stream(filename, std::ios::app);
  stream << content;
}

}  // namespace
