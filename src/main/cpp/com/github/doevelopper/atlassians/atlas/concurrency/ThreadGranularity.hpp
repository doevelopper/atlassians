
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_THREADGRANULARITY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_THREADGRANULARITY_HPP

#include <thread>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::concurrency
{

    class ThreadGranularity
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
     public:
        ThreadGranularity() noexcept;
        ThreadGranularity(const ThreadGranularity&) noexcept = default;
        ThreadGranularity(ThreadGranularity&&) noexcept =  default;
        ThreadGranularity& operator=(const ThreadGranularity&) noexcept = default;
        ThreadGranularity& operator=(ThreadGranularity&&) noexcept = default;
        virtual ~ThreadGranularity() noexcept;

        auto affinity(std::thread & t, int n) -> void;
        auto concurrency() -> std::uint8_t;
        auto priority(std::thread & th, int priority) -> void;
     protected:
     private:

        unsigned int          m_numberOfThreads { 0 };
        const unsigned int    m_supportedThreads { std::thread::hardware_concurrency() };
    };
}

#endif
