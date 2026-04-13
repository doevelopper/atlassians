#include <com/github/doevelopper/atlassians/atlas/concurrency/MutexTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr MutexTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.MutexTest" ) );

MutexTest::MutexTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

MutexTest::~MutexTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TEST_P(MutexTest, LockingAndUnlocking)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    std::vector<int> v;
    
    Bloomberg::quantum::Mutex m;
    
    //lock the vector before posting the coroutines
    m.lock();
    v.push_back(5);
    
    //start a couple of coroutines waiting to access the vector
    dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx, Bloomberg::quantum::Mutex& mu, std::vector<int>& vec)->int{
        mu.lock(ctx);
        vec.push_back(6);
        mu.unlock();
        return 0;
    }, m, v);

    dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx, Bloomberg::quantum::Mutex& mu, std::vector<int>& vec)->int{
        mu.lock(ctx);
        vec.push_back(7);
        mu.unlock();
        return 0;
    }, m, v);
    
    std::this_thread::sleep_for(ms(200));
    m.unlock();
    
    dispatcher.drain(); //wait for completion
    
    ASSERT_EQ((size_t)3, v.size());
    EXPECT_EQ(5, v[0]);
    EXPECT_TRUE((6 == v[1] || 7 == v[1]) && (6 == v[2] || 7 == v[2]));
}

TEST_P(MutexTest, SignalWithConditionVariable)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    std::vector<int> v;
    
    Bloomberg::quantum::Mutex m;
    Bloomberg::quantum::ConditionVariable cv;
    
    //access the vector
    m.lock();
    
    //start a couple of coroutines waiting to access the vector
    dispatcher.post(0, false, [](Bloomberg::quantum::ICoroContext<int>::Ptr ctx, Bloomberg::quantum::Mutex& mu, std::vector<int>& vec, Bloomberg::quantum::ConditionVariable& cv)->int{
        mu.lock(ctx);
        cv.wait(ctx, mu, [&vec]()->bool{ return !vec.empty(); });
        vec.push_back(6);
        mu.unlock();
        return 0;
    }, m, v, cv);
    dispatcher.post(0, false, [](Bloomberg::quantum::ICoroContext<int>::Ptr ctx, Bloomberg::quantum::Mutex& mu, std::vector<int>& vec, Bloomberg::quantum::ConditionVariable& cv)->int{
        mu.lock(ctx);
        cv.wait(ctx, mu, [&vec]()->bool{ return !vec.empty(); });
        vec.push_back(7);
        mu.unlock();
        return 0;
    }, m, v, cv);
    
    std::this_thread::sleep_for(ms(200));
    v.push_back(5);
    m.unlock();
    
    cv.notifyAll();
    dispatcher.drain();
    
    ASSERT_EQ((size_t)3, v.size());
    EXPECT_EQ(5, v[0]);
    EXPECT_TRUE((6 == v[1] || 7 == v[1]) && (6 == v[2] || 7 == v[2]));
}

INSTANTIATE_TEST_CASE_P(MutexTest_Default,
                        MutexTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));