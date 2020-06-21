#include <functional>
#include <iostream>
#include <memory>
#include <mutex>

class Logger;
static std::shared_ptr<Logger> m_instance;

#define ERROR(X) (std::string("\033[0;31m") + std::string(X) + std::string("\033[0m"))
class Logger {
private:
  std::recursive_mutex m_log_mutex;
  Logger() : m_log_mutex(){};

public:
  static std::weak_ptr<Logger> getLogger();
  template <class T>
  void log(const T &param) {
    std::lock_guard lock(m_log_mutex);
    std::cout << param;
    std::cout << "\n";
  }

  template <class T, class... Args>
  void log(const T &first, const Args &... args) {
    std::lock_guard lock(m_log_mutex);
    std::cout << first;
    log(args...);
  };
  ~Logger(){};
};

inline std::weak_ptr<Logger> Logger::getLogger() {

  if (m_instance == nullptr)
    m_instance = std::shared_ptr<Logger>(new Logger());
  return m_instance;
}
