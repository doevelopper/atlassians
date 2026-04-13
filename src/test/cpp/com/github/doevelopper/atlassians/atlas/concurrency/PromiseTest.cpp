#include <com/github/doevelopper/atlassians/atlas/concurrency/PromiseTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr PromiseTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.PromiseTest" ) );

PromiseTest::PromiseTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

PromiseTest::~PromiseTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TEST_P(PromiseTest, GetFutureFromCoroutine)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        return ctx->set(55); //Set the promise
    });
    EXPECT_EQ(55, ctx->get()); //block until value is available
    EXPECT_THROW(ctx->get(), Bloomberg::quantum::FutureAlreadyRetrievedException);
}

TEST_P(PromiseTest, GetFutureFromIoTask)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::ThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int{
        //post an IO task and get future from there
        Bloomberg::quantum::CoroFuture<double>::Ptr fut = ctx->postAsyncIo([](Bloomberg::quantum::ThreadPromise<double>::Ptr promise)->int{
            return promise->set(33.22);
        });
        return ctx->set((int)fut->get(ctx)); //forward the promise
    });
    EXPECT_EQ(33, ctx->get()); //block until value is available
}

TEST_P(PromiseTest, GetFutureFromIoTask2)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::ThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::VoidContextPtr ctx)->int{
        //post an IO task and get future from there
        Bloomberg::quantum::CoroFuture<double>::Ptr fut = ctx->postAsyncIo([]()->double{
            return 33.22;
        });
        return (int)fut->get(ctx); //forward the promise
    });
    EXPECT_EQ(33, ctx->get()); //block until value is available
}

TEST_P(PromiseTest, GetFutureFromExternalSource)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::Promise<int> promise;
    Bloomberg::quantum::ThreadContext<int>::Ptr ctx = dispatcher.post([&promise](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int{
        //post an IO task and get future from there
        Bloomberg::quantum::CoroFuture<int>::Ptr future = promise.getICoroFuture();
        return ctx->set(future->get(ctx)); //forward the promise
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    promise.set(33); //set the external promise
    EXPECT_EQ(33, ctx->get()); //block until value is available
}

TEST_P(PromiseTest, BufferedFuture)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::ThreadContext<Bloomberg::quantum::Buffer<int>>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::CoroContext<Bloomberg::quantum::Buffer<int>>::Ptr ctx)->int{
        for (int d = 0; d < 100; d++)
        {
            ctx->push(d);
        }
        return ctx->closeBuffer();
    });
    
    std::vector<int> v;
    while (1)
    {
        bool isBufferClosed = false;
        int value = ctx->pull(isBufferClosed);
        if (isBufferClosed) break;
        v.push_back(value);
    }
    
    //Validate
    size_t num = 0;
    for (auto&& val : v) 
    {
        EXPECT_EQ(num++, (size_t)val);
    }
}

TEST_P(PromiseTest, BufferedFutureException)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::ThreadContext<Bloomberg::quantum::Buffer<double>>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::CoroContext<Bloomberg::quantum::Buffer<double>>::Ptr ctx)->int{
        for (int d = 0; d < 100; d++)
        {
            ctx->push(d);
        }
        // Don't close the buffer but throw instead
        try {
            throw std::runtime_error("42");
        }
        catch (const std::exception&)
        {
            return ctx->setException(std::current_exception());
        }
    });
    
    std::vector<double> v;
    bool wasCaught = false;
    while (1)
    {
        try {
            bool isBufferClosed = false;
            double value = ctx->pull(isBufferClosed);
            if (isBufferClosed) break;
            v.push_back(value);
        }
        catch (const std::exception&) {
            wasCaught = true;
            break;
        }
    }
    
    //Validate
    EXPECT_TRUE(wasCaught);
    EXPECT_GE(100, (int)v.size());
}

