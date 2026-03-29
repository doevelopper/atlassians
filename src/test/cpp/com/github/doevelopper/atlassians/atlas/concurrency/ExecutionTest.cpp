#include <cfs/osal/quantum/ExecutionTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr ExecutionTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.ExecutionTest" ) );

ExecutionTest::ExecutionTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

ExecutionTest::~ExecutionTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

int ExecutionTest::DummyCoro(Bloomberg::quantum::CoroContext<int>::Ptr ctx)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    UNUSED(ctx);
    return 0;
}


TEST_P(ExecutionTest, DrainAllTasks)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    //Turn the drain on and make sure we cannot queue any tasks
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    
    //Post a bunch of coroutines to run and wait for completion.
    for (int i = 0; i < 100; ++i)
    {
        dispatcher.post(ExecutionTest::DummyCoro);
    }
    
    dispatcher.drain();
    EXPECT_EQ((size_t)0, dispatcher.size(Bloomberg::quantum::IQueue::QueueType::Coro));
    EXPECT_EQ((size_t)0, dispatcher.size());
}

TEST_P(ExecutionTest, YieldingBetweenTwoCoroutines)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    //Basic test which verifies cooperation between two coroutines.
    //This also outlines lock-free coding.
    
    auto func = [](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::set<int>& s)->int {
        s.insert(1);
        ctx->yield();
        s.insert(3);
        ctx->yield();
        s.insert(5);
        return 0;
    };
    
    auto func2 = [](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::set<int>& s)->int {
        s.insert(2);
        ctx->yield();
        s.insert(4);
        ctx->yield();
        s.insert(6);
        return 0;
    };
    
    std::set<int> testSet; //this will contain [1,6]
    
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    
    dispatcher.post(3, false, func, testSet);
    dispatcher.post(3, false, func2, testSet);
    dispatcher.drain();
    
    std::set<int> validation{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(validation, testSet);
}

TEST_P(ExecutionTest, ChainCoroutinesFromDispatcher)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1;
    std::vector<int> v;
    std::vector<int> validation{1,2,3,4};
    
    auto func = [](Bloomberg::quantum::CoroContext<int>::Ptr, std::vector<int>& v, int& i)->int
    {
        v.push_back(i++);
        return 0;
    };
    dispatcher.postFirst(func, v, i)->then(func, v, i)->then(func, v, i)->then(func, v, i)->end();
    dispatcher.drain();
    
    //Validate values
    EXPECT_EQ(validation, v);
}

TEST_P(ExecutionTest, ChainCoroutinesFromCoroutineContext)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1, err = 10, final = 20;
    std::vector<int> v;
    std::vector<int> validation{1,2,3,4,20};
    
    auto func2 = [](Bloomberg::quantum::CoroContext<int>::Ptr, std::vector<int>& v, int& i)->int
    {
        v.push_back(i++);
        return 0;
    };
     
    auto func = [&](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::vector<int>& v, int& i)->int
    {
        ctx->postFirst(func2, v, i)->
             then(func2, v, i)->
             then(func2, v, i)->
             then(func2, v, i)->
             onError(func2, v, err)->
             finally(func2, v, final)->
             end(); //OnError *should not* run
        return 0;
    };
    
    dispatcher.post(func, v, i);
    dispatcher.drain();
    
    //Validate values
    EXPECT_EQ(validation, v);
}

TEST_P(ExecutionTest, ChainCoroutinesFromCoroutineContext2)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1, err = 10, final = 20;
    std::vector<int> v;
    std::vector<int> validation{1,2,3,4,20};
    
    auto func2 = [](Bloomberg::quantum::VoidContextPtr, std::vector<int>& v, int& i)->std::vector<int>
    {
        v.push_back(i++);
        return v;
    };
    
    auto func = [&](Bloomberg::quantum::VoidContextPtr ctx, std::vector<int>& v, int& i)->std::vector<int>
    {
        return ctx->postFirst(func2, v, i)->
                    then(func2, v, i)->
                    then(func2, v, i)->
                    then(func2, v, i)->
                    onError(func2, v, err)->
                    finally(func2, v, final)->
                    end()->get(ctx); //OnError *should not* run
    };
    
    dispatcher.post(func, v, i);
    dispatcher.drain();
    
    //Validate values
    EXPECT_EQ(validation, v);
}

TEST_P(ExecutionTest, OnErrorTaskRuns)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1, err = 10, final = 20;
    std::vector<int> v;
    std::vector<int> validation{1,2,10,20}; //includes error
    
    auto func2 = [](Bloomberg::quantum::CoroContext<int>::Ptr, std::vector<int>& v, int& i)->int
    {
        if (i == 3) return -1; //cause an error
        v.push_back(i++);
        return 0;
    };
    
    auto func = [&](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::vector<int>& v, int& i)->int
    {
        ctx->postFirst(func2, v, i)->then(func2, v, i)->then(func2, v, i)->
             then(func2, v, i)->onError(func2, v, err)->finally(func2, v, final)->end(); //OnError *should* run
        return 0;
    };
    
    dispatcher.post(func, v, i);
    dispatcher.drain();
    
    //Validate values
    EXPECT_EQ(validation, v);
}

TEST_P(ExecutionTest, FinallyAlwaysRuns)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1, final = 20;
    std::vector<int> v;
    std::vector<int> validation{1,2,20}; //includes error
    
    auto func2 = [](Bloomberg::quantum::CoroContext<int>::Ptr, std::vector<int>& v, int& i)->int
    {
        if (i == 3) return -1; //cause an error
        v.push_back(i++);
        return 0;
    };
    
    auto func = [&](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::vector<int>& v, int& i)->int
    {
        ctx->postFirst(func2, v, i)->then(func2, v, i)->then(func2, v, i)->
             then(func2, v, i)->finally(func2, v, final)->end(); //OnError *should* run
        return 0;
    };
    
    dispatcher.post(func, v, i);
    dispatcher.drain();
    
    //Validate values
    EXPECT_EQ(validation, v);
}

TEST_P(ExecutionTest, CoroutineSleep)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        ctx->sleep(ms(100));
        return 0;
    });
    
    auto start = std::chrono::steady_clock::now();
    ctx->wait(); //block until value is available
    auto end = std::chrono::steady_clock::now();
    
    //check elapsed time
    size_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    EXPECT_GE(elapsed, (size_t)100);
}

INSTANTIATE_TEST_CASE_P(ExecutionTest_Default,
                        ExecutionTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));