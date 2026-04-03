
#ifndef CFS_OSAL_QUANTUM_FOREACHTEST_HPP
#define CFS_OSAL_QUANTUM_FOREACHTEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class ForEachTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        ForEachTest( ) noexcept;
        ForEachTest(const ForEachTest&) = default;
        ForEachTest(ForEachTest&&) = default;
        ForEachTest& operator=(const ForEachTest&) = default;
        ForEachTest& operator=(ForEachTest&&) = default;
        virtual ~ForEachTest() noexcept;

    protected:
        static constexpr int ioLoops = 10000;
        static constexpr int batchNum = 1000;
    private:
        // Q_DISABLE_COPY_MOVE ( ForEachTest )
    };
}
#endif
