
#ifndef CFS_OSAL_QUANTUM_SHAREDQUEUETEST_HPP
#define CFS_OSAL_QUANTUM_SHAREDQUEUETEST_HPP

#include <cfs/osal/quantum/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class SharedQueueTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        SharedQueueTest( ) noexcept;
        SharedQueueTest(const SharedQueueTest&) = default;
        SharedQueueTest(SharedQueueTest&&) = default;
        SharedQueueTest& operator=(const SharedQueueTest&) = default;
        SharedQueueTest& operator=(SharedQueueTest&&) = default;
        virtual ~SharedQueueTest() noexcept;

        void SetUp() override;
        void TearDown() override;
    protected:
        void enqueue_sleep_tasks(Bloomberg::quantum::Dispatcher& dispatcher,
                         const std::vector<std::pair<size_t, std::chrono::milliseconds>> & sleepTimes);
    private:
        // Q_DISABLE_COPY_MOVE ( SharedQueueTest )
    };
}
#endif
