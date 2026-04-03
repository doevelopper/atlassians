
#ifndef CFS_OSAL_QUANTUM_PROMISETEST_HPP
#define CFS_OSAL_QUANTUM_PROMISETEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class PromiseTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        PromiseTest( ) noexcept;
        PromiseTest(const PromiseTest&) = default;
        PromiseTest(PromiseTest&&) = default;
        PromiseTest& operator=(const PromiseTest&) = default;
        PromiseTest& operator=(PromiseTest&&) = default;
        virtual ~PromiseTest() noexcept;

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( PromiseTest )
    };
}
#endif
