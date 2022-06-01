#include <getopt.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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

namespace {

const char *ENV_RUNTIME_PATH = "WRAPPER_MAKE_RUNTIME_PATH";
const char *COMPILATION_DATABASE = "compile_commands.json";

void Help();
int ExecuteCommand(char **argv);
const char *ParseSourceFile(int argc, char **argv);
rapidjson::Document GenerateJSON(int argc, char **argv,
                                 const char *source_file);
void WriteToFile(const std::string &filename,
                 const rapidjson::Document &document);

}  // namespace

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    Help();
    return EXIT_FAILURE;
  }
  argc -= 1;
  argv += 1;

  const char *runtime_path = getenv(ENV_RUNTIME_PATH);
  if (runtime_path != nullptr) {
    std::ostringstream stream;
    stream << runtime_path << "/" << COMPILATION_DATABASE << ".tmp";
    std::string filename = stream.str();
    if (!std::filesystem::exists(filename)) {
      return EXIT_FAILURE;
    }

    const auto *source_file = ParseSourceFile(argc, argv);
    if (source_file != nullptr) {
      const auto &document = GenerateJSON(argc, argv, source_file);
      WriteToFile(filename, document);
    }
  }
  return ExecuteCommand(argv);
}

namespace {

void Help() {}

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
  int num_arguments = argc;
  std::unique_ptr<char *[]> arguments(new char *[num_arguments]);
  for (int i = 0; i < num_arguments; ++i) {
    arguments[i] = argv[i];
  }

  static struct option options[] = {
      {nullptr, 0, nullptr, 0},
  };
  opterr = 0;
  while (getopt_long(num_arguments, arguments.get(), "o:", options, nullptr) !=
         -1)
    ;
  return (optind == num_arguments - 1) ? arguments[optind] : nullptr;
}

rapidjson::Document GenerateJSON(int argc, char **argv,
                                 const char *source_file) {
  rapidjson::Document document;
  auto &object = document.SetObject();

  char directory[PATH_MAX];
  getcwd(directory, PATH_MAX);
  object.AddMember(
      "directory",
      rapidjson::Value(rapidjson::kStringType)
          .SetString(directory, strlen(directory), document.GetAllocator())
          .Move(),
      document.GetAllocator());

  object.AddMember("file", rapidjson::StringRef(source_file),
                   document.GetAllocator());

  rapidjson::Value arguments(rapidjson::kArrayType);
  for (int i = 0; i < argc; ++i) {
    arguments.PushBack(rapidjson::StringRef(argv[i]), document.GetAllocator());
  }
  object.AddMember("arguments", arguments, document.GetAllocator());
  return document;
}

void WriteToFile(const std::string &filename,
                 const rapidjson::Document &document) {
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);

  std::string content = buffer.GetString();
  content += ",\n";
  std::ofstream stream(filename, std::ios::app);
  stream << content;
}

}  // namespace
