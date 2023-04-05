#include "gtest/gtest.h"
#include "glog/logging.h"

TEST(TestFrame, Test1) {
  LOG(INFO) << "TestFrame.Test1";
  EXPECT_EQ(1, 1);
}