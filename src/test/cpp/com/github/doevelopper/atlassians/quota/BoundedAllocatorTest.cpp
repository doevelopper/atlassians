#include <com/github/doevelopper/atlassians/quota/BoundedAllocatorTest.hpp>

using namespace  com::github::doevelopper::night::owl::quota;
using namespace  com::github::doevelopper::night::owl::quota::test;

log4cxx::LoggerPtr BoundedAllocatorTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.quota.test.BoundedAllocatorTest"));

BoundedAllocatorTest::BoundedAllocatorTest()
    // : m_targetUnderTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

BoundedAllocatorTest::~BoundedAllocatorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

void BoundedAllocatorTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // this->m_targetUnderTest = new SpinMutex();
}

void BoundedAllocatorTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    // if(this->m_targetUnderTest) {
    //     delete this->m_targetUnderTest;
    //     this->m_targetUnderTest = nullptr;
    // }
}
