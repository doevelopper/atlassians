
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASKTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASKTEST_HPP

#include <gtest/gtest.h>

#include <memory>

#include <com/github/doevelopper/premisses/atlas/utils/Task.hpp>

namespace com::github::doevelopper::premisses::atlas::utils::test
{
class TaskTest : public ::testing::Test
{
    // LOG4CXX_DECLARE_STATIC_TEST_LOGGER
public:
    TaskTest();
    TaskTest(const TaskTest &) = delete;
    TaskTest(TaskTest &&) = delete;
    TaskTest &operator=(const TaskTest &) = delete;
    TaskTest &operator=(TaskTest &&) = delete;
    ~TaskTest() override;

    void SetUp() override;
    void TearDown() override;

protected:
private:
    std::unique_ptr<com::github::doevelopper::premisses::atlas::utils::Task> testee;
};
} // namespace com::github::doevelopper::premisses::atlas::utils::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_TASKTEST_HPP
