#include <cfs/osal/quantum/CoroLocalStorageTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr CoroLocalStorageTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.CoroLocalStorageTest" ) );

CoroLocalStorageTest::CoroLocalStorageTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

CoroLocalStorageTest::~CoroLocalStorageTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TEST_P(CoroLocalStorageTest, AccessTest)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    for(int globalCounter = 0; globalCounter < 100; ++globalCounter)
    {
        getDispatcher().post([globalCounter](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
        {
            static const std::string globalCounterName = "globalCounter";            
            static const std::string localCounterName = "localCounter";
            // make sure nothing is inherited from the previous tasks
            EXPECT_EQ(nullptr, Bloomberg::quantum::local::variable<int>(globalCounterName));
            EXPECT_EQ(nullptr, Bloomberg::quantum::local::variable<int>(localCounterName));

            // set the local variable that remains constant
            int globalCounterCopy = globalCounter;
            Bloomberg::quantum::local::variable<int>(globalCounterName) = &globalCounterCopy;

            int i = 0;
            // set the local variable that is changed in every iteration
            Bloomberg::quantum::local::variable<int>(localCounterName) = &i;

            for(i = 0; i < 10; ++i)
            {
                ctx->sleep(ms(10));

                int* localCounterValue = Bloomberg::quantum::local::variable<int>(localCounterName);
                EXPECT_EQ(&i, localCounterValue);

                int* globalCounterValue = Bloomberg::quantum::local::variable<int>(globalCounterName);
                EXPECT_EQ(&globalCounterCopy, globalCounterValue);
            }

            // exit abnormally from time to time
            if (globalCounter % 5 == 0)
            {
                throw std::runtime_error("test");
            }

            return ctx->set(0);
        });
    }
    getDispatcher().drain();
}

TEST_P(CoroLocalStorageTest, GuardTest)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    getDispatcher().post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
    {
        const std::string name = "v";
        int v = 1;
        Bloomberg::quantum::local::VariableGuard<int> guard1(name, &v);
        for(int i = 0; i < 10; ++i)
        {
            EXPECT_EQ(&v, Bloomberg::quantum::local::variable<int>(name));
            Bloomberg::quantum::local::VariableGuard<int> guard2(name, &i);

            for(int j = 0; j < 10; ++j)
            {
                EXPECT_EQ(&i, Bloomberg::quantum::local::variable<int>(name));
                Bloomberg::quantum::local::VariableGuard<int> guard3(name, &j);
                EXPECT_EQ(&j, Bloomberg::quantum::local::variable<int>(name));
            }
            EXPECT_EQ(&i, Bloomberg::quantum::local::variable<int>(name));
        }
        EXPECT_EQ(&v, Bloomberg::quantum::local::variable<int>(name));
        return ctx->set(0);
    })->get();
}

TEST_P(CoroLocalStorageTest, GetContext)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    EXPECT_EQ(nullptr, Bloomberg::quantum::local::context());
    
    getDispatcher().post([](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
    {
        EXPECT_NE(nullptr, Bloomberg::quantum::local::context());
        EXPECT_EQ((Bloomberg::quantum::CoroContext<Bloomberg::quantum::Void>*)ctx.get(), Bloomberg::quantum::local::context().get());
        return ctx->set(0);
    })->get();
}



INSTANTIATE_TEST_CASE_P(CoroLocalStorageTest_Default,
                        CoroLocalStorageTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));