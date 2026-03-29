#include <cfs/osal/quantum/CoreTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr CoreTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.CoreTest" ) );

CoreTest::CoreTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

CoreTest::~CoreTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

std::string CoreTest::DummyCoro2(Bloomberg::quantum::VoidContextPtr)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    return "test";
}

int CoreTest::DummyCoro(Bloomberg::quantum::CoroContext<int>::Ptr ctx)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    UNUSED(ctx);
    return 0;
}

int CoreTest::DummyIoTask(Bloomberg::quantum::ThreadPromise<int>::Ptr promise)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    UNUSED(promise);
    std::this_thread::sleep_for(ms(10));
    return (EXIT_SUCCESS);
}

void CoreTest::runCoro(Bloomberg::quantum::Traits::Coroutine & coro, std::mutex& m, int& end, int start)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    int var = start;
    while(end<20) 
    {
        m.lock();
        if (coro)
            coro(var);
        m.unlock();
        sleep(1);
    }
}

TEST_P(CoreTest, Constructor)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    //Check if we have 0 coroutines and IO tasks running
    EXPECT_EQ(0, (int)getDispatcher().size(Bloomberg::quantum::IQueue::QueueType::Coro));
    EXPECT_EQ(0, (int)getDispatcher().size(Bloomberg::quantum::IQueue::QueueType::IO));
    EXPECT_EQ(0, (int)getDispatcher().size());
}

TEST_P(CoreTest, CheckReturnValue)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::IThreadContext<std::string>::Ptr tctx = getDispatcher().post(CoreTest::DummyCoro2);
    std::string s = tctx->get();
    EXPECT_STREQ("test", s.c_str());
}

TEST_P(CoreTest, CheckNumThreads)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::IThreadContext<int>::Ptr tctx = getDispatcher().post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int{
        EXPECT_EQ(DispatcherSingleton::numCoro, ctx->getNumCoroutineThreads());
        EXPECT_EQ(DispatcherSingleton::numThreads, ctx->getNumIoThreads());
        return (EXIT_SUCCESS);
    });
    EXPECT_EQ(DispatcherSingleton::numCoro, tctx->getNumCoroutineThreads());
    EXPECT_EQ(DispatcherSingleton::numThreads, tctx->getNumIoThreads());
}

TEST_P(CoreTest, CheckCoroutineQueuing)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    //Post various IO tasks and coroutines and make sure they executed on the proper queues
    for (int i = 0; i < 3; ++i)
    {
        getDispatcher().post(0, false, CoreTest::DummyCoro);
    }
    getDispatcher().post(1, true, CoreTest::DummyCoro);
    getDispatcher().post(2, false, CoreTest::DummyCoro);
    getDispatcher().drain();
    
    //Posted
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 2).postedCount());
    EXPECT_EQ((size_t)5, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).postedCount()); //total
    
    //Completed
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).completedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).completedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 2).completedCount());
    EXPECT_EQ((size_t)5, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).completedCount()); //total
    
    //Errors
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 2).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).errorCount()); //total
    
    //High Priority
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).highPriorityCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 2).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).highPriorityCount()); //total
    
    //Check if all tasks have stopped
    EXPECT_EQ((size_t)0, getDispatcher().size(Bloomberg::quantum::IQueue::QueueType::Coro));
}

TEST_P(CoreTest, CheckIoQueuing)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    //IO (10 tasks)
    for (int i = 0; i < 10; ++i)
    {
        getDispatcher().postAsyncIo(CoreTest::DummyIoTask); //shared queue
    }
    getDispatcher().postAsyncIo(1, true, CoreTest::DummyIoTask);
    getDispatcher().postAsyncIo(2, false, CoreTest::DummyIoTask);
    
    getDispatcher().drain();
    
    //Posted
    EXPECT_EQ((size_t)10, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, (int)Bloomberg::quantum::IQueue::QueueId::Any).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).postedCount());
    EXPECT_EQ((size_t)12, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).postedCount()); //total
    
    //Completed
    EXPECT_LE((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, (int)Bloomberg::quantum::IQueue::QueueId::Any).completedCount());
    EXPECT_EQ((size_t)10, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, (int)Bloomberg::quantum::IQueue::QueueId::Any).completedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 0).sharedQueueCompletedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).sharedQueueCompletedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).sharedQueueCompletedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 3).sharedQueueCompletedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 4).sharedQueueCompletedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).completedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).completedCount());
    EXPECT_EQ((size_t)12, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).completedCount() +
                          getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).sharedQueueCompletedCount()); //total
    
    //Errors
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, (int)Bloomberg::quantum::IQueue::QueueId::Any).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).errorCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).errorCount()); //total
    
    //High Priority
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, (int)Bloomberg::quantum::IQueue::QueueId::Any).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).highPriorityCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).highPriorityCount()); //total
    
    //Check if all tasks have stopped
    EXPECT_EQ((size_t)0, getDispatcher().size(Bloomberg::quantum::IQueue::QueueType::IO));
}

