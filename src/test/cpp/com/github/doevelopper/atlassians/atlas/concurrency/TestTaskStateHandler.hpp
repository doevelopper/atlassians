#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTTASKSTATEHANDLER_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTTASKSTATEHANDLER_HPP

#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/quantum.h>
#include <mutex>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{
    class TestTaskStateHandlerPrivate;
    class TestTaskStateHandler
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        TestTaskStateHandler( ) noexcept;
        // TestConfiguration(const TestConfiguration&) = default;
        // TestConfiguration(TestConfiguration&&) = default;
        // TestConfiguration& operator=(const TestConfiguration&) = default;
        // TestConfiguration& operator=(TestConfiguration&&) = default;
        virtual ~TestTaskStateHandler() noexcept;

        void operator()(size_t taskId, int queueId, Bloomberg::quantum::TaskType type, Bloomberg::quantum::TaskState state);
    protected:

    private:

        // Q_DISABLE_COPY_MOVE ( TestTaskStateHandler )
        // class TestTaskStateHandlerPrivate;
        // std::shared_ptr<TestTaskStateHandlerPrivate> m_impl;

        Q_DECLARE_PRIVATE(TestTaskStateHandler)
        // std::unique_ptr<TestTaskStateHandlerPrivate> d_ptr;
        std::shared_ptr<TestTaskStateHandlerPrivate> d_ptr;
    };
}

#endif
