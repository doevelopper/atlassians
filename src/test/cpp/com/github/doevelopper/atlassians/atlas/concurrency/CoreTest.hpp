
#ifndef CFS_OSAL_QUANTUM_CORETEST_HPP
#define CFS_OSAL_QUANTUM_CORETEST_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class CoreTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        CoreTest( ) noexcept;
        CoreTest(const CoreTest&) = default;
        CoreTest(CoreTest&&) = default;
        CoreTest& operator=(const CoreTest&) = default;
        CoreTest& operator=(CoreTest&&) = default;
        virtual ~CoreTest() noexcept;

        static std::string DummyCoro2(Bloomberg::quantum::VoidContextPtr);
        static int DummyCoro(Bloomberg::quantum::CoroContext<int>::Ptr ctx);
        static int DummyIoTask(Bloomberg::quantum::ThreadPromise<int>::Ptr promise);
        static void runCoro(Bloomberg::quantum::Traits::Coroutine & coro, std::mutex& m, int& end, int start);
    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( CoreTest )
    };
}
#endif
