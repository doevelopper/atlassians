
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_RESOURCELIMITTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_RESOURCELIMITTEST_HPP

#include <com/github/doevelopper/premisses/atlas/concurrency/Fixture.hpp>


namespace com::github::doevelopper::premisses::atlas::concurrency::test
{

    class ResourceLimitTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        ResourceLimitTest( ) noexcept;
        ResourceLimitTest(const ResourceLimitTest&) = default;
        ResourceLimitTest(ResourceLimitTest&&) = default;
        ResourceLimitTest& operator=(const ResourceLimitTest&) = default;
        ResourceLimitTest& operator=(ResourceLimitTest&&) = default;
        virtual ~ResourceLimitTest() noexcept;

    protected:
        static constexpr int ioLoops = 10000;
        static constexpr int batchNum = 1000;
    private:
        // Q_DISABLE_COPY_MOVE ( ResourceLimitTest )
    };

}
#endif
