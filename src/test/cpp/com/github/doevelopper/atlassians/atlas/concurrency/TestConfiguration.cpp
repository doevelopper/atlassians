
#include <com/github/doevelopper/atlassians/atlas/concurrency/TestConfiguration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TestTaskStateHandler.hpp>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

log4cxx::LoggerPtr TestConfiguration::logger = 
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.TestConfiguration" ) );

TestConfiguration::TestConfiguration ( ) noexcept
    : m_loadBalance(false)
    , m_coroutineSharingForAny(false)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TestConfiguration::~TestConfiguration ( ) noexcept
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
}

TestConfiguration::TestConfiguration(bool loadBalance, bool coroutineSharingForAny ,const Bloomberg::quantum::TaskStateConfiguration & taskStateConfiguration)
    : m_loadBalance(loadBalance)
    , m_coroutineSharingForAny(coroutineSharingForAny)
    , m_taskStateConfiguration(taskStateConfiguration)
{
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    Bloomberg::quantum::TaskStateConfiguration config;
    config.setTaskStateHandler(TestTaskStateHandler());
    config.setHandledTaskStates(Bloomberg::quantum::TaskState::All);
    config.setHandledTaskTypes(Bloomberg::quantum::TaskType::Coroutine);
    this->m_taskStateConfiguration = config;
}

bool TestConfiguration::loadBalance () const
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ );
    return (m_loadBalance);
}

void TestConfiguration::loadBalance (bool newLoadBalance)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    m_loadBalance = newLoadBalance;
}

bool TestConfiguration::coroutineSharingForAny () const
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    return (m_coroutineSharingForAny);
}

void TestConfiguration::coroutineSharingForAny (bool newCoroutineSharingForAny)
{ 
    LOG4CXX_TRACE ( logger, __LOG4CXX_FUNC__ ); 
    m_coroutineSharingForAny = newCoroutineSharingForAny;
}