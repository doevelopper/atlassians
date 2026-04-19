#include <com/github/doevelopper/atlassians/atlas/concurrency/CleanupTest.hpp>


using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr CleanupTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.CleanupTest" ) );

CleanupTest::CleanupTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

CleanupTest::~CleanupTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}


TEST_P(CleanupTest, CoroutineUnwind)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
     for(int i = 0; i < 10; ++i)
     {
         getDispatcher().post([i](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
         {
              ctx->sleep(ms(100));
              return (EXIT_SUCCESS);
          });
     }
     std::this_thread::sleep_for(ms(10));
     getDispatcher().terminate();
}

//This test **must** come last to make Valgrind happy.
TEST_P(CleanupTest, DeleteDispatcherInstance)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    DispatcherSingleton::deleteInstances();
}

INSTANTIATE_TEST_CASE_P(CleanupTest_Default,
                        CleanupTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));