#include <com/github/doevelopper/premisses/atlas/concurrency/ParamtersTest.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/interface/ICoroContext.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

using ms = std::chrono::milliseconds;
using us = std::chrono::microseconds;

log4cxx::LoggerPtr ParamtersTest::logger = 
    log4cxx::Logger::getLogger ( std::string ( "cfs.osal.quantum.ParamtersTest" ) );

ParamtersTest::ParamtersTest ( ) noexcept
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

ParamtersTest::~ParamtersTest ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TEST_P(ParamtersTest, CheckParameterPassingInCoroutines)
{
    //Test pass by value, reference and address.
    int a = 5;
    std::string str = "original";
    std::string str2 = "original2";
    ParamtersTest::NonCopyable nc("move");
    double dbl = 4.321;

    auto func = [&](Bloomberg::quantum::CoroContext<int>::Ptr ctx, int byVal, std::string& byRef, std::string&& byRvalue, ParamtersTest::NonCopyable&& byRvalueNoCopy, double* byAddress)->int {
        //modify all passed-in values
        EXPECT_EQ(5, byVal);
        byVal = 6;
        EXPECT_NE(a, byVal);
        byRef = "changed";
        EXPECT_EQ(byRef.data(), str.data());
        *byAddress = 6.543;
        EXPECT_NE(str2.c_str(), byRvalue.c_str());
        std::string tempStr(std::move(byRvalue));
        EXPECT_STREQ("original2", tempStr.c_str());
        ParamtersTest::NonCopyable tempStr2 = std::move(byRvalueNoCopy);
        EXPECT_STREQ("move", tempStr2._str.c_str());
        return ctx->set(0);
    };

    getDispatcher().post(func, int(a), str, std::move(str2), std::move(nc), &dbl)->get();

    //Validate values
    EXPECT_EQ(5, a);
    EXPECT_STREQ("changed", str.c_str());
    EXPECT_TRUE(str2.empty());
    EXPECT_EQ(0, (int)nc._str.size());
    EXPECT_DOUBLE_EQ(6.543, dbl);
}

INSTANTIATE_TEST_CASE_P(ParamtersTest_Default,
                        ParamtersTest,
                        ::testing::Values(TestConfiguration(false, false),
                                          TestConfiguration(false, true)));