#include <cfs/osal/quantum/ForEachTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr ForEachTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.ForEachTest" ) );

ForEachTest::ForEachTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

ForEachTest::~ForEachTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}


TEST_P(ForEachTest, Simple)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    std::vector<int> start{0,1,2,3,4,5,6,7,8,9};
    std::vector<char> end{'a','b','c','d','e','f','g','h','i','j'};
    std::vector<char> results = getDispatcher().forEach(start.cbegin(), start.size(),
        [](Bloomberg::quantum::VoidContextPtr, const int& val)->char {
        return 'a'+val;
    })->get();
    EXPECT_EQ(end, results);
}

TEST_P(ForEachTest, SimpleNonConst)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    std::vector<int> start{0,1,2,3,4,5,6,7,8,9};
    std::vector<char> end{'b','c','d','e','f','g','h','i','j','k'};
    std::vector<char> results = getDispatcher().forEach(start.begin(), start.size(),
        [](Bloomberg::quantum::VoidContextPtr ctx, int& val)->char {
        val = ctx->postAsyncIo([&](Bloomberg::quantum::ThreadPromisePtr<int> p){
            return p->set(++val);
        })->get(ctx);
        return 'a'+ val;
    })->get();
    EXPECT_EQ(end, results);
    EXPECT_EQ(1, start[0]);
    EXPECT_EQ(10, start[9]);
}

TEST_P(ForEachTest, SmallBatch)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    std::vector<int> start{0,1,2};
    std::vector<char> end{'a','b','c'};
    
    std::vector<std::vector<char>> results = getDispatcher().forEachBatch(start.cbegin(), start.size(),
        [](Bloomberg::quantum::VoidContextPtr, const int& val)->char
    {
        return 'a'+val;
    })->get();

    ASSERT_EQ(start.size(), results.size());
    EXPECT_EQ(results[0].front(), end[0]);
    EXPECT_EQ(results[1].front(), end[1]);
    EXPECT_EQ(results[2].front(), end[2]);
}

TEST_P(ForEachTest, LargeBatch)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    std::vector<int> start(batchNum);
    
    //Build a large input vector
    for (int i = 0; i < batchNum; ++i) {
        start[i]=i;
    }
    
    std::vector<std::vector<int>> results = getDispatcher().forEachBatch(start.begin(), start.size(),
        [](Bloomberg::quantum::VoidContextPtr, int val)->int {
        return val*2; //double the value
    })->get();
    
    ASSERT_EQ((int)results.size(), getDispatcher().getNumCoroutineThreads());
    
    //Merge batches
    std::vector<int> merged;
    for (auto&& v : results) 
    {
        merged.insert(merged.end(), v.begin(), v.end());
    }
    
    ASSERT_EQ(batchNum, (int)merged.size());
    for (size_t i = 0; i < merged.size(); ++i) 
    {
        EXPECT_EQ(merged[i], start[i]*2);
    }
}

TEST_P(ForEachTest, LargeBatchFromCoroutine)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    getDispatcher().post([this](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int {
        std::vector<int> start(batchNum);
    
        std::vector<std::vector<int>> results = ctx->forEachBatch(start.begin(), start.size(),
            [](Bloomberg::quantum::VoidContextPtr, int val)->int {
            return val*2; //double the value
        })->get(ctx);
        
        EXPECT_EQ(getDispatcher().getNumCoroutineThreads(), (int)results.size());
        
        //Merge batches
        std::vector<int> merged;
        for (auto&& v : results) 
        {
            merged.insert(merged.end(), v.begin(), v.end());
        }
        
        size_t size = merged.size();

        EXPECT_EQ(batchNum, size);

        for (size_t i = 0; i < size; ++i) 
        {
            EXPECT_EQ(merged[i], start[i]*2);
        }
        return ctx->set(0);
    })->get();
}


INSTANTIATE_TEST_CASE_P(ForEachTest_Default,
                        ForEachTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));