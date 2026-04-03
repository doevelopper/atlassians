#include "SystemMonitor.hpp"

#include <algorithm>
#include <mutex>

namespace com::github::doevelopper::atlassians::canary
{
    log4cxx::LoggerPtr SystemMonitor::logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.canary.SystemMonitor");

    struct SystemMonitor::Impl
    {
        std::shared_ptr<SystemInfo> system_info;
        std::vector<std::shared_ptr<ISystemObserver>> observers;
        mutable std::mutex observers_mutex;

        AlertThreshold thresholds;

        std::atomic<bool> running{false};
        std::thread monitoring_thread;
        std::chrono::milliseconds interval{std::chrono::seconds(5)};

        // Previous CPU statistics for usage calculation
        std::optional<CpuStatistics> previous_cpu_stats;
        mutable std::mutex stats_mutex;

        explicit Impl(std::shared_ptr<SystemInfo> info)
            : system_info(std::move(info))
        {
        }

        Impl(std::shared_ptr<SystemInfo> info, AlertThreshold thresh)
            : system_info(std::move(info))
            , thresholds(thresh)
        {
        }
    };

    SystemMonitor::SystemMonitor(std::shared_ptr<SystemInfo> system_info)
        : pImpl(std::make_unique<Impl>(std::move(system_info)))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
        LOG4CXX_INFO(logger, "SystemMonitor created with default thresholds");
    }

    SystemMonitor::SystemMonitor(std::shared_ptr<SystemInfo> system_info, AlertThreshold thresholds)
        : pImpl(std::make_unique<Impl>(std::move(system_info), thresholds))
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
        LOG4CXX_INFO(logger, "SystemMonitor created with custom thresholds");
    }

    SystemMonitor::~SystemMonitor()
    {
        LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
        stop();
    }

    SystemMonitor::SystemMonitor(SystemMonitor&&) noexcept = default;

    auto SystemMonitor::operator=(SystemMonitor&&) noexcept -> SystemMonitor& = default;

    auto SystemMonitor::addObserver(std::shared_ptr<ISystemObserver> observer) -> void
    {
        if (!observer)
        {
            LOG4CXX_WARN(logger, "Attempted to add null observer");
            return;
        }

        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);
        pImpl->observers.push_back(std::move(observer));
        LOG4CXX_INFO(logger, "Observer added. Total observers: " << pImpl->observers.size());
    }

    auto SystemMonitor::removeObserver(std::shared_ptr<ISystemObserver> observer) -> void
    {
        if (!observer)
        {
            return;
        }

        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);
        auto it = std::remove(pImpl->observers.begin(), pImpl->observers.end(), observer);
        pImpl->observers.erase(it, pImpl->observers.end());
        LOG4CXX_INFO(logger, "Observer removed. Total observers: " << pImpl->observers.size());
    }

    auto SystemMonitor::clearObservers() -> void
    {
        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);
        pImpl->observers.clear();
        LOG4CXX_INFO(logger, "All observers cleared");
    }

    auto SystemMonitor::start(std::chrono::milliseconds interval) -> void
    {
        if (pImpl->running.load())
        {
            LOG4CXX_WARN(logger, "Monitor already running");
            return;
        }

        pImpl->interval = interval;
        pImpl->running.store(true);

        pImpl->monitoring_thread = std::thread([this]() {
            monitoringLoop();
        });

        LOG4CXX_INFO(logger, "Monitoring started with interval: " << interval.count() << "ms");
    }

    auto SystemMonitor::stop() -> void
    {
        if (!pImpl->running.load())
        {
            return;
        }

        LOG4CXX_INFO(logger, "Stopping monitor...");
        pImpl->running.store(false);

        if (pImpl->monitoring_thread.joinable())
        {
            pImpl->monitoring_thread.join();
        }

        LOG4CXX_INFO(logger, "Monitor stopped");
    }

    auto SystemMonitor::isRunning() const noexcept -> bool
    {
        return pImpl && pImpl->running.load();
    }

    auto SystemMonitor::checkOnce() -> void
    {
        if (!pImpl || !pImpl->system_info)
        {
            LOG4CXX_ERROR(logger, "Cannot check: system info not available");
            return;
        }

        auto snapshot = pImpl->system_info->getSystemSnapshot();
        if (!snapshot.has_value())
        {
            LOG4CXX_ERROR(logger, "Failed to get system snapshot");
            return;
        }

        notifySystemUpdate(snapshot.value());
        checkThresholdsAndNotify(snapshot.value());
    }

    auto SystemMonitor::setThresholds(const AlertThreshold& thresholds) -> void
    {
        if (pImpl)
        {
            pImpl->thresholds = thresholds;
            LOG4CXX_INFO(logger, "Thresholds updated");
        }
    }

    auto SystemMonitor::getThresholds() const noexcept -> AlertThreshold
    {
        return pImpl ? pImpl->thresholds : AlertThreshold{};
    }

    auto SystemMonitor::getObserverCount() const noexcept -> std::size_t
    {
        if (!pImpl)
        {
            return 0;
        }
        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);
        return pImpl->observers.size();
    }

    auto SystemMonitor::monitoringLoop() -> void
    {
        LOG4CXX_DEBUG(logger, "Monitoring loop started");

        while (pImpl->running.load())
        {
            try
            {
                checkOnce();
            }
            catch (const std::exception& ex)
            {
                LOG4CXX_ERROR(logger, "Error in monitoring loop: " << ex.what());

                std::lock_guard<std::mutex> lock(pImpl->observers_mutex);
                for (auto& observer : pImpl->observers)
                {
                    if (observer)
                    {
                        observer->onMonitoringError(ex.what());
                    }
                }
            }

            // Sleep for the specified interval
            std::this_thread::sleep_for(pImpl->interval);
        }

        LOG4CXX_DEBUG(logger, "Monitoring loop ended");
    }

    auto SystemMonitor::checkThresholdsAndNotify(const SystemSnapshot& snapshot) -> void
    {
        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);

        if (pImpl->observers.empty())
        {
            return;
        }

        // Check memory usage
        double memory_usage = snapshot.memory_info.getUsedPercentage();
        if (memory_usage > pImpl->thresholds.memory_usage_percent)
        {
            LOG4CXX_WARN(logger, "Memory usage threshold exceeded: " << memory_usage << "%");
            for (auto& observer : pImpl->observers)
            {
                if (observer)
                {
                    observer->onMemoryThresholdExceeded(memory_usage, pImpl->thresholds.memory_usage_percent);
                }
            }
        }

        // Check swap usage
        double swap_usage = snapshot.memory_info.getSwapUsedPercentage();
        if (swap_usage > pImpl->thresholds.swap_usage_percent)
        {
            LOG4CXX_WARN(logger, "Swap usage threshold exceeded: " << swap_usage << "%");
            for (auto& observer : pImpl->observers)
            {
                if (observer)
                {
                    observer->onSwapThresholdExceeded(swap_usage, pImpl->thresholds.swap_usage_percent);
                }
            }
        }

        // Check load average
        if (snapshot.load_average.load_1min > pImpl->thresholds.load_average_1min)
        {
            LOG4CXX_WARN(logger, "Load average threshold exceeded: " << snapshot.load_average.load_1min);
            for (auto& observer : pImpl->observers)
            {
                if (observer)
                {
                    observer->onLoadAverageExceeded(snapshot.load_average.load_1min,
                                                   pImpl->thresholds.load_average_1min);
                }
            }
        }

        // CPU usage check would require tracking previous values
        // This is a simplified version
        if (!snapshot.cpu_statistics.empty())
        {
            std::lock_guard<std::mutex> stats_lock(pImpl->stats_mutex);
            const auto& current_stats = snapshot.cpu_statistics[0];

            if (pImpl->previous_cpu_stats.has_value())
            {
                // Calculate CPU usage percentage (simplified)
                const auto& prev_stats = pImpl->previous_cpu_stats.value();
                auto total_delta = current_stats.getTotalTime() - prev_stats.getTotalTime();
                auto idle_delta = current_stats.getIdleTime() - prev_stats.getIdleTime();

                if (total_delta > 0)
                {
                    double cpu_usage = 100.0 * (1.0 - static_cast<double>(idle_delta) / static_cast<double>(total_delta));

                    if (cpu_usage > pImpl->thresholds.cpu_usage_percent)
                    {
                        LOG4CXX_WARN(logger, "CPU usage threshold exceeded: " << cpu_usage << "%");
                        for (auto& observer : pImpl->observers)
                        {
                            if (observer)
                            {
                                observer->onCpuThresholdExceeded(cpu_usage, pImpl->thresholds.cpu_usage_percent);
                            }
                        }
                    }
                }
            }

            pImpl->previous_cpu_stats = current_stats;
        }
    }

    auto SystemMonitor::notifySystemUpdate(const SystemSnapshot& snapshot) -> void
    {
        std::lock_guard<std::mutex> lock(pImpl->observers_mutex);

        for (auto& observer : pImpl->observers)
        {
            if (observer)
            {
                observer->onSystemUpdate(snapshot);
            }
        }
    }

    auto SystemMonitor::calculateCpuUsage(const CpuStatistics& stats) const noexcept -> double
    {
        auto total_time = stats.getTotalTime();
        if (total_time == 0)
        {
            return 0.0;
        }

        auto active_time = stats.getActiveTime();
        return 100.0 * static_cast<double>(active_time) / static_cast<double>(total_time);
    }

} // namespace com::github::doevelopper::atlassians::canary
