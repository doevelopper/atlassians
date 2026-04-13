
#include <com/github/doevelopper/atlassians/atlas/time/TimeFixtureTest.hpp>

using namespace  com::github::doevelopper::night::owl::time;
using namespace  com::github::doevelopper::night::owl::time::test;

log4cxx::LoggerPtr TimeFixtureTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.time.test.TimeFixtureTest"));

TimeFixtureTest::TimeFixtureTest()
    // : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

TimeFixtureTest::~TimeFixtureTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void TimeFixtureTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SpinLock();
}

void TimeFixtureTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}

TEST_F(TimeFixtureTest, test_basics)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // EXPECT_EQ(this->m_targetUnderTest->try_lock(),false);
}
