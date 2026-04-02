
#include <com/github/doevelopper/atlassians/atlas/utils/TaskTest.hpp>

#include <gmock/gmock.h>

#include <atomic>
#include <stdexcept>

using namespace com::github::doevelopper::atlassians::atlas::utils;
using namespace com::github::doevelopper::atlassians::atlas::utils::test;

namespace
{
class MockRunnable final : public IRunnable
{
public:
    MOCK_METHOD(void, run, (), (override));
};

class MockSchedulable final : public ISchedulable
{
public:
    MOCK_METHOD(void, schedule, (), (override));
    MOCK_METHOD(void, cancel, (), (override));
};
} // namespace

// log4cxx::LoggerPtr TaskTest::logger =
//     log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.atlas.utils.test.TaskTest"));

TaskTest::TaskTest() : testee()
{
    // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TaskTest::~TaskTest()
{
    // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void TaskTest::SetUp()
{
    // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    testee = std::make_unique<Task>([] {}, "TestTask");
    ASSERT_NE(testee, nullptr);
}

void TaskTest::TearDown()
{
    // LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    testee.reset();
}

TEST_F(TaskTest, Constructor_InitialStateIdleAndName)
{
    EXPECT_EQ(testee->getName(), "TestTask");
    EXPECT_EQ(testee->getState(), Task::State::Idle);
    EXPECT_EQ(testee->getRunCount(), 0U);
    EXPECT_FALSE(testee->isScheduled());
}

TEST_F(TaskTest, IRunnableMock_CanBeUsedThroughInterface)
{
    MockRunnable runnable;
    IRunnable *runnableInterface = &runnable;

    EXPECT_CALL(runnable, run()).Times(1);

    runnableInterface->run();
}

TEST_F(TaskTest, ISchedulableMock_CanBeUsedThroughInterface)
{
    MockSchedulable schedulable;
    ISchedulable *schedulableInterface = &schedulable;

    EXPECT_CALL(schedulable, schedule()).Times(1);
    EXPECT_CALL(schedulable, cancel()).Times(1);

    schedulableInterface->schedule();
    schedulableInterface->cancel();
}

TEST_F(TaskTest, RunWithoutSchedule_DoesNotExecuteAction)
{
    std::atomic<int> counter{0};
    Task task([&counter] { ++counter; }, "CounterTask");

    task.run();

    EXPECT_EQ(counter.load(), 0);
    EXPECT_EQ(task.getRunCount(), 0U);
    EXPECT_EQ(task.getState(), Task::State::Idle);
}

TEST_F(TaskTest, ScheduleThenRun_ExecutesActionAndCompletes)
{
    std::atomic<int> counter{0};
    Task task([&counter] { ++counter; }, "CounterTask");

    task.schedule();
    task.run();

    EXPECT_EQ(counter.load(), 1);
    EXPECT_EQ(task.getState(), Task::State::Completed);
    EXPECT_EQ(task.getRunCount(), 1U);
    EXPECT_FALSE(task.getLastError().has_value());
}

TEST_F(TaskTest, Cancel_PreventsExecution)
{
    std::atomic<int> counter{0};
    Task task([&counter] { ++counter; }, "CounterTask");

    task.cancel();
    task.run();

    EXPECT_EQ(counter.load(), 0);
    EXPECT_EQ(task.getState(), Task::State::Cancelled);
    EXPECT_EQ(task.getRunCount(), 0U);
}

TEST_F(TaskTest, CancelDuringRun_LeavesCancelledState)
{
    Task *taskPtr = nullptr;
    Task task([&taskPtr] {
        if (taskPtr)
        {
            taskPtr->cancel();
        }
    }, "CancelTask");

    taskPtr = &task;

    task.schedule();
    task.run();

    EXPECT_EQ(task.getState(), Task::State::Cancelled);
    EXPECT_EQ(task.getRunCount(), 1U);
}

TEST_F(TaskTest, RunWhenActionThrows_SetsFailedAndError)
{
    Task task([] { throw std::runtime_error("boom"); }, "FailingTask");

    task.schedule();
    task.run();

    EXPECT_EQ(task.getState(), Task::State::Failed);
    auto lastError = task.getLastError();
    ASSERT_TRUE(lastError.has_value());
    EXPECT_EQ(lastError.value(), "boom");
    EXPECT_EQ(task.getRunCount(), 1U);
}

TEST_F(TaskTest, ScheduleResetsErrorAndAllowsRerun)
{
    std::atomic<int> counter{0};
    bool shouldThrow = true;
    Task task([&] {
        if (shouldThrow)
        {
            shouldThrow = false;
            throw std::runtime_error("boom");
        }
        ++counter;
    }, "RetryTask");

    task.schedule();
    task.run();

    EXPECT_EQ(task.getState(), Task::State::Failed);
    EXPECT_TRUE(task.getLastError().has_value());

    task.schedule();
    task.run();

    EXPECT_EQ(task.getState(), Task::State::Completed);
    EXPECT_FALSE(task.getLastError().has_value());
    EXPECT_EQ(counter.load(), 1);
    EXPECT_EQ(task.getRunCount(), 2U);
}

TEST_F(TaskTest, StatusTracksExecutionTimestamps)
{
    Task task([] {}, "TimingTask");

    task.schedule();
    task.run();

    auto status = task.getStatus();
    EXPECT_EQ(status.runCount, 1U);
    EXPECT_TRUE(status.lastStarted >= std::chrono::steady_clock::time_point{});
    EXPECT_TRUE(status.lastFinished >= status.lastStarted);
}
