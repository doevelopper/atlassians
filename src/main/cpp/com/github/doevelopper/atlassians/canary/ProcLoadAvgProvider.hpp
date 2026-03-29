#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCLOADAVGPROVIDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCLOADAVGPROVIDER_HPP

#include <string>

#include "ISystemInfoProvider.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Provider for system load average from /proc/loadavg
 */

namespace com::github::doevelopper::atlassians::canary
{
    class ProcLoadAvgProvider : public ILoadAverageProvider
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        explicit ProcLoadAvgProvider() noexcept;
        explicit ProcLoadAvgProvider(std::string proc_path) noexcept;

        ~ProcLoadAvgProvider() noexcept override = default;

        ProcLoadAvgProvider(const ProcLoadAvgProvider&) = delete;
        auto operator=(const ProcLoadAvgProvider&) -> ProcLoadAvgProvider& = delete;
        ProcLoadAvgProvider(ProcLoadAvgProvider&&) noexcept = default;
        auto operator=(ProcLoadAvgProvider&&) noexcept -> ProcLoadAvgProvider& = default;

        [[nodiscard]] auto isAvailable() const noexcept -> bool override;
        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto validate() const noexcept -> bool override;

        [[nodiscard]] auto getLoadAverage() const noexcept -> std::optional<LoadAverage> override;

    private:
        std::string m_proc_path;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCLOADAVGPROVIDER_HPP
