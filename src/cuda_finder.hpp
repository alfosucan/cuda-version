#pragma once

#include <atomic>
#ifdef GCC7
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#include <functional>
#include <map>
#include <mutex>
#include <thread>
#include <iostream>

#include <logger.hpp>



class CudaFinder {
private:
  std::mutex m_map_mutex;
  std::map<std::string, fs::directory_entry> m_results_map;
  fs::path m_target_path;
  std::string m_target_version;
  std::vector<std::thread> m_workers;
  std::atomic_bool m_working = true;
  std::atomic_bool m_nvcc_found = false;
  std::atomic_bool m_lib64_found = false;
  std::shared_ptr<Logger> m_logger;
  const std::vector<std::string> m_dirs{"/", "/usr", "/opt"};

  void search_path(fs::path, const std::string &);
  bool verify_cuda();
  bool is_nvcc(const fs::path &);
  bool is_lib64(const fs::path &);
  void find(std::size_t workers_ = std::thread::hardware_concurrency());
  fs::path get_install_dir(const fs::path&);
  void on_time_out();
public:
  CudaFinder(const std::string &version)
      : m_target_version(version), m_logger(Logger::getLogger()){};
  void stop();
  bool swapVersion(int time_out = 30);
  ~CudaFinder(){};
  std::map<std::string, fs::directory_entry> yield();
};
