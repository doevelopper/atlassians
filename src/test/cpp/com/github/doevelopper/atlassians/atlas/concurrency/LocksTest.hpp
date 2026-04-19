
#ifndef CFS_OSAL_QUANTUM_LOCKSTEST_HPP
#define CFS_OSAL_QUANTUM_LOCKSTEST_HPP

#include <gmock/gmock.h>
#include <com/github/doevelopper/atlassians/atlas/concurrency/quantum.h>
#include <cfs/utils/log/LoggingService.hpp>

namespace cfs::osal::quantum::test
{
    class LocksTest
        : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
    public:

        LocksTest();
        LocksTest(const LocksTest &) = default;
        LocksTest(LocksTest &&) = default;
        LocksTest &operator=(const LocksTest &) = default;
        LocksTest &operator=(LocksTest &&) = default;
        virtual ~LocksTest();

        void SetUp() override;
        void TearDown() override;

        static void runnable(Bloomberg::quantum::SpinLock* exclusiveLock);
        static void runThreads(int num);
        static void spinlockSettings (
                size_t min,
                size_t max,
                std::chrono::microseconds sleepUs,
                size_t numYields,
                int num,
                int enable);
    protected:
        static constexpr int spins = 100000;
        static int val ;
        static constexpr const int numThreads = 50;
        static constexpr int numLockAcquires = 100;        
    private:
    };
}
#endif
