#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTTASKSTATEHANDLERPRIVATE_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTTASKSTATEHANDLERPRIVATE_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/quantum.h>
#include <mutex>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{
    class TestTaskStateHandlerPrivate
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        TestTaskStateHandlerPrivate( ) noexcept;
        // TestConfiguration(const TestConfiguration&) = default;
        // TestConfiguration(TestConfiguration&&) = default;
        // TestConfiguration& operator=(const TestConfiguration&) = default;
        // TestConfiguration& operator=(TestConfiguration&&) = default;
        virtual ~TestTaskStateHandlerPrivate() noexcept;

        void operator()(size_t taskId, int queueId, Bloomberg::quantum::TaskType type, Bloomberg::quantum::TaskState state);
    protected:

    private:

        // Q_DISABLE_COPY_MOVE ( TestTaskStateHandlerPrivate )
    };
}

#endif

