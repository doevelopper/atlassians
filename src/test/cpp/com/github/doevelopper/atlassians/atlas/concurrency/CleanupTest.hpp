
#ifndef CFS_OSAL_QUANTUM_CLEANUPTEST_HPP
#define CFS_OSAL_QUANTUM_CLEANUPTEST_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{

    class CleanupTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        CleanupTest( ) noexcept;
        CleanupTest(const CleanupTest&) = default;
        CleanupTest(CleanupTest&&) = default;
        CleanupTest& operator=(const CleanupTest&) = default;
        CleanupTest& operator=(CleanupTest&&) = default;
        virtual ~CleanupTest() noexcept;

    protected:
    private:
        // Q_DISABLE_COPY_MOVE ( CleanupTest )
    };
}
#endif