TEST_P(PromiseTest, GetFutureReference)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        return ctx->set(55); //Set the promise
    });
    EXPECT_EQ(55, ctx->getRef()); //block until value is available
    EXPECT_NO_THROW(ctx->getRef());
    EXPECT_NO_THROW(ctx->get());
    EXPECT_THROW(ctx->get(), Bloomberg::quantum::FutureAlreadyRetrievedException);
}

TEST_P(PromiseTest, GetIntermediateFutures)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    auto ctx = dispatcher.postFirst([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        return ctx->set(55); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<double>::Ptr ctx)->int {
        return ctx->set(22.33); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<std::string>::Ptr ctx)->int {
        return ctx->set("future"); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<std::list<int>>::Ptr ctx)->int {
        return ctx->set(std::list<int>{1,2,3}); //Set the promise
    })->end();
    
    std::list<int> validate{1,2,3};
    
    EXPECT_EQ(55, ctx->getAt<int>(0));
    EXPECT_DOUBLE_EQ(22.33, ctx->getAt<double>(1));
    EXPECT_THROW(ctx->getAt<double>(1),Bloomberg::quantum::FutureAlreadyRetrievedException); //already retrieved
    EXPECT_STREQ("future", ctx->getAt<std::string>(2).c_str());
    EXPECT_EQ(validate, ctx->getRefAt<std::list<int>>(-1));
    EXPECT_EQ(validate, ctx->get()); //ok - can read value again
}

TEST_P(PromiseTest, GetIntermediateFutures2)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    auto ctx = dispatcher.postFirst([](Bloomberg::quantum::VoidContextPtr)->int {
        return 55; //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<double>::Ptr ctx)->int { //mix with V1 API
        return ctx->set(22.33); //Set the promise
    })->then([](Bloomberg::quantum::VoidContextPtr)->std::string {
        return "future"; //Set the promise
    })->then([](Bloomberg::quantum::VoidContextPtr)->std::list<int> {
        return {1,2,3}; //Set the promise
    })->end();
    
    std::list<int> validate{1,2,3};
    
    EXPECT_EQ(55, ctx->getAt<int>(0));
    EXPECT_DOUBLE_EQ(22.33, ctx->getAt<double>(1));
    EXPECT_THROW(ctx->getAt<double>(1), Bloomberg::quantum::FutureAlreadyRetrievedException); //already retrieved
    EXPECT_STREQ("future", ctx->getAt<std::string>(2).c_str());
    EXPECT_EQ(validate, ctx->getRefAt<std::list<int>>(-1));
    EXPECT_EQ(validate, ctx->get()); //ok - can read value again
}

TEST_P(PromiseTest, GetPreviousFutures)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    auto ctx = dispatcher.postFirst([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        return ctx->set(55); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<double>::Ptr ctx)->int {
        EXPECT_EQ(55, ctx->getPrev<int>());
        return ctx->set(22.33); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<std::string>::Ptr ctx)->int {
        EXPECT_DOUBLE_EQ(22.33, ctx->getPrev<double>());
        return ctx->set("future"); //Set the promise
    })->then([](Bloomberg::quantum::CoroContext<std::list<int>>::Ptr ctx)->int {
        EXPECT_STREQ("future", ctx->getPrevRef<std::string>().c_str());
        return ctx->set(std::list<int>{1,2,3}); //Set the promise
    })->end();
    
    std::list<int> validate{1,2,3};
    EXPECT_EQ(validate, ctx->get()); //ok - can read value again
    EXPECT_STREQ("future", ctx->getAt<std::string>(2).c_str());
}

TEST_P(PromiseTest, BrokenPromiseInAsyncIo)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::ThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int{
        //post an IO task and get future from there
        Bloomberg::quantum::CoroFuture<double>::Ptr fut = ctx->postAsyncIo([](Bloomberg::quantum::ThreadPromise<double>::Ptr)->int{
            //Do not set the promise so that we break it
            return 0;
        });
        EXPECT_THROW(fut->get(ctx), Bloomberg::quantum::BrokenPromiseException); //broken promise
        return 0;
    });
}

