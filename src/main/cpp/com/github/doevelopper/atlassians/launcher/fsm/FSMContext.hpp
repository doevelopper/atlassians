/*******************************************************************
 * @file   FSMContext.hpp
 * @brief  Context/data holder for the Application Lifecycle FSM
 *
 * Provides shared state and configuration data accessible to all
 * states and actions in the state machine.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMCONTEXT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMCONTEXT_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <atomic>
#include <mutex>
#include <any>

namespace com::github::doevelopper::atlassians::launcher::fsm
{
    /**
     * @brief Configuration data for the FSM
     */
    struct FSMConfiguration
    {
        // Retry limits
        int maxCoreRecoveryAttempts{3};
        int maxPluginRecoveryAttempts{2};
        int maxIPCRecoveryAttempts{2};

        // Timeout values (milliseconds)
        std::chrono::milliseconds configLoadTimeout{5000};
        std::chrono::milliseconds licenseCheckTimeout{3000};
        std::chrono::milliseconds pluginLoadTimeout{10000};
        std::chrono::milliseconds eventFlowTestTimeout{5000};
        std::chrono::milliseconds ipcHandshakeTimeout{8000};
        std::chrono::milliseconds shutdownTimeout{30000};

        // Feature flags
        bool enableGracePeriodMode{true};
        bool enableDegradedMode{true};
        bool enablePluginHotReload{false};
        bool enableStatePersistence{true};

        // Paths
        std::string configPath;
        std::string pluginDirectory;
        std::string licensePath;
        std::string stateSnapshotPath;
    };

    /**
     * @brief Statistics and metrics for FSM operations
     */
    struct FSMMetrics
    {
        std::chrono::system_clock::time_point startupStartTime;
        std::chrono::system_clock::time_point lastTransitionTime;

        std::atomic<int> totalTransitions{0};
        std::atomic<int> recoveryAttempts{0};
        std::atomic<int> failedPlugins{0};
        std::atomic<int> activePlugins{0};

        std::map<std::string, std::chrono::milliseconds> phaseDurations;
        std::map<std::string, int> errorCounts;

        mutable std::mutex mutex;

        auto recordTransition(const std::string& from, const std::string& to) -> void
        {
            std::lock_guard<std::mutex> lock(mutex);
            totalTransitions++;
            lastTransitionTime = std::chrono::system_clock::now();
        }

        auto recordError(const std::string& phase, const std::string& error) -> void
        {
            std::lock_guard<std::mutex> lock(mutex);
            errorCounts[phase + "::" + error]++;
        }

        auto recordPhaseDuration(const std::string& phase, std::chrono::milliseconds duration) -> void
        {
            std::lock_guard<std::mutex> lock(mutex);
            phaseDurations[phase] = duration;
        }
    };

    /**
     * @brief Phase-specific state data
     */
    struct PhaseData
    {
        std::chrono::system_clock::time_point phaseStartTime;
        std::chrono::system_clock::time_point phaseEndTime;

        int retryCount{0};
        bool completed{false};
        bool failed{false};

        std::vector<std::string> errors;
        std::vector<std::string> warnings;

        // Phase-specific custom data
        std::map<std::string, std::any> customData;

        auto reset() -> void
        {
            retryCount = 0;
            completed = false;
            failed = false;
            errors.clear();
            warnings.clear();
            customData.clear();
        }
    };

    /**
     * @brief Main FSM Context holding all state machine data
     */
    class FSMContext
    {
    private:
        FSMConfiguration m_config;
        FSMMetrics m_metrics;

        // Phase-specific data
        std::map<std::string, PhaseData> m_phaseData;

        // Current state information
        std::string m_currentStateName;
        std::string m_previousStateName;

        // Global flags
        std::atomic<bool> m_shutdownRequested{false};
        std::atomic<bool> m_emergencyMode{false};
        std::atomic<bool> m_degradedMode{false};

        // Thread safety
        mutable std::mutex m_mutex;

    public:
        /**
         * @brief Constructor
         */
        explicit FSMContext(const FSMConfiguration& config = FSMConfiguration{})
            : m_config(config)
        {
            m_metrics.startupStartTime = std::chrono::system_clock::now();
            initializePhaseData();
        }

        /**
         * @brief Destructor
         */
        ~FSMContext() = default;

        // Delete copy operations (context should be unique)
        FSMContext(const FSMContext&) = delete;
        auto operator=(const FSMContext&) -> FSMContext& = delete;

        // Allow move operations
        FSMContext(FSMContext&&) noexcept = default;
        auto operator=(FSMContext&&) noexcept -> FSMContext& = default;

        // ============================================
        // Configuration Access
        // ============================================

        [[nodiscard]] auto getConfig() const noexcept -> const FSMConfiguration&
        {
            return m_config;
        }

        [[nodiscard]] auto getConfig() noexcept -> FSMConfiguration&
        {
            return m_config;
        }

        // ============================================
        // Metrics Access
        // ============================================

        [[nodiscard]] auto getMetrics() const noexcept -> const FSMMetrics&
        {
            return m_metrics;
        }

        [[nodiscard]] auto getMetrics() noexcept -> FSMMetrics&
        {
            return m_metrics;
        }

        // ============================================
        // Phase Data Access
        // ============================================

        [[nodiscard]] auto getPhaseData(const std::string& phaseName) -> PhaseData&
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_phaseData[phaseName];
        }

        [[nodiscard]] auto getPhaseData(const std::string& phaseName) const -> const PhaseData&
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            static const PhaseData empty;
            auto it = m_phaseData.find(phaseName);
            return (it != m_phaseData.end()) ? it->second : empty;
        }

        auto markPhaseCompleted(const std::string& phaseName) -> void
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto& phase = m_phaseData[phaseName];
            phase.completed = true;
            phase.phaseEndTime = std::chrono::system_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                phase.phaseEndTime - phase.phaseStartTime);
            m_metrics.recordPhaseDuration(phaseName, duration);
        }

        auto markPhaseFailed(const std::string& phaseName, const std::string& error) -> void
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            auto& phase = m_phaseData[phaseName];
            phase.failed = true;
            phase.errors.push_back(error);
            phase.phaseEndTime = std::chrono::system_clock::now();

            m_metrics.recordError(phaseName, error);
        }

        auto incrementPhaseRetry(const std::string& phaseName) -> int
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return ++m_phaseData[phaseName].retryCount;
        }

        // ============================================
        // State Tracking
        // ============================================

        auto setCurrentState(const std::string& stateName) -> void
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_previousStateName = m_currentStateName;
            m_currentStateName = stateName;
            m_metrics.recordTransition(m_previousStateName, m_currentStateName);
        }

        [[nodiscard]] auto getCurrentStateName() const noexcept -> std::string
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_currentStateName;
        }

        [[nodiscard]] auto getPreviousStateName() const noexcept -> std::string
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_previousStateName;
        }

        // ============================================
        // Global Flags
        // ============================================

        [[nodiscard]] auto isShutdownRequested() const noexcept -> bool
        {
            return m_shutdownRequested.load();
        }

        auto requestShutdown() noexcept -> void
        {
            m_shutdownRequested.store(true);
        }

        [[nodiscard]] auto isEmergencyMode() const noexcept -> bool
        {
            return m_emergencyMode.load();
        }

        auto setEmergencyMode(bool enabled) noexcept -> void
        {
            m_emergencyMode.store(enabled);
        }

        [[nodiscard]] auto isDegradedMode() const noexcept -> bool
        {
            return m_degradedMode.load();
        }

        auto setDegradedMode(bool enabled) noexcept -> void
        {
            m_degradedMode.store(enabled);
        }

        // ============================================
        // Utility Functions
        // ============================================

        [[nodiscard]] auto getUptime() const noexcept -> std::chrono::milliseconds
        {
            auto now = std::chrono::system_clock::now();
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                now - m_metrics.startupStartTime);
        }

        auto reset() -> void
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_currentStateName.clear();
            m_previousStateName.clear();
            m_shutdownRequested.store(false);
            m_emergencyMode.store(false);
            m_degradedMode.store(false);

            for (auto& [name, phase] : m_phaseData)
            {
                phase.reset();
            }
        }

    private:
        auto initializePhaseData() -> void
        {
            // Initialize all phase data structures
            const std::vector<std::string> phases = {
                "CoreInitializing",
                "PluginSystemInitializing",
                "EventSystemInitializing",
                "IPCInitializing",
                "ReadinessChecking",
                "Running",
                "ShuttingDown"
            };

            for (const auto& phase : phases)
            {
                m_phaseData[phase] = PhaseData{};
                m_phaseData[phase].phaseStartTime = std::chrono::system_clock::now();
            }
        }
    };

} // namespace com::github::doevelopper::atlassians::launcher::fsm

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMCONTEXT_HPP
