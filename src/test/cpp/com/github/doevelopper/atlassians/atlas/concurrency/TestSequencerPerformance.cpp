
#include<com/github/doevelopper/atlassians/atlas/concurrency/TestSequencerPerformance.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/TimeFixture.hpp>

#include <fstream>
#include <iostream>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;
using namespace com::github::doevelopper::atlassians::atlas::concurrency::test;

template <typename Sequencer>
log4cxx::LoggerPtr TestSequencerPerformance<Sequencer>::logger =
    log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.atlassians.atlas.concurrency.test.TestSequencerPerformance" ) );

ProcStats ProcStats::operator-(const ProcStats & s1)
{
    //     ProcStats s;
    //     s._kernelModeTime = s1._kernelModeTime - s2._kernelModeTime;
    //     s._userModeTime   = s1._userModeTime - s2._userModeTime;
    //     return s;
}

template <typename Sequencer>
TestSequencerPerformance<Sequencer>::TestSequencerPerformance() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Sequencer>
TestSequencerPerformance<Sequencer>::~TestSequencerPerformance() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename Sequencer>
void TestSequencerPerformance<Sequencer>::doRun(
    const std::string & name, Bloomberg::quantum::Dispatcher & dispatcher, unsigned sleepTimeMicroseconds,
    unsigned distinctKeyCount, unsigned keyCountPerTask, unsigned taskCount, unsigned universalTaskFreq,
    unsigned yieldCount)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    Sequencer sequencer(dispatcher);
}
/*
template <typename Sequencer>
ProcStats TestSequencerPerformance<Sequencer>::operator-(const ProcStats & s1, const ProcStats & s2)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // ProcStats s3(s1);
    // return s3 -= s2;
    ProcStats s;
    s._kernelModeTime = s1._kernelModeTime - s2._kernelModeTime;
    s._userModeTime   = s1._userModeTime - s2._userModeTime;
    return s;
}
*/
template <typename Sequencer>
ProcStats TestSequencerPerformance<Sequencer>::getProcStats()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#ifdef __linux__
    std::ostringstream buf;
    buf << "/proc/" << getpid() << "/stat";
    std::ifstream file(buf.str());
    if (not file)
    {
        return ProcStats();
    }

    std::string stoken;
    for (int i = 0; i < 13; ++i)
    {
        if (not(file >> stoken))
        {
            return ProcStats();
        }
    }
    // token 14: utime
    // token 15: stime
    ProcStats stats;
    if (not(file >> stats._userModeTime) or not(file >> stats._kernelModeTime))
    {
        return ProcStats();
    }
    return stats;
#else
    return ProcStats();
#endif // __linux__
}
