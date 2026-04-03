#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMINFOPROVIDER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMINFOPROVIDER_HPP

#include <memory>
#include <optional>
#include <string_view>

#include "SystemMetrics.hpp"

/**
 * @brief Interface for system information providers
 *
 * Demonstrates the Strategy pattern - different implementations can provide
 * system information from different sources (proc filesystem, sysinfo(), etc.)
 * Also follows the Interface Segregation Principle (ISP) from SOLID - clients
 * only depend on the methods they need.
 */

namespace com::github::doevelopper::atlassians::canary
{
    /**
     * @brief Base interface for all system information providers
     *
     * Following SOLID principles:
     * - Single Responsibility: Each provider handles one type of system info
     * - Interface Segregation: Specific interfaces for specific needs
     * - Dependency Inversion: Depend on abstractions, not concretions
     */
    class ISystemInfoProvider
    {
    public:
        virtual ~ISystemInfoProvider() = default;

        /**
         * @brief Check if this provider is available on current system
         */
        [[nodiscard]] virtual auto isAvailable() const noexcept -> bool = 0;

        /**
         * @brief Get human-readable name of this provider
         */
        [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

        /**
         * @brief Validate provider can read required system information
         */
        [[nodiscard]] virtual auto validate() const noexcept -> bool = 0;
    };

    /**
     * @brief Interface for CPU information providers
     */
    class ICpuInfoProvider : public ISystemInfoProvider
    {
    public:
        ~ICpuInfoProvider() override = default;

        /**
         * @brief Get CPU information
         * @return CPU information or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getCpuInfo() const noexcept -> std::optional<std::vector<CpuInfo>> = 0;

        /**
         * @brief Get number of CPU cores
         */
        [[nodiscard]] virtual auto getCoreCount() const noexcept -> std::optional<std::uint32_t> = 0;
    };

    /**
     * @brief Interface for memory information providers
     */
    class IMemoryInfoProvider : public ISystemInfoProvider
    {
    public:
        ~IMemoryInfoProvider() override = default;

        /**
         * @brief Get current memory information
         * @return Memory information or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getMemoryInfo() const noexcept -> std::optional<MemoryInfo> = 0;
    };

    /**
     * @brief Interface for load average providers
     */
    class ILoadAverageProvider : public ISystemInfoProvider
    {
    public:
        ~ILoadAverageProvider() override = default;

        /**
         * @brief Get current system load average
         * @return Load average or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getLoadAverage() const noexcept -> std::optional<LoadAverage> = 0;
    };

    /**
     * @brief Interface for CPU statistics providers
     */
    class ICpuStatisticsProvider : public ISystemInfoProvider
    {
    public:
        ~ICpuStatisticsProvider() override = default;

        /**
         * @brief Get CPU statistics for all CPUs
         * @return Vector of CPU statistics or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getCpuStatistics() const noexcept -> std::optional<std::vector<CpuStatistics>> = 0;

        /**
         * @brief Get aggregate CPU statistics
         */
        [[nodiscard]] virtual auto getAggregateCpuStatistics() const noexcept -> std::optional<CpuStatistics> = 0;
    };

    /**
     * @brief Interface for system uptime providers
     */
    class IUptimeProvider : public ISystemInfoProvider
    {
    public:
        ~IUptimeProvider() override = default;

        /**
         * @brief Get system uptime
         * @return System uptime or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getUptime() const noexcept -> std::optional<SystemUptime> = 0;
    };

    /**
     * @brief Interface for network statistics providers
     */
    class INetworkStatsProvider : public ISystemInfoProvider
    {
    public:
        ~INetworkStatsProvider() override = default;

        /**
         * @brief Get network interface statistics
         * @return Vector of network stats or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getNetworkStats() const noexcept -> std::optional<std::vector<NetworkInterfaceStats>> = 0;
    };

    /**
     * @brief Interface for disk statistics providers
     */
    class IDiskStatsProvider : public ISystemInfoProvider
    {
    public:
        ~IDiskStatsProvider() override = default;

        /**
         * @brief Get disk statistics
         * @return Vector of disk stats or std::nullopt if unavailable
         */
        [[nodiscard]] virtual auto getDiskStats() const noexcept -> std::optional<std::vector<DiskStats>> = 0;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMINFOPROVIDER_HPP
