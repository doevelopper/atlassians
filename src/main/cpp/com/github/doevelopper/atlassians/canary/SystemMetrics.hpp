#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMETRICS_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMETRICS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

/**
 * @brief Data structures for system performance metrics
 *
 * This file contains all the data transfer objects (DTOs) for system monitoring.
 * Following the Data Transfer Object pattern for clean separation.
 */

namespace com::github::doevelopper::atlassians::canary
{
    /**
     * @brief CPU information from /proc/cpuinfo
     */
    struct CpuInfo
    {
        std::uint32_t processor_id{0};
        std::string vendor_id;
        std::uint32_t cpu_family{0};
        std::uint32_t model{0};
        std::string model_name;
        std::uint32_t stepping{0};
        double cpu_mhz{0.0};
        std::uint64_t cache_size_kb{0};
        std::uint32_t physical_id{0};
        std::uint32_t siblings{0};
        std::uint32_t core_id{0};
        std::uint32_t cpu_cores{0};
        std::vector<std::string> flags;
        std::string bogomips;
    };

    /**
     * @brief Memory information from /proc/meminfo
     */
    struct MemoryInfo
    {
        std::uint64_t total_ram_kb{0};      ///< Total RAM in kilobytes
        std::uint64_t free_ram_kb{0};       ///< Free RAM in kilobytes
        std::uint64_t available_ram_kb{0};  ///< Available RAM (includes reclaimable)
        std::uint64_t buffers_kb{0};        ///< Memory used by buffers
        std::uint64_t cached_kb{0};         ///< Memory used for cache
        std::uint64_t swap_cached_kb{0};    ///< Swap cached
        std::uint64_t swap_total_kb{0};     ///< Total swap space
        std::uint64_t swap_free_kb{0};      ///< Free swap space
        std::uint64_t dirty_kb{0};          ///< Dirty pages
        std::uint64_t writeback_kb{0};      ///< Pages being written back
        std::uint64_t slab_kb{0};           ///< In-kernel data structures

        /**
         * @brief Calculate used RAM percentage
         */
        [[nodiscard]] auto getUsedPercentage() const noexcept -> double
        {
            if (total_ram_kb == 0) return 0.0;
            return 100.0 * (1.0 - static_cast<double>(available_ram_kb) / static_cast<double>(total_ram_kb));
        }

        /**
         * @brief Calculate swap usage percentage
         */
        [[nodiscard]] auto getSwapUsedPercentage() const noexcept -> double
        {
            if (swap_total_kb == 0) return 0.0;
            return 100.0 * (1.0 - static_cast<double>(swap_free_kb) / static_cast<double>(swap_total_kb));
        }
    };

    /**
     * @brief System load average from /proc/loadavg
     */
    struct LoadAverage
    {
        double load_1min{0.0};      ///< 1-minute load average
        double load_5min{0.0};      ///< 5-minute load average
        double load_15min{0.0};     ///< 15-minute load average
        std::uint32_t running_processes{0};    ///< Currently running processes
        std::uint32_t total_processes{0};      ///< Total number of processes
        std::uint32_t last_pid{0};             ///< Last created PID
    };

    /**
     * @brief CPU statistics from /proc/stat
     */
    struct CpuStatistics
    {
        std::uint64_t user{0};       ///< Time in user mode
        std::uint64_t nice{0};       ///< Time in user mode with low priority
        std::uint64_t system{0};     ///< Time in system mode
        std::uint64_t idle{0};       ///< Time in idle task
        std::uint64_t iowait{0};     ///< Time waiting for I/O
        std::uint64_t irq{0};        ///< Time servicing interrupts
        std::uint64_t softirq{0};    ///< Time servicing softirqs
        std::uint64_t steal{0};      ///< Stolen time (virtualization)
        std::uint64_t guest{0};      ///< Time running virtual CPU
        std::uint64_t guest_nice{0}; ///< Time running low-priority virtual CPU

        /**
         * @brief Calculate total CPU time
         */
        [[nodiscard]] auto getTotalTime() const noexcept -> std::uint64_t
        {
            return user + nice + system + idle + iowait + irq + softirq + steal + guest + guest_nice;
        }

        /**
         * @brief Calculate idle time (including iowait)
         */
        [[nodiscard]] auto getIdleTime() const noexcept -> std::uint64_t
        {
            return idle + iowait;
        }

        /**
         * @brief Calculate active time
         */
        [[nodiscard]] auto getActiveTime() const noexcept -> std::uint64_t
        {
            return getTotalTime() - getIdleTime();
        }
    };

    /**
     * @brief System uptime from /proc/uptime
     */
    struct SystemUptime
    {
        double uptime_seconds{0.0};      ///< System uptime in seconds
        double idle_time_seconds{0.0};   ///< Total idle time in seconds

        /**
         * @brief Get uptime as a duration
         */
        [[nodiscard]] auto getUptimeDuration() const noexcept -> std::chrono::seconds
        {
            return std::chrono::seconds(static_cast<std::int64_t>(uptime_seconds));
        }
    };

    /**
     * @brief Network interface statistics from /proc/net/dev
     */
    struct NetworkInterfaceStats
    {
        std::string interface_name;
        std::uint64_t bytes_received{0};
        std::uint64_t packets_received{0};
        std::uint64_t errors_received{0};
        std::uint64_t drops_received{0};
        std::uint64_t bytes_transmitted{0};
        std::uint64_t packets_transmitted{0};
        std::uint64_t errors_transmitted{0};
        std::uint64_t drops_transmitted{0};
    };

    /**
     * @brief Disk statistics from /proc/diskstats
     */
    struct DiskStats
    {
        std::string device_name;
        std::uint64_t reads_completed{0};
        std::uint64_t reads_merged{0};
        std::uint64_t sectors_read{0};
        std::uint64_t time_reading_ms{0};
        std::uint64_t writes_completed{0};
        std::uint64_t writes_merged{0};
        std::uint64_t sectors_written{0};
        std::uint64_t time_writing_ms{0};
        std::uint64_t io_in_progress{0};
        std::uint64_t time_doing_io_ms{0};
    };

    /**
     * @brief Complete system snapshot
     *
     * Aggregates all system metrics in a single structure.
     * Follows the Composite pattern for system monitoring.
     */
    struct SystemSnapshot
    {
        std::chrono::system_clock::time_point timestamp;
        std::vector<CpuInfo> cpu_info;
        MemoryInfo memory_info;
        LoadAverage load_average;
        std::vector<CpuStatistics> cpu_statistics; ///< Per-CPU statistics
        SystemUptime uptime;
        std::vector<NetworkInterfaceStats> network_stats;
        std::vector<DiskStats> disk_stats;

        SystemSnapshot()
            : timestamp(std::chrono::system_clock::now())
        {
        }
    };

    /**
     * @brief Alert threshold configuration
     */
    struct AlertThreshold
    {
        double cpu_usage_percent{90.0};      ///< CPU usage threshold
        double memory_usage_percent{85.0};   ///< Memory usage threshold
        double swap_usage_percent{50.0};     ///< Swap usage threshold
        double load_average_1min{4.0};       ///< 1-min load average threshold
        std::uint64_t disk_io_threshold_mbps{100}; ///< Disk I/O threshold in MB/s
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMETRICS_HPP
