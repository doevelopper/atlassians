
#include <pthread.h>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <iterator>

#include <com/github/doevelopper/atlassians/atlas/concurrency/ThreadGranularity.hpp>

using namespace com::github::doevelopper::atlassians::atlas::concurrency;

log4cxx::LoggerPtr ThreadGranularity::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.concurrency.ThreadGranularity"));


ThreadGranularity::ThreadGranularity() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ThreadGranularity::~ThreadGranularity() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ThreadGranularity::affinity(std::thread & t, int n) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if(t.get_id() == std::thread::id())
    {
        // throw std::runtime_error("thread not running");
        LOG4CXX_ERROR(logger, "Current thread an guiven one are the same");
    }
    else
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset); CPU_SET(n, &cpuset);
        auto pth = t.native_handle();
        if ( ::pthread_setaffinity_np(pth, sizeof(cpuset), &cpuset) != 0)
        {
            LOG4CXX_ERROR(logger, "Failed to set thread affinity.");
        }
    }
}

auto ThreadGranularity::concurrency() -> std::uint8_t
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto proc = []() -> int {
        std::ifstream cpuinfo("/proc/cpuinfo");
        return std::count(std::istream_iterator<std::string>(cpuinfo),
                          std::istream_iterator<std::string>(),
                          std::string("processor"));
    };

    auto hwc = std::thread::hardware_concurrency();
    return hwc ? hwc : proc();
}

auto ThreadGranularity::priority(std::thread & th, const int priority) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    sched_param sch;
    sch.sched_priority = priority;

    if(::pthread_setschedparam(th.native_handle(), SCHED_FIFO, &sch))
    {
        LOG4CXX_ERROR(logger, "Failed to set Thread scheduling priority : " << std::strerror(errno));
    }
}
