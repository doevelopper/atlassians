#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCUPTIMEPROVIDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCUPTIMEPROVIDER_HPP

#include <string>

#include "ISystemInfoProvider.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Provider for system uptime from /proc/uptime
 */

namespace com::github::doevelopper::atlassians::canary
{
    class ProcUptimeProvider : public IUptimeProvider
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        explicit ProcUptimeProvider() noexcept;
        explicit ProcUptimeProvider(std::string proc_path) noexcept;

        ~ProcUptimeProvider() noexcept override = default;

        ProcUptimeProvider(const ProcUptimeProvider&) = delete;
        auto operator=(const ProcUptimeProvider&) -> ProcUptimeProvider& = delete;
        ProcUptimeProvider(ProcUptimeProvider&&) noexcept = default;
        auto operator=(ProcUptimeProvider&&) noexcept -> ProcUptimeProvider& = default;

        [[nodiscard]] auto isAvailable() const noexcept -> bool override;
        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto validate() const noexcept -> bool override;

        [[nodiscard]] auto getUptime() const noexcept -> std::optional<SystemUptime> override;

    private:
        std::string m_proc_path;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_PROCUPTIMEPROVIDER_HPP
