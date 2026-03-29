#include<com/github/doevelopper/premisses/atlas/concurrency/TestTaskStateHandlerPrivate.hpp>
#include <gmock/gmock.h>
using namespace com::github::doevelopper::premisses::atlas::concurrency:
using namespace com::github::doevelopper::premisses::atlas::concurrency::test;

log4cxx::LoggerPtr TestTaskStateHandlerPrivate::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.test.TestTaskStateHandlerPrivate" ) );

TestTaskStateHandlerPrivate::TestTaskStateHandlerPrivate() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TestTaskStateHandlerPrivate::~TestTaskStateHandlerPrivate() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void TestTaskStateHandlerPrivate::operator()(
    size_t taskId, int queueId, Bloomberg::quantum::TaskType type, Bloomberg::quantum::TaskState state)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    switch (state)
    {
    case Bloomberg::quantum::TaskState::Started:
    {
        size_t *& startedId = Bloomberg::quantum::local::variable<size_t>("StartedId");
        EXPECT_FALSE(startedId);
        startedId = new size_t(Bloomberg::quantum::local::taskId().id());
        break;
    }
    case Bloomberg::quantum::TaskState::Resumed:
    {
        // Check that suspended id exists, equals to the task id and removes it
        size_t *& suspendedId = Bloomberg::quantum::local::variable<size_t>("SuspendedId");
        EXPECT_TRUE(suspendedId);
        EXPECT_EQ(*suspendedId, Bloomberg::quantum::local::taskId().id());
        delete suspendedId;
        suspendedId = nullptr;

        // Create a resumed id
        size_t *& resumedId = Bloomberg::quantum::local::variable<size_t>("ResumedId");
        EXPECT_FALSE(resumedId);
        resumedId = new size_t(Bloomberg::quantum::local::taskId().id());
        break;
    }
    case Bloomberg::quantum::TaskState::Suspended:
    {
        // Call this handler with 'Stopped' task state to handle stored variables
        (*this)(taskId, queueId, type, Bloomberg::quantum::TaskState::Stopped);
        // Create a suspended id
        size_t *& suspendedId = Bloomberg::quantum::local::variable<size_t>("SuspendedId");
        EXPECT_FALSE(suspendedId);
        suspendedId = new size_t(Bloomberg::quantum::local::taskId().id());
        break;
    }
    case Bloomberg::quantum::TaskState::Stopped:
    {
        size_t *& startedId = Bloomberg::quantum::local::variable<size_t>("StartedId");
        size_t *& resumedId = Bloomberg::quantum::local::variable<size_t>("ResumedId");

        // Only one id must exist
        EXPECT_FALSE(startedId && resumedId);
        EXPECT_TRUE(startedId || resumedId);

        // If started id exists check that it equals to the task id and removes it
        if (startedId)
        {
            EXPECT_EQ(*startedId, Bloomberg::quantum::local::taskId().id());
            delete startedId;
            startedId = nullptr;
        }

        // If resumed id exists check that it equals to the task id and removes it
        if (resumedId)
        {
            EXPECT_EQ(*resumedId, Bloomberg::quantum::local::taskId().id());
            delete resumedId;
            resumedId = nullptr;
        }
        break;
    }

    default:
        LOG4CXX_WARN(logger, "Should not be here");
        break;
    }
}