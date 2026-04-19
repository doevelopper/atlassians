#include <com/github/doevelopper/atlassians/atlas/concurrency/StressTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr StressTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.StressTest" ) );

StressTest::StressTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

StressTest::~StressTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

int StressTest::recursiveFibonacci(Bloomberg::quantum::CoroContext<size_t>::Ptr ctx, size_t fib)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    ctx->sleep(us(100));

    if (fib <= 2)
    {
        return ctx->set(1);
    }
    else
    {
        //Post both branches of the Fibonacci series before blocking on get().
        auto ctx1 = ctx->post(recursiveFibonacci, fib - 2);
        auto ctx2 = ctx->post(recursiveFibonacci, fib - 1);
        return ctx->set(ctx1->get(ctx) + ctx2->get(ctx));
    }
}

int StressTest::sequentialFibonacci(Bloomberg::quantum::CoroContext<size_t>::Ptr ctx, size_t fib)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    size_t a = 0, b = 1, c, i;
    for (i = 2; i <= fib; i++)
    {
        c = a + b;
        a = b;
        b = c;
    }
    return ctx->set(c);    
}

TEST_P(StressTest, ParallelFibonacciSerie)
{
    GTEST_SKIP();
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    
    for (int i = 0; i < 1; ++i)
    {
        Bloomberg::quantum::ThreadContext<size_t>::Ptr tctx = dispatcher.post(StressTest::sequentialFibonacci, fibInput);
        if (i == 0)
        {
            //Check once
            size_t num = tctx->get();
            EXPECT_EQ((size_t)fibValues[fibInput], num);
        }
    }
    dispatcher.drain();
    EXPECT_TRUE(dispatcher.empty());
    EXPECT_EQ((size_t)0, dispatcher.size());
}

TEST_P(StressTest, RecursiveFibonacciSerie)
{
    GTEST_SKIP();
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::ThreadContext<size_t>::Ptr tctx = getDispatcher().post(StressTest::recursiveFibonacci, fibInput);
    EXPECT_EQ((size_t)fibValues[fibInput], tctx->get());
}

// #ifdef BOOST_USE_VALGRIND
//     int ioLoops = 10;
//     int batchNum = 10;
// #else
//     int ioLoops = 10000;
//     int batchNum = 1000;
// #endif

TEST_P(StressTest, AsyncIo)
{
    GTEST_SKIP();
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::mutex m;
    std::set<std::pair<int, int>> s; //queueId,iteration
    std::vector<std::pair<int, int>> v;
    v.reserve(ioLoops);
    for (int i = 0; i < ioLoops; ++i) 
    {
        int queueId = i % getDispatcher().getNumIoThreads();
        getDispatcher().postAsyncIo(queueId, false, [&m,&v,&s,queueId,i](Bloomberg::quantum::ThreadPromise<int>::Ptr promise){
            {
                std::lock_guard<std::mutex> lock(m);
                s.insert(std::make_pair(queueId, i));
                v.push_back(std::make_pair(queueId, i));
            }
            return promise->set(0);
        });
    }
    getDispatcher().drain();
    EXPECT_EQ(ioLoops, (int)v.size());
    EXPECT_EQ(ioLoops, (int)s.size()); //all elements unique
}

TEST_P(StressTest, AsyncIoAnyQueue)
{
    GTEST_SKIP();
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::mutex m;
    std::set<std::pair<int, int>> s; //queueId,iteration
    std::vector<std::pair<int, int>> v;
    v.reserve(ioLoops);
    for (int i = 0; i < ioLoops; ++i) 
    {
        int queueId = i % getDispatcher().getNumIoThreads();
        getDispatcher().postAsyncIo([&m,&v,&s,queueId,i](Bloomberg::quantum::ThreadPromise<int>::Ptr promise){
            {
                std::lock_guard<std::mutex> lock(m);
                s.insert(std::make_pair(queueId, i));
                v.push_back(std::make_pair(queueId, i));
            }
            return promise->set(0);
        });
    }
    getDispatcher().drain();
    EXPECT_EQ(ioLoops, (int)v.size());
    EXPECT_EQ(ioLoops, (int)s.size()); //all elements unique
}

INSTANTIATE_TEST_CASE_P(StressTest_Default,
                        StressTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));