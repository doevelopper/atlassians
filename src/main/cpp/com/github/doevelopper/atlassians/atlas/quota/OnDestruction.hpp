#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_ONDESTRUCTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_ONDESTRUCTION_HPP

#include <atomic>
#include <type_traits>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::quota
{
    class OnDestruction
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
     public:
        OnDestruction() noexcept;
        OnDestruction(const OnDestruction&) noexcept = default;
        OnDestruction(OnDestruction&&) noexcept =  default;
        OnDestruction& operator=(const OnDestruction&) noexcept = default;
        OnDestruction& operator=(OnDestruction&&) noexcept = default;
        virtual ~OnDestruction() noexcept;

        OnDestruction(std::function<void()> f);
        std::function<void()> callback;
    protected:
    private:
    };
}
#endif
