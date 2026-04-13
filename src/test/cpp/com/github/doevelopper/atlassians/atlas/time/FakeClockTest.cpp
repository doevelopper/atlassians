
#include <com/github/doevelopper/atlassians/atlas/time/FakeClockTest.hpp>

using namespace  com::github::doevelopper::night::owl::time;
using namespace  com::github::doevelopper::night::owl::time::test;
using namespace com::github::doevelopper::night::owl::extentions::test;

log4cxx::LoggerPtr FakeClockTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.test.FakeClockTest"));

FakeClockTest::FakeClockTest()
    : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

FakeClockTest::~FakeClockTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void FakeClockTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    this->m_targetUnderTest = new FakeClock();
}

void FakeClockTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    if(this->m_targetUnderTest) {
        delete this->m_targetUnderTest;
        this->m_targetUnderTest = nullptr;
    }
}

TEST_F(FakeClockTest, test_basics)
{

    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // EXPECT_EQ(this->m_targetUnderTest->try_lock(),false);
}

TEST_F(FakeClockTest, ClockTest) {
    // FakeClock::time_point t0 = FakeClock::now();
    // FakeClock::time_point t1 = FakeClock::now();
    // EXPECT_EQ(std::chrono::microseconds(0), t1 - t0);
    //
    // FakeClock::advance(std::chrono::microseconds(10));
    // FakeClock::time_point t2 = FakeClock::now();
    // EXPECT_EQ(std::chrono::microseconds(10), t2 - t0);
    //
    // FakeClock::reset_to_epoch();
    // FakeClock::time_point t3 = FakeClock::now();
    // EXPECT_EQ(std::chrono::microseconds(0), t3 - t0);
}

TEST_F(FakeClockTest, ExampleTest) {
    // Ensures that the example in README.md works.
    // FakeClock::time_point t1 = FakeClock::now();
    // FakeClock::advance(std::chrono::milliseconds(100));
    // FakeClock::time_point t2 = FakeClock::now();
    // int64_t elapsed_us =
    //     std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    // EXPECT_EQ(100000, elapsed_us);
}

TEST_F(FakeClockTest, PrecisionTest) {
    // Ensures that there are enough bits in the internal
    // representation of time to store all nanoseconds in a century.
    EXPECT_TRUE(sizeof(FakeClock::Rep) * CHAR_BIT >= 62);
}