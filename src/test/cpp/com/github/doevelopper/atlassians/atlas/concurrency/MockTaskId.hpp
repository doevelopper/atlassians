
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_QUANTUM_MOCKTASKID_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_QUANTUM_MOCKTASKID_HPP


#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/TaskId.hpp>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{

    class MockTaskId : public Bloomberg::quantum::TaskId
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        struct ThisThreadTag{};

        MockTaskId( ) noexcept;
        MockTaskId(const MockTaskId&) = default;
        MockTaskId(MockTaskId&&) = default;
        MockTaskId& operator=(const MockTaskId&) = default;
        MockTaskId& operator=(MockTaskId&&) = default;
        virtual ~MockTaskId() noexcept;

        explicit MockTaskId(ThisThreadTag);
        explicit MockTaskId(Bloomberg::quantum::CoroContextTag t);
        explicit MockTaskId(Bloomberg::quantum::ThreadContextTag t);

        void assign();

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( MockTaskId )
    };
}
#endif