TEST_P(PromiseTest, BreakPromiseByThrowingError)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr)->int{
        throw std::runtime_error("don't set the promise");
    });
    EXPECT_THROW(ctx->getRef(), std::runtime_error);
    EXPECT_THROW(ctx->get(), std::runtime_error);
}

TEST_P(PromiseTest, PromiseBrokenWhenOnError)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    int i = 1;
    
    auto func2 = [](Bloomberg::quantum::CoroContext<int>::Ptr ctx, int& i)->int
    {
        UNUSED(ctx);
        if (i == 2) return -1; //cause an error
        return ctx->set(i++);
    };
    
    auto onErrorFunc = [](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
    {
        EXPECT_THROW(ctx->getPrev<int>(), Bloomberg::quantum::BrokenPromiseException);
        return ctx->set(77);
    };
    
    auto func = [&](Bloomberg::quantum::CoroContext<int>::Ptr ctx, int& i)->int
    {
        Bloomberg::quantum::CoroContext<int>::Ptr chain = ctx->postFirst(func2, i)->then(func2, i)->then(func2, i)->
             then(func2, i)->onError(onErrorFunc)->end(); //OnError *should* run
        EXPECT_THROW(chain->getAt<int>(1, ctx),Bloomberg::quantum:: BrokenPromiseException);
        EXPECT_THROW(chain->getAt<int>(2, ctx), Bloomberg::quantum::BrokenPromiseException);
        EXPECT_THROW(chain->getAt<int>(3, ctx), Bloomberg::quantum::BrokenPromiseException);
        EXPECT_NO_THROW(chain->getRef(ctx));
        EXPECT_EQ(77, chain->get(ctx)); //OnError task
        return 0;
    };
    
    dispatcher.post(func, i);
    dispatcher.drain();
}

TEST_P(PromiseTest, SetExceptionInPromise)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        try {
            throw std::runtime_error("42");
        }
        catch (const std::exception&)
        {
            return ctx->setException(std::current_exception());
        }
    });
    EXPECT_THROW(ctx->get(), std::runtime_error);
}

TEST_P(PromiseTest, FutureTimeout)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        ctx->sleep(ms(300));
        return 0;
    });
    
    auto start = std::chrono::steady_clock::now();
    std::future_status status = ctx->waitFor(ms(100)); //block until value is available or 100ms have expired
    auto end = std::chrono::steady_clock::now();
    
    //check elapsed time
    size_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    EXPECT_LT(elapsed, (size_t)300);
    EXPECT_EQ((int)status, (int)std::future_status::timeout);
}

TEST_P(PromiseTest, FutureWithoutTimeout)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    Bloomberg::quantum::Dispatcher& dispatcher = getDispatcher();
    Bloomberg::quantum::IThreadContext<int>::Ptr ctx = dispatcher.post([](Bloomberg::quantum::ICoroContext<int>::Ptr ctx)->int{
        ctx->sleep(ms(100));
        return 0;
    });
    
    auto start = std::chrono::steady_clock::now();
    std::future_status status = ctx->waitFor(ms(300)); //block until value is available or 300ms have expired
    auto end = std::chrono::steady_clock::now();
    
    //check elapsed time
    size_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    EXPECT_GE(elapsed, (size_t)100);
    EXPECT_LT(elapsed, (size_t)300);
    EXPECT_EQ((int)status, (int)std::future_status::ready);
}

TEST_P(PromiseTest, WaitForAllFutures)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    
   Bloomberg::quantum:: Dispatcher& dispatcher = getDispatcher();
    auto func = [](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        ctx->sleep(ms(50));
        return 0;
    };
    
    auto ctx = dispatcher.postFirst(func)->then(func)->then(func)->then(func)->end();
    auto start = std::chrono::steady_clock::now();
    ctx->waitAll(); //block until value is available or 4x50ms has expired
    auto end = std::chrono::steady_clock::now();
    
    //check elapsed time
    size_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    EXPECT_GE(elapsed, (size_t)200);
}

INSTANTIATE_TEST_CASE_P(PromiseTest_Default,
                        PromiseTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));
