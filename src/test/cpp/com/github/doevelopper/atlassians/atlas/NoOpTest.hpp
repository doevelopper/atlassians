
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_NOOPTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_NOOPTEST_HPP

#include <gtest/gtest.h>

#include <memory>

#include <com/github/doevelopper/premisses/atlas/NoOp.hpp>

namespace com::github::doevelopper::premisses::atlas::utils::test {
class NoOpTest : public ::testing::Test {
  //   LOG4CXX_DECLARE_STATIC_TEST_LOGGER
 public:
  NoOpTest();
  NoOpTest(const NoOpTest&) = delete;
  NoOpTest(NoOpTest&&) = delete;
  NoOpTest& operator=(const NoOpTest&) = delete;
  NoOpTest& operator=(NoOpTest&&) = delete;
  ~NoOpTest() override;

  void SetUp() override;
  void TearDown() override;

 protected:
 private:
  std::unique_ptr<com::github::doevelopper::premisses::atlas::utils::NoOp> testee;
};
}  // namespace com::github::doevelopper::premisses::atlas::utils::test
#endif
