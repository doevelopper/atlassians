#include <com/github/doevelopper/atlassians/atlas/concurrency/FutureJoinerTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr FutureJoinerTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.FutureJoinerTest" ) );

FutureJoinerTest::FutureJoinerTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

FutureJoinerTest::~FutureJoinerTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}


TEST_P(FutureJoinerTest, JoinThreadFutures)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::vector<Bloomberg::quantum::ThreadContext<int>::Ptr> futures;
    
    for (int i = 0; i < 10; ++i) {
        futures.push_back(getDispatcher().post([i](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
            ctx->sleep(std::chrono::milliseconds(10));
            return ctx->set(i);
        }));
    }
    
    std::vector<int> output = Bloomberg::quantum::FutureJoiner<int>()(getDispatcher(), std::move(futures))->get();
    EXPECT_EQ(output, std::vector<int>({0,1,2,3,4,5,6,7,8,9}));
}

TEST_P(FutureJoinerTest, JoinCoroFutures)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    std::vector<int> output;
    
    getDispatcher().post([&output](Bloomberg::quantum::CoroContext<double>::Ptr ctx)->int {
        std::vector<Bloomberg::quantum::CoroContext<int>::Ptr> futures;
        for (int i = 0; i < 10; ++i) 
        {
            futures.push_back(ctx->post([i](Bloomberg::quantum::CoroContext<int>::Ptr ctx2)->int {
                ctx2->sleep(std::chrono::milliseconds(10));
                return ctx2->set(i);
            }));
        }
        output = Bloomberg::quantum::FutureJoiner<int>()(*ctx, std::move(futures))->get(ctx);
        return ctx->set(0);
    })->get();
    
    EXPECT_EQ(output, std::vector<int>({0,1,2,3,4,5,6,7,8,9}));
}

INSTANTIATE_TEST_CASE_P(FutureJoinerTest_Default,
                        FutureJoinerTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));