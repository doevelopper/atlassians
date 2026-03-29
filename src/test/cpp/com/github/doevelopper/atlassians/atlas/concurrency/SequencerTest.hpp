
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_SEQUENCERTEST_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_SEQUENCERTEST_HPP

#include <com/github/doevelopper/premisses/atlas/concurrency/SequencerTestData.hpp>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{
    class SequencerTest : public DispatcherFixture
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER
    public:

        SequencerTest();
        SequencerTest(const SequencerTest &) = default;
        SequencerTest(SequencerTest &&) = default;
        SequencerTest &operator=(const SequencerTest &) = default;
        SequencerTest &operator=(SequencerTest &&) = default;
        virtual ~SequencerTest();

    protected:

    private:
    };
}
#endif
