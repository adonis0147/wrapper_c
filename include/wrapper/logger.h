#pragma once

#include <ctime>
#include <iostream>

namespace wrapper {

enum class LogLevel { INFO, WARN, ERROR };

inline constexpr const char* ToString(LogLevel level) {
  constexpr const char* Literals[] = {"INFO", "WARN", "ERROR"};
  return Literals[static_cast<int>(level)];
}

class Logger {
 public:
  static void SetActive(bool value) { active = value; }
  static void SetLevel(LogLevel value) { level = value; }

  explicit Logger(LogLevel level) : Logger(std::cerr, level) {}

  Logger(std::ostream& ostream, LogLevel level) : ostream_(ostream), level_(level) {
    if (!Enabled(level_)) {
      return;
    }
    ostream_ << std::endl << Timestamp() << " [" << ToString(level) << "] ";
  }

  template <typename T>
  Logger& operator<<(T&& value) {
    if (Enabled(level_)) {
      ostream_ << value;
    }
    return *this;
  }

 private:
  bool Enabled(LogLevel level) { return Logger::active && level >= Logger::level; }

  std::string Timestamp() {
    time_t raw_time = 0;
    time(&raw_time);
    struct tm* timeinfo = localtime(&raw_time);

    char buffer[] = "YYYY-MM-DD HH:MM:SS";
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
  }

  inline static bool active = false;
  inline static LogLevel level = LogLevel::INFO;

  std::ostream& ostream_;
  LogLevel level_;
};

}  // namespace wrapper

#define LOG(LEVEL) \
  wrapper::Logger { wrapper::LogLevel::LEVEL }

#define SET_LOG_LEVEL(LEVEL)                             \
  do {                                                   \
    wrapper::Logger::SetActive(true);                    \
    wrapper::Logger::SetLevel(wrapper::LogLevel::LEVEL); \
  } while (0)
