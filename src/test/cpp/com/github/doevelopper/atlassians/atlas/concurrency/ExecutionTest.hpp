
#ifndef CFS_OSAL_QUANTUM_EXECUTIONTEST_HPP
#define CFS_OSAL_QUANTUM_EXECUTIONTEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class ExecutionTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        ExecutionTest( ) noexcept;
        ExecutionTest(const ExecutionTest&) = default;
        ExecutionTest(ExecutionTest&&) = default;
        ExecutionTest& operator=(const ExecutionTest&) = default;
        ExecutionTest& operator=(ExecutionTest&&) = default;
        virtual ~ExecutionTest() noexcept;

        static int DummyCoro(Bloomberg::quantum::CoroContext<int>::Ptr ctx);
    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( ExecutionTest )
    };
}
#endif
