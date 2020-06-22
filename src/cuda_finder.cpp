#include <cuda_finder.hpp>
#include <timer.hpp>

void CudaFinder::find(std::size_t workers_)
{

  for (int i = 0; i < workers_; i++)
  {
    m_workers.push_back(std::move(
        std::thread(&CudaFinder::search_path, this,
                    fs::path(m_dirs[i % m_dirs.size()]), m_target_version)));
  };
}

void CudaFinder::search_path(fs::path path_, const std::string &match_)
{
  fs::recursive_directory_iterator recDirIt(
      path_,
      fs::directory_options(fs::directory_options::skip_permission_denied));
  for (const auto &entry : recDirIt)
{
    if ((m_nvcc_found && m_lib64_found) || !m_working)
    {
      break;
    }

    if (entry.path().string().find(match_) != std::string::npos)
    {
      std::string key;

      if (is_nvcc(entry.path()))
        key = std::string("nvcc");
      if (is_lib64(entry.path()))
        key = std::string("lib64");
      std::lock_guard lock(m_map_mutex);
      m_results_map.try_emplace(key, entry);
    }
  }
}

void CudaFinder::stop()
{
  m_working = false;
  for (auto &worker : m_workers)
    worker.join();
}

bool CudaFinder::swapVersion(int time_out)
{
  auto timer = Timer<std::chrono::seconds, void>(
      time_out, std::bind(&CudaFinder::on_time_out, this));

  timer.start();
  find();
  yield();
  timer.stop();

  if (verify_cuda())
  {
    m_logger->log("Valid CUDA version ", m_target_version);
    auto inst_dir =
        get_install_dir(m_results_map.find(std::string("nvcc"))->second.path());

    auto cuda_dir = inst_dir / fs::path("cuda");

    if (fs::exists(cuda_dir))
      fs::remove(cuda_dir);
    fs::create_directory_symlink(inst_dir / fs::path(m_target_version),
                                 cuda_dir);
    m_logger->log("Created symlink ", cuda_dir, " -> ",
                  inst_dir / fs::path(m_target_version));
    return true;
  }
  m_logger->log("Not valid CUDA version :", m_target_version);
  return false;
}

bool CudaFinder::verify_cuda()
{
  if (m_lib64_found && m_nvcc_found)
  {
    auto nvcc = m_results_map.find("nvcc")->second.path();
    if (system(std::string(nvcc.string() + " -V").c_str()) == 0)
      return true;
  }
  return false;
}

bool CudaFinder::is_lib64(const fs::path &path_)
{
  if (!m_lib64_found &&
      path_.stem().string().find("lib64") != std::string::npos &&
      path_.string().find(m_target_version + "/lib64") != std::string::npos &&
      !path_.has_extension() &&
#ifdef GCC7
      fs::is_directory(path_)
#else
      fs::directory_entry(path_).is_directory()
#endif
  )
  {
    m_lib64_found = true;
    m_logger->log("Found lib64 ->\t", path_.string());
    return true;
  }
  return false;
}

bool CudaFinder::is_nvcc(const fs::path &path_)
{
  if (!m_nvcc_found && path_.string().find("nvcc") != std::string::npos &&
      !path_.has_extension() &&
#ifdef GCC7
      fs::is_regular_file(path_)
#else
      fs::directory_entry(path_).is_regular_file()
#endif
)

  {
    m_nvcc_found = true;
    m_logger->log("Found nvcc ->\t", path_.string());
    return true;
  }
  return false;
}

std::map<std::string, fs::directory_entry> CudaFinder::yield()
{
  for (auto &thr : m_workers)
    thr.join();
  return m_results_map;
}

fs::path CudaFinder::get_install_dir(const fs::path &path_)
{
  fs::path o_path;
  for (auto &p : path_)
    if (p.string() == m_target_version)
      break;
    else
      o_path /= p;
  return o_path;
}

void CudaFinder::on_time_out()
{
  m_working = false;
  m_logger->log(ERROR("Time out searching files"));
}