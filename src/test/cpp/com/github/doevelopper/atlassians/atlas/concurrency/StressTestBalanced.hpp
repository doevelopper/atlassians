
#ifndef CFS_OSAL_QUANTUM_STRESSTESTBALANCED_HPP
#define CFS_OSAL_QUANTUM_STRESSTESTBALANCED_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class StressTestBalanced : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        StressTestBalanced( ) noexcept;
        StressTestBalanced(const StressTestBalanced&) = default;
        StressTestBalanced(StressTestBalanced&&) = default;
        StressTestBalanced& operator=(const StressTestBalanced&) = default;
        StressTestBalanced& operator=(StressTestBalanced&&) = default;
        virtual ~StressTestBalanced() noexcept;

    protected:
        static constexpr int ioLoops = 10000;
        static constexpr int batchNum = 1000;
    private:
        // Q_DISABLE_COPY_MOVE ( StressTestBalanced )
    };
}
#endif
