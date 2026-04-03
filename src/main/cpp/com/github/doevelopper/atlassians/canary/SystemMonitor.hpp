#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITOR_HPP

#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

#include "SystemInfo.hpp"
#include "SystemMetrics.hpp"
#include "ISystemObserver.hpp"
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

/**
 * @brief Continuous system monitoring with observer notification
 *
 * Demonstrates:
 * - Observer design pattern
 * - Template Method pattern (monitoring loop)
 * - RAII for thread management
 */

namespace com::github::doevelopper::atlassians::canary
{
    /**
     * @brief System monitor with observer pattern support
     *
     * Continuously monitors system metrics and notifies observers when
     * thresholds are exceeded or significant changes occur.
     */
    class SystemMonitor
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Construct monitor with default settings
         * @param system_info System information facade
         */
        explicit SystemMonitor(std::shared_ptr<SystemInfo> system_info);

        /**
         * @brief Construct monitor with custom thresholds
         * @param system_info System information facade
         * @param thresholds Alert thresholds
         */
        SystemMonitor(std::shared_ptr<SystemInfo> system_info, AlertThreshold thresholds);

        ~SystemMonitor();

        // No copying, allow moving
        SystemMonitor(const SystemMonitor&) = delete;
        auto operator=(const SystemMonitor&) -> SystemMonitor& = delete;
        SystemMonitor(SystemMonitor&&) noexcept;
        auto operator=(SystemMonitor&&) noexcept -> SystemMonitor&;

        /**
         * @brief Register an observer for system events
         * @param observer Observer to register
         */
        auto addObserver(std::shared_ptr<ISystemObserver> observer) -> void;

        /**
         * @brief Unregister an observer
         * @param observer Observer to remove
         */
        auto removeObserver(std::shared_ptr<ISystemObserver> observer) -> void;

        /**
         * @brief Remove all observers
         */
        auto clearObservers() -> void;

        /**
         * @brief Start monitoring (non-blocking)
         * @param interval Monitoring interval
         */
        auto start(std::chrono::milliseconds interval = std::chrono::seconds(5)) -> void;

        /**
         * @brief Stop monitoring
         */
        auto stop() -> void;

        /**
         * @brief Check if monitoring is active
         */
        [[nodiscard]] auto isRunning() const noexcept -> bool;

        /**
         * @brief Perform a single monitoring check (synchronous)
         */
        auto checkOnce() -> void;

        /**
         * @brief Set alert thresholds
         */
        auto setThresholds(const AlertThreshold& thresholds) -> void;

        /**
         * @brief Get current thresholds
         */
        [[nodiscard]] auto getThresholds() const noexcept -> AlertThreshold;

        /**
         * @brief Get number of registered observers
         */
        [[nodiscard]] auto getObserverCount() const noexcept -> std::size_t;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;

        /**
         * @brief Main monitoring loop (runs in background thread)
         */
        auto monitoringLoop() -> void;

        /**
         * @brief Check thresholds and notify observers
         */
        auto checkThresholdsAndNotify(const SystemSnapshot& snapshot) -> void;

        /**
         * @brief Notify all observers of system update
         */
        auto notifySystemUpdate(const SystemSnapshot& snapshot) -> void;

        /**
         * @brief Calculate CPU usage from statistics
         */
        [[nodiscard]] auto calculateCpuUsage(const CpuStatistics& stats) const noexcept -> double;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_SYSTEMMONITOR_HPP