TEST_P(CoreTest, CheckQueuingFromSameCoroutine)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    getDispatcher().post(0, false, [](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        //Test with VoidContext
        Bloomberg::quantum::Util::makeVoidContext<int>(ctx)->postFirst(1, true, CoreTest::DummyCoro)->then(CoreTest::DummyCoro)->finally(CoreTest::DummyCoro)->end();
        return (EXIT_SUCCESS);
    });
    getDispatcher().drain();
    
    //Posted
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).postedCount());
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).postedCount());
    EXPECT_EQ((size_t)4, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).postedCount()); //total
    
    //High priority
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).highPriorityCount());
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 1).highPriorityCount());
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).highPriorityCount()); //total
}

TEST_P(CoreTest, CheckIoQueuingFromACoroutine)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    getDispatcher().post(0, false, [](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        ctx->postAsyncIo(1, true, DummyIoTask);
        ctx->postAsyncIo(2, false, DummyIoTask);
        ctx->postAsyncIo(3, true, DummyIoTask);
        return 0;
    });
    getDispatcher().drain();
    
    //Posted
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).postedCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 3).postedCount());
    
    //High priority
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro, 0).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 1).highPriorityCount());
    EXPECT_EQ((size_t)0, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 2).highPriorityCount());
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO, 3).highPriorityCount());
    
    //Completed count
    EXPECT_EQ((size_t)1, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).completedCount());
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).completedCount());
    
    //Remaining
    EXPECT_EQ((size_t)0, getDispatcher().size());
}

TEST_P(CoreTest, CheckCoroutineErrors)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::string s("original"); //string must remain unchanged
    
    getDispatcher().post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::string& str)->int {
        ctx->yield();
        return 1; //error! coroutine must stop here
        str = "changed";
        return 0;
    }, s);
    
    getDispatcher().post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx, std::string& str)->int {
        ctx->yield(); //test yield via the VoidContext
        throw std::exception(); //error! coroutine must stop here
        str = "changed";
        return 0;
    }, s);
    
    getDispatcher().post([](Bloomberg::quantum::VoidContextPtr ctx, std::string& str)->std::string {
        ctx->yield(); //test yield via the VoidContext
        throw std::exception(); //error! coroutine must stop here
        str = "changed";
        return str;
    }, s);
    
    getDispatcher().postAsyncIo([](Bloomberg::quantum::ThreadPromise<int>::Ptr, std::string& str)->int {
        std::this_thread::sleep_for(ms(10));
        return 1; //error! coroutine must stop here
        str = "changed";
        return 0;
    }, s);
    
    getDispatcher().postAsyncIo([](Bloomberg::quantum::ThreadPromise<int>::Ptr, std::string& str)->int {
        std::this_thread::sleep_for(ms(10));
        throw std::exception(); //error! coroutine must stop here
        str = "changed";
        return 0;
    }, s);
    
    getDispatcher().postAsyncIo([](std::string& str)->std::string {
        std::this_thread::sleep_for(ms(10));
        throw std::exception(); //error! coroutine must stop here
        str = "changed";
        return str;
    }, s);
    
    getDispatcher().drain();
    
    //Error count
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::Coro).errorCount());
    EXPECT_EQ((size_t)3, getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).errorCount() +
                         getDispatcher().stats(Bloomberg::quantum::IQueue::QueueType::IO).sharedQueueErrorCount());
    EXPECT_STREQ("original", s.c_str());
    
    //Remaining
    EXPECT_EQ((size_t)0, getDispatcher().size());
}

TEST_P(CoreTest, ResumeFromTwoThreads)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    // int end{0};
    // std::mutex m;
    // Bloomberg::quantum::Traits::Coroutine coro([&end](Bloomberg::quantum::Traits::Yield & yield) {
    //     while (1) 
    //     {
    //         LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__ 
    //             << "Running on thread: " << std::hex << std::this_thread::get_id() 
    //             << " value: " << yield.get()++ << " iteration: " << end++);
    //         yield();
    //     }
    // });
    // std::thread t1(CoreTest::runCoro, std::ref(coro), std::ref(m), std::ref(end), 0);
    // std::thread t2(CoreTest::runCoro, std::ref(coro), std::ref(m), std::ref(end), 100);

    // t1.join();
    // t2.join();

    LOG4CXX_DEBUG( logger, __LOG4CXX_FUNC__  << " Done" );
}

INSTANTIATE_TEST_CASE_P(CoreTest_Default,
                        CoreTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));