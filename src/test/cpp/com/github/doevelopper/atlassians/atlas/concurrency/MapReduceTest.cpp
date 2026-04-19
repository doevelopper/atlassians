#include <com/github/doevelopper/atlassians/atlas/concurrency/MapReduceTest.hpp>

using namespace cfs::osal::quantum::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr MapReduceTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.MapReduceTest" ) );

MapReduceTest::MapReduceTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

MapReduceTest::~MapReduceTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TEST_P(MapReduceTest, OccuranceCount)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    //count the number of times a word of a specific length occurs
    std::vector<std::vector<std::string>> input = {
        {"a", "b", "aa", "aaa", "cccc" },
        {"bb", "bbb", "bbbb", "a", "bb"},
        {"aaa", "bb", "eee", "cccc", "d", "ddddd"},
        {"eee", "d", "a" }
    };
    
    std::map<std::string, size_t> result = getDispatcher().mapReduce(input.begin(), input.size(),
        //mapper
        [](Bloomberg::quantum::VoidContextPtr, const std::vector<std::string>& input)->std::vector<std::pair<std::string, size_t>>
        {
            std::vector<std::pair<std::string, size_t>> out;
            for (auto&& i : input) {
                out.push_back({i, 1});
            }
            return out;
        },
        //reducer
        [](Bloomberg::quantum::VoidContextPtr, std::pair<std::string, std::vector<size_t>>&& input)->std::pair<std::string, size_t>
        {
            size_t sum = 0;
            for (auto&& i : input.second) {
                sum += i;
            }
            return {std::move(input.first), sum};
        })->get();
    
    ASSERT_EQ(result.size(), 11UL);
    EXPECT_EQ(result["a"], 3UL);
    EXPECT_EQ(result["aa"], 1UL);
    EXPECT_EQ(result["aaa"], 2UL);
    EXPECT_EQ(result["b"], 1UL);
    EXPECT_EQ(result["bb"], 3UL);
    EXPECT_EQ(result["bbb"], 1UL);
    EXPECT_EQ(result["bbbb"], 1UL);
    EXPECT_EQ(result["cccc"], 2UL);
    EXPECT_EQ(result["d"], 2UL);
    EXPECT_EQ(result["ddddd"], 1UL);
    EXPECT_EQ(result["eee"], 2UL);
}

TEST_P(MapReduceTest, WordLength)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    //count the number of times each string occurs
    std::vector<std::vector<std::string>> input = {
        {"a", "b", "aa", "aaa", "cccc" },
        {"bb", "bbb", "bbbb", "a", "bb"},
        {"aaa", "bb", "eee", "cccc", "d", "ddddd"},
        {"eee", "d", "a" }
    };
    
    std::map<size_t, size_t> result = getDispatcher().mapReduceBatch(input.begin(), input.size(),
        //mapper
        [](Bloomberg::quantum::VoidContextPtr, const std::vector<std::string>& input)->std::vector<std::pair<size_t, std::string>>
        {
            std::vector<std::pair<size_t, std::string>> out;
            for (auto&& i : input) {
                out.push_back({i.size(), i});
            }
            return out;
        },
        //reducer
        [](Bloomberg::quantum::VoidContextPtr, std::pair<size_t, std::vector<std::string>>&& input)->std::pair<size_t, size_t>
        {
            return {input.first, input.second.size()};
        })->get();
    
    ASSERT_EQ(result.size(), 5UL); //longest word 'ddddd'
    EXPECT_EQ(result[1], 6UL);
    EXPECT_EQ(result[2], 4UL);
    EXPECT_EQ(result[3], 5UL);
    EXPECT_EQ(result[4], 3UL);
    EXPECT_EQ(result[5], 1UL);
}

TEST_P(MapReduceTest, WordLengthFromCoroutine)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );

    //count the number of times each string occurs
    std::vector<std::vector<std::string>> input = {
        {"a", "b", "aa", "aaa", "cccc" },
        {"bb", "bbb", "bbbb", "a", "bb"},
        {"aaa", "bb", "eee", "cccc", "d", "ddddd"},
        {"eee", "d", "a" }
    };
    
    getDispatcher().post([input](Bloomberg::quantum::CoroContext<int>::Ptr ctx)->int
    {
        std::map<size_t, size_t> result = ctx->mapReduceBatch(input.begin(), input.size(),
        //mapper
        [](Bloomberg::quantum::VoidContextPtr, const std::vector<std::string>& input)->std::vector<std::pair<size_t, std::string>>
        {
            std::vector<std::pair<size_t, std::string>> out;
            for (auto&& i : input) {
                out.push_back({i.size(), i});
            }
            return out;
        },
        //reducer
        [](Bloomberg::quantum::VoidContextPtr, std::pair<size_t, std::vector<std::string>>&& input)->std::pair<size_t, size_t>
        {
            return {input.first, input.second.size()};
        })->get(ctx);
        
        EXPECT_EQ(result.size(), 5UL); //longest word 'ddddd'
        EXPECT_EQ(result[1], 6UL);
        EXPECT_EQ(result[2], 4UL);
        EXPECT_EQ(result[3], 5UL);
        EXPECT_EQ(result[4], 3UL);
        EXPECT_EQ(result[5], 1UL);
        
        return ctx->set(0);
    
    })->get();
}

INSTANTIATE_TEST_CASE_P(MapReduce_Default,
                        MapReduceTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));