#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFO_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFO_HPP

#include <memory>
#include <vector>

#include "SystemMetrics.hpp"
#include "ISystemInfoProvider.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Facade for system information access
 *
 * Demonstrates the Facade design pattern - provides a simplified interface
 * to a complex subsystem of providers.
 * Also uses Dependency Injection for testability.
 */

namespace com::github::doevelopper::atlassians::canary
{
    /**
     * @brief Main facade for accessing system information
     *
     * This class provides a unified interface to various system information providers.
     * Following SOLID principles:
     * - Single Responsibility: Coordinates access to system info providers
     * - Open/Closed: Can extend by adding new providers without modifying this class
     * - Dependency Inversion: Depends on abstractions (interfaces) not concretions
     */
    class SystemInfo
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Construct with default providers
         */
        SystemInfo();

        /**
         * @brief Construct with custom providers (for testing/flexibility)
         * @param cpu_provider Custom CPU info provider
         * @param memory_provider Custom memory info provider
         * @param load_provider Custom load average provider
         * @param uptime_provider Custom uptime provider
         */
        SystemInfo(
            std::unique_ptr<ICpuInfoProvider> cpu_provider,
            std::unique_ptr<IMemoryInfoProvider> memory_provider,
            std::unique_ptr<ILoadAverageProvider> load_provider,
            std::unique_ptr<IUptimeProvider> uptime_provider
        );

        ~SystemInfo();

        // No copying, allow moving
        SystemInfo(const SystemInfo&) = delete;
        auto operator=(const SystemInfo&) -> SystemInfo& = delete;
        SystemInfo(SystemInfo&&) noexcept;
        auto operator=(SystemInfo&&) noexcept -> SystemInfo&;

        /**
         * @brief Get complete system snapshot
         * @return Current system state
         */
        [[nodiscard]] auto getSystemSnapshot() const noexcept -> std::optional<SystemSnapshot>;

        /**
         * @brief Get CPU information
         */
        [[nodiscard]] auto getCpuInfo() const noexcept -> std::optional<std::vector<CpuInfo>>;

        /**
         * @brief Get memory information
         */
        [[nodiscard]] auto getMemoryInfo() const noexcept -> std::optional<MemoryInfo>;

        /**
         * @brief Get load average
         */
        [[nodiscard]] auto getLoadAverage() const noexcept -> std::optional<LoadAverage>;

        /**
         * @brief Get system uptime
         */
        [[nodiscard]] auto getUptime() const noexcept -> std::optional<SystemUptime>;

        /**
         * @brief Check if all providers are available
         */
        [[nodiscard]] auto areProvidersAvailable() const noexcept -> bool;

        /**
         * @brief Validate all providers
         */
        [[nodiscard]] auto validateProviders() const noexcept -> bool;

        /**
         * @brief Get list of unavailable providers
         */
        [[nodiscard]] auto getUnavailableProviders() const noexcept -> std::vector<std::string>;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMINFO_HPP
