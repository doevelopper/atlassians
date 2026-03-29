#include <cfs/osal/quantum/StressTestBalanced.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr StressTestBalanced::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.StressTestBalanced" ) );

StressTestBalanced::StressTestBalanced ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

StressTestBalanced::~StressTestBalanced ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

INSTANTIATE_TEST_CASE_P(StressTestBalanced_Default,
                        StressTestBalanced,
                        ::testing::Values(TestConfiguration(true, false),
                                          TestConfiguration(true, true)));

TEST_P(StressTestBalanced, AsyncIoAnyQueueLoadBalance)
{
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