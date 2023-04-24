#include "gtest/gtest.h"
#include "glog/logging.h"
#include "defs.h"

// main
int main(int argc, char **argv) {
  google::InitGoogleLogging(argv[0]);
  if(strcmp(PLATFORM_OS, "windows") == 0) {
    FLAGS_log_dir = PROJECT_ROOT_DIR + std::string("log\\test");
  } else if(strcmp(PLATFORM_OS, "linux") == 0) {
    FLAGS_log_dir = PROJECT_ROOT_DIR + std::string("log/test");
  }
  ::testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  std::cout << PLATFORM_OS << std::endl;
  std::vector<std::string> dirs = google::GetLoggingDirectories();
  for (auto& dir : dirs) {
    std::cout << dir << std::endl;
  }
  google::ShutdownGoogleLogging();
  return 0;
}