
#include <com/github/doevelopper/premisses/atlas/concurrency/GenericFutureTest.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/Fixture.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency;
using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr GenericFutureTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.GenericFutureTest" ) );

GenericFutureTest::GenericFutureTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

GenericFutureTest::~GenericFutureTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
}

void GenericFutureTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void GenericFutureTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TEST_F(GenericFutureTest, MakeFutureInCoroutineAndMainThread) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    using namespace Bloomberg::quantum;
    Dispatcher& dispatcher = DispatcherSingleton::instance({false, false});
    GenericFuture<int> threadFuture = dispatcher.post([](VoidContextPtr ctx)->int {
        //post an IO task and get future from there
        GenericFuture<int> coroFuture(ctx->postAsyncIo([]()->int {
            return 33;
        }), ctx);
        return coroFuture.get(); //forward the promise to main thread
    });
    EXPECT_EQ(33, threadFuture.get()); //block until value is available    
}

TEST_F(GenericFutureTest, WaitForCoroutineFutureInIoTask) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    using namespace Bloomberg::quantum;
    Dispatcher& dispatcher = DispatcherSingleton::instance({false, false});
    GenericFuture<int> threadFuture = dispatcher.post([](VoidContextPtr ctx)->int {
        //post an IO task and get future from there
        volatile int i = 0;
        GenericFuture<int> coroFuture0(ctx->postAsyncIo([&i]()->int {
            while (i==0) {
                std::this_thread::sleep_for(ms(10));
            }
            return 33;
        }), ctx);
        
        //start another IO task and wait in the previous future
        GenericFuture<int> coroFuture1(ctx->postAsyncIo([&i, &coroFuture0]()->int {
            //wait on this future
            i = 1;
            return coroFuture0.get() + 10; //wait on the coroutine future
        }), ctx);
        
        return coroFuture1.get(); //return value to main thread
    });
    EXPECT_EQ(43, threadFuture.get()); //block until value is available    
}

TEST_F(GenericFutureTest, WaitForIoFutureInCoroutine) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    using namespace Bloomberg::quantum;

    Dispatcher& dispatcher = DispatcherSingleton::instance({false, false});
    volatile int i = 0;
    GenericFuture<int> ioFuture = dispatcher.postAsyncIo([&i]()->int {
        while (i==0) {
            std::this_thread::sleep_for(ms(10));
        }
        return 33;
    });
    
    GenericFuture<int> threadFuture = dispatcher.post([&i, &ioFuture](VoidContextPtr)->int {
        i = 1;
        return ioFuture.get() + 10;
    });
    
    EXPECT_TRUE(threadFuture.valid());
    
    EXPECT_EQ(43, threadFuture.get()); //block until value is available    
}

TEST_F(GenericFutureTest, TestCopyable) 
{
    using namespace Bloomberg::quantum;
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    Dispatcher& dispatcher = DispatcherSingleton::instance({false, false});
    std::vector<GenericFuture<int>> v;
    
    //create one future
    v.push_back(dispatcher.post([](VoidContextPtr)->int{ return 33; }));
    
    //use Promise constructor
    v.push_back(Promise<int>{});
    
    //copy it
    v.push_back(v.front());
    
    //read the value from the first future
    EXPECT_EQ(33, v.front().get());
    
    //read from the second future and we should throw
    EXPECT_THROW(v.back().get(), FutureAlreadyRetrievedException);    
}

TEST_F(GenericFutureTest, Invalid) 
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    using namespace Bloomberg::quantum;
    GenericFuture<int> future; //default constructed
    EXPECT_FALSE(future.valid());    
}