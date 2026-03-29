
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTSEQUENCERPERFORMANCE_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_TESTSEQUENCERPERFORMANCE_HPP

#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/Dispatcher.hpp>
#include <condition_variable>
#include <mutex>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{
    struct ProcStats
    {
        double _kernelModeTime = 0.0; // time spent in kernel mode
        double _userModeTime   = 0.0; // time spent in user mode
        ProcStats operator-(const ProcStats & s1);
    };

    template <typename Sequencer>
    class TestSequencerPerformance
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        TestSequencerPerformance( ) noexcept;
        // TestSequencerPerformance(const TestConfiguration&) = default;
        // TestSequencerPerformance(TestConfiguration&&) = default;
        // TestSequencerPerformance& operator=(const TestConfiguration&) = default;
        // TestSequencerPerformance& operator=(TestConfiguration&&) = default;
        virtual ~TestSequencerPerformance() noexcept;

        void doRun(const std::string & name, Bloomberg::quantum::Dispatcher & dispatcher, unsigned int sleepTimeMicroseconds
            , unsigned int distinctKeyCount, unsigned int keyCountPerTask, unsigned int taskCount
            , unsigned int universalTaskFreq, unsigned int yieldCount);
        ProcStats getProcStats();
    protected:
    private:
    };
    // ProcStats operator-(const ProcStats & s1, const ProcStats & s2)
    // {
    //     // ProcStats s3(s1);
    //     // return s3 -= s2;
    //     ProcStats s;
    //     s._kernelModeTime = s1._kernelModeTime - s2._kernelModeTime;
    //     s._userModeTime   = s1._userModeTime - s2._userModeTime;
    //     return s;
    // }
}
#endif
