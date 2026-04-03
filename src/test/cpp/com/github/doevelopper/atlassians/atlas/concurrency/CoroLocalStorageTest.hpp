
#ifndef CFS_OSAL_QUANTUM_CoroLocalStorageTest_HPP
#define CFS_OSAL_QUANTUM_CoroLocalStorageTest_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class CoroLocalStorageTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        CoroLocalStorageTest( ) noexcept;
        CoroLocalStorageTest(const CoroLocalStorageTest&) = default;
        CoroLocalStorageTest(CoroLocalStorageTest&&) = default;
        CoroLocalStorageTest& operator=(const CoroLocalStorageTest&) = default;
        CoroLocalStorageTest& operator=(CoroLocalStorageTest&&) = default;
        virtual ~CoroLocalStorageTest() noexcept;

    protected:
        static constexpr int ioLoops = 10000;
        static constexpr int batchNum = 1000;
    private:
        // Q_DISABLE_COPY_MOVE ( CoroLocalStorageTest )
    };
}
#endif
