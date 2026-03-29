
#ifndef CFS_OSAL_QUANTUM_MUTEXTEST_HPP
#define CFS_OSAL_QUANTUM_MUTEXTEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class MutexTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        MutexTest( ) noexcept;
        MutexTest(const MutexTest&) = default;
        MutexTest(MutexTest&&) = default;
        MutexTest& operator=(const MutexTest&) = default;
        MutexTest& operator=(MutexTest&&) = default;
        virtual ~MutexTest() noexcept;

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( MutexTest )
    };
}
#endif
