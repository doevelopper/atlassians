#include <com/github/doevelopper/premisses/atlas/concurrency/Fixture.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/DispatcherSingleton.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr DispatcherFixture::logger = 
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.concurrency.test.DispatcherFixture"));

DispatcherFixture::DispatcherFixture() noexcept
    : m_dispatcher()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    Bloomberg::quantum::StackTraits::defaultSize() = 1 << 14; //16k stack for testing
}

DispatcherFixture::~DispatcherFixture() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void DispatcherFixture::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_dispatcher = &DispatcherSingleton::instance(GetParam());
    // //Don't drain in the TearDown() because of the final CleanupTest::DeleteDispatcherInstance()
    m_dispatcher->drain();
    m_dispatcher->resetStats();
}

void DispatcherFixture::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_dispatcher = nullptr;
}

Bloomberg::quantum::Dispatcher&
DispatcherFixture::getDispatcher()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return * m_dispatcher;
}