
#ifndef CFS_OSAL_QUANTUM_MAPREDUCETEST_HPP
#define CFS_OSAL_QUANTUM_MAPREDUCETEST_HPP

#include <com/github/doevelopper/atlassians/atlas/concurrency/Fixture.hpp>

namespace cfs::osal::quantum::test
{

    class MapReduceTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        MapReduceTest( ) noexcept;
        MapReduceTest(const MapReduceTest&) = default;
        MapReduceTest(MapReduceTest&&) = default;
        MapReduceTest& operator=(const MapReduceTest&) = default;
        MapReduceTest& operator=(MapReduceTest&&) = default;
        virtual ~MapReduceTest() noexcept;

    protected:

    private:
        // Q_DISABLE_COPY_MOVE ( MapReduceTest )
    };
}
#endif
