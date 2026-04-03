
#ifndef CFS_OSAL_QUANTUM_FUTUREJOINERTEST_HPP
#define CFS_OSAL_QUANTUM_FUTUREJOINERTEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class FutureJoinerTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        FutureJoinerTest( ) noexcept;
        FutureJoinerTest(const FutureJoinerTest&) = default;
        FutureJoinerTest(FutureJoinerTest&&) = default;
        FutureJoinerTest& operator=(const FutureJoinerTest&) = default;
        FutureJoinerTest& operator=(FutureJoinerTest&&) = default;
        virtual ~FutureJoinerTest() noexcept;


        void assign();

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( FutureJoinerTest )
    };
}
#endif
