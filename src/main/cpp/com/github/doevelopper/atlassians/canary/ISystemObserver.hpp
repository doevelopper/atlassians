#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMOBSERVER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMOBSERVER_HPP

#include "SystemMetrics.hpp"

/**
 * @brief Observer interface for system monitoring
 *
 * Implements the Observer design pattern for monitoring system changes.
 * Allows loose coupling between the monitoring system and notification handlers.
 */

namespace com::github::doevelopper::atlassians::canary
{
    /**
     * @brief Observer interface for system metric updates
     *
     * Following the Observer pattern from Gang of Four.
     * Observers are notified when system metrics change or thresholds are exceeded.
     */
    class ISystemObserver
    {
    public:
        virtual ~ISystemObserver() = default;

        /**
         * @brief Called when a new system snapshot is available
         * @param snapshot Current system state
         */
        virtual auto onSystemUpdate(const SystemSnapshot& snapshot) -> void = 0;

        /**
         * @brief Called when CPU usage exceeds threshold
         * @param cpu_usage Current CPU usage percentage
         * @param threshold Configured threshold
         */
        virtual auto onCpuThresholdExceeded(double cpu_usage, double threshold) -> void = 0;

        /**
         * @brief Called when memory usage exceeds threshold
         * @param memory_usage Current memory usage percentage
         * @param threshold Configured threshold
         */
        virtual auto onMemoryThresholdExceeded(double memory_usage, double threshold) -> void = 0;

        /**
         * @brief Called when swap usage exceeds threshold
         * @param swap_usage Current swap usage percentage
         * @param threshold Configured threshold
         */
        virtual auto onSwapThresholdExceeded(double swap_usage, double threshold) -> void = 0;

        /**
         * @brief Called when load average exceeds threshold
         * @param load_avg Current load average
         * @param threshold Configured threshold
         */
        virtual auto onLoadAverageExceeded(double load_avg, double threshold) -> void = 0;

        /**
         * @brief Called when monitoring encounters an error
         * @param error_message Description of the error
         */
        virtual auto onMonitoringError(const std::string& error_message) -> void = 0;
    };

} // namespace com::github::doevelopper::atlassians::canary

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_CANARY_ISYSTEMOBSERVER_HPP
