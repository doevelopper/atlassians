
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TASKIDTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_TASKIDTEST_HPP

#include <gmock/gmock.h>
#include <com/github/doevelopper/premisses/atlas/concurrency/MockTaskId.hpp>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{
    class TaskIdTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
    public:

        TaskIdTest();
        TaskIdTest(const TaskIdTest &) = default;
        TaskIdTest(TaskIdTest &&) = default;
        TaskIdTest &operator=(const TaskIdTest &) = default;
        TaskIdTest &operator=(TaskIdTest &&) = default;
        virtual ~TaskIdTest();

        void SetUp() override;
        void TearDown() override;

    protected:
        MockTaskId * testee;
    private:
    };
}
#endif
