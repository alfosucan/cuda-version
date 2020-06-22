#include <cuda_finder.hpp>

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << "Usage: cuda-version <version>" << std::endl;
    return -1;
  }
  std::string tgt_version(argv[1]);
  if (tgt_version.find("cuda-") == std::string::npos)
    tgt_version = "cuda-" + tgt_version;
  auto finder = CudaFinder(tgt_version);
  try {
    finder.swapVersion(argc >= 3? atoi(argv[2]) : 60);
  } catch (const fs::filesystem_error &e) {
    std::cerr <<"\u001b[31m" <<  e.what() << '\n';
  }

  return 0;
}