#include <cfs/osal/quantum/SharedQueueTest.hpp>
#include <cfs/osal/quantum/DispatcherSingleton.hpp>

using namespace cfs::osal::quantum::test;
using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr SharedQueueTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.SharedQueueTest" ) );

SharedQueueTest::SharedQueueTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

SharedQueueTest::~SharedQueueTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

void SharedQueueTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void SharedQueueTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void SharedQueueTest::enqueue_sleep_tasks(Bloomberg::quantum::Dispatcher& dispatcher,
                         const std::vector<std::pair<size_t, ms>> & sleepTimes)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TEST_F(SharedQueueTest, PerformanceTest) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // The code below enqueues 30 short tasks, then 1 large task, and then 30 short tasks.
    // The intuition is that in the shared-coro mode, while one thread is busy with the large task,
    // the other threads will help it with the short tasks, and as a result
    // the shared-coro dispatcher will finish faster.
    size_t elapsedWithoutCoroSharing;
    size_t elapsedWithCoroSharing;

    const std::vector<std::pair<size_t, ms>> sleepTimes =
        {{30, ms(10)},
         {1, ms(100)},
         {30, ms(10)}};
    {
        const TestConfiguration noCoroSharingConfig(false, false);
        Bloomberg::quantum::Dispatcher& dispatcher = DispatcherSingleton::instance(noCoroSharingConfig);
        dispatcher.drain();
        auto start = std::chrono::steady_clock::now();
        enqueue_sleep_tasks(dispatcher, sleepTimes);
        dispatcher.drain();
        auto end = std::chrono::steady_clock::now();
        elapsedWithoutCoroSharing = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    }
    
    {
        const TestConfiguration coroSharingConfig(false, true);
        Bloomberg::quantum::Dispatcher& dispatcher = DispatcherSingleton::instance(coroSharingConfig);
        dispatcher.drain();
        auto start = std::chrono::steady_clock::now();
        enqueue_sleep_tasks(dispatcher, sleepTimes);
        dispatcher.drain();
        auto end = std::chrono::steady_clock::now();
        elapsedWithCoroSharing = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    }
    EXPECT_LT(elapsedWithCoroSharing, elapsedWithoutCoroSharing);    
}