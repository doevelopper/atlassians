
#include<com/github/doevelopper/premisses/atlas/concurrency/TestTaskStateHandler.hpp>
#include<com/github/doevelopper/premisses/atlas/concurrency/TestTaskStateHandlerPrivate.hpp>

using namespace com::github::doevelopper::premisses::atlas::concurrency;
using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr TestTaskStateHandler::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.test.TestTaskStateHandler" ) );

TestTaskStateHandler::TestTaskStateHandler() noexcept
    // : d_ptr ( new TestTaskStateHandlerPrivate () )
    : d_ptr(std::make_shared<TestTaskStateHandlerPrivate>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TestTaskStateHandler::~TestTaskStateHandler() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void TestTaskStateHandler::operator()(
    size_t taskId, int queueId, Bloomberg::quantum::TaskType type, Bloomberg::quantum::TaskState state)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    d_ptr->operator()(taskId, queueId, type, state);
}
