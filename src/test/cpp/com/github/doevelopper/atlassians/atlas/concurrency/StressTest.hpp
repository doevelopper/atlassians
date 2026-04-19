
#ifndef CFS_OSAL_QUANTUM_STRESSTEST_HPP
#define CFS_OSAL_QUANTUM_STRESSTEST_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class StressTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        StressTest( ) noexcept;
        StressTest(const StressTest&) = default;
        StressTest(StressTest&&) = default;
        StressTest& operator=(const StressTest&) = default;
        StressTest& operator=(StressTest&&) = default;
        virtual ~StressTest() noexcept;

        static int recursiveFibonacci(Bloomberg::quantum::CoroContext<size_t>::Ptr ctx, size_t fib);
        static int sequentialFibonacci(Bloomberg::quantum::CoroContext<size_t>::Ptr ctx, size_t fib);
    protected:

        static constexpr int ioLoops = 10000;
        static constexpr int batchNum = 1000;
        static constexpr int fibInput = 23;
        std::map<int, int> fibValues{
                {10, 55}, {20, 6765}, {21, 10946}, {22, 17711},
                {23, 28657}, {24, 46368}, {25, 75025}, {30, 832040}
        };
    private:
        // Q_DISABLE_COPY_MOVE ( StressTest )
    };
}
#endif
