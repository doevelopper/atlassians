
#include <com/github/doevelopper/atlassians/atlas/barriers/SpinLockTest.hpp>

using namespace  com::github::doevelopper::night::owl::barriers;
using namespace  com::github::doevelopper::night::owl::barriers::test;

log4cxx::LoggerPtr SpinLockTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.barriers.test.SpinLockTest"));

SpinLockTest::SpinLockTest()
    : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

SpinLockTest::~SpinLockTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void SpinLockTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    this->m_targetUnderTest = new SpinLock();
}

void SpinLockTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    if(this->m_targetUnderTest) {
        delete this->m_targetUnderTest;
        this->m_targetUnderTest = nullptr;
    }
}

TEST_F(SpinLockTest, test_basics)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // EXPECT_EQ(this->m_targetUnderTest->try_lock(),false);
}
TEST_F(SpinLockTest, lock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    SpinLock lock;
    // lock.flag.clear();
    // lock.lock();
    // EXPECT_TRUE(lock.flag.test_and_set(std::memory_order_acquire));
}

TEST_F(SpinLockTest, try_lock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    SpinLock lock;
    // EXPECT_TRUE(lock.try_lock());
    // EXPECT_FALSE(lock.try_lock());
}

TEST_F(SpinLockTest, unlock)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    SpinLock lock;
    // lock.flag.test_and_set(std::memory_order_acquire);
    // lock.unlock();
    // EXPECT_FALSE(lock.flag.test_and_set(std::memory_order_acquire));
}