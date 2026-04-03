
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_QUANTUM_DISPATCHERCORORANGETEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_QUANTUM_DISPATCHERCORORANGETEST_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Configuration.hpp>

namespace com::github::doevelopper::atlassians::atlas::concurrency::test
{

    class DispatcherCoroRangeTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        DispatcherCoroRangeTest( ) noexcept;
        DispatcherCoroRangeTest(const DispatcherCoroRangeTest&) = default;
        DispatcherCoroRangeTest(DispatcherCoroRangeTest&&) = default;
        DispatcherCoroRangeTest& operator=(const DispatcherCoroRangeTest&) = default;
        DispatcherCoroRangeTest& operator=(DispatcherCoroRangeTest&&) = default;
        virtual ~DispatcherCoroRangeTest() noexcept;

        void SetUp() override;
        void TearDown() override;

    protected:
        Bloomberg::quantum::Configuration configuration;
    private:

        // Q_DISABLE_COPY_MOVE ( DispatcherCoroRangeTest )

    };

}
#endif
