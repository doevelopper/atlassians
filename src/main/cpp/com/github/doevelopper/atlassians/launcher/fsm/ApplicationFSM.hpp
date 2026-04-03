/*******************************************************************
 * @file   ApplicationFSM.hpp
 * @brief  Main Application Lifecycle Finite State Machine
 *
 * Implements a hierarchical state machine using Boost.MSM for managing
 * the complete application lifecycle from initialization through shutdown.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSM_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSM_HPP

#include <memory>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/launcher/fsm/FSMContext.hpp>
#include <com/github/doevelopper/atlassians/launcher/fsm/FSMEvents.hpp>
#include <com/github/doevelopper/atlassians/launcher/fsm/FSMStates.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::fsm
{
    /**
     * @brief Main Application Finite State Machine
     *
     * This class provides a high-level interface to the Boost.MSM-based
     * state machine that manages the complete application lifecycle.
     *
     * Features:
     * - Hierarchical states with proper parent-child relationships
     * - Idempotent actions for safe re-entry during recovery
     * - Timeout guards on all blocking operations
     * - Failure containment with localized recovery states
     * - State persistence for crash recovery
     * - Comprehensive logging and metrics
     *
     * Usage:
     * @code
     * FSMConfiguration config;
     * config.configPath = "/etc/app/config.xml";
     *
     * ApplicationFSM fsm(config);
     * fsm.start();
     *
     * // Process events
     * fsm.processEvent(PreValidationPassed{});
     * fsm.processEvent(ConfigLoaded{});
     * // ...
     * @endcode
     */
    class ApplicationFSM
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    private:
        struct Impl; // PIMPL idiom to hide Boost.MSM details
        std::unique_ptr<Impl> m_impl;

        std::shared_ptr<FSMContext> m_context;

    public:
        /**
         * @brief Constructor
         * @param config Configuration for the FSM
         */
        explicit ApplicationFSM(const FSMConfiguration& config = FSMConfiguration{});

        /**
         * @brief Destructor
         */
        ~ApplicationFSM();

        // Delete copy operations
        ApplicationFSM(const ApplicationFSM&) = delete;
        auto operator=(const ApplicationFSM&) -> ApplicationFSM& = delete;

        // Allow move operations
        ApplicationFSM(ApplicationFSM&&) noexcept;
        auto operator=(ApplicationFSM&&) noexcept -> ApplicationFSM&;

        // ============================================
        // State Machine Control
        // ============================================

        /**
         * @brief Start the state machine
         * Initializes and transitions to the initial state
         */
        auto start() -> void;

        /**
         * @brief Stop the state machine
         * @param graceful If true, initiate graceful shutdown
         */
        auto stop(bool graceful = true) -> void;

        /**
         * @brief Reset the state machine to initial state
         * WARNING: This clears all state and context
         */
        auto reset() -> void;

        // ============================================
        // Event Processing
        // ============================================

        /**
         * @brief Process an event (synchronous)
         * @tparam Event The event type
         * @param event The event instance
         * @return true if event was handled, false otherwise
         */
        template<typename Event>
        auto processEvent(const Event& event) -> bool;

        /**
         * @brief Process InitializationStarted event
         */
        auto processInitializationStarted() -> bool;

        /**
         * @brief Process PreValidationPassed event
         */
        auto processPreValidationPassed() -> bool;

        /**
         * @brief Process PreValidationFailed event
         */
        auto processPreValidationFailed(const std::string& reason, int retryCount = 0) -> bool;

        /**
         * @brief Process ConfigLoaded event
         */
        auto processConfigLoaded() -> bool;

        /**
         * @brief Process ConfigLoadFailed event
         */
        auto processConfigLoadFailed(const std::string& reason) -> bool;

        /**
         * @brief Process ConfigActivated event
         */
        auto processConfigActivated() -> bool;

        /**
         * @brief Process LicenseValidated event
         */
        auto processLicenseValidated() -> bool;

        /**
         * @brief Process CoreInitialized event
         */
        auto processCoreInitialized() -> bool;

        /**
         * @brief Process ShutdownInitiated event
         */
        auto processShutdownInitiated(const std::string& reason = "", bool graceful = true) -> bool;

        // ============================================
        // State Queries
        // ============================================

        /**
         * @brief Get the current state name
         */
        [[nodiscard]] auto getCurrentStateName() const -> std::string;

        /**
         * @brief Check if FSM is in a specific state
         * @tparam State The state type to check
         * @return true if currently in specified state
         */
        template<typename State>
        [[nodiscard]] auto isInState() const -> bool;

        /**
         * @brief Check if FSM is in any error/recovery state
         */
        [[nodiscard]] auto isInErrorState() const -> bool;

        /**
         * @brief Check if FSM is in a terminal state
         */
        [[nodiscard]] auto isInTerminalState() const -> bool;

        /**
         * @brief Check if FSM is fully operational (Active state)
         */
        [[nodiscard]] auto isOperational() const -> bool;

        /**
         * @brief Check if system is in degraded mode
         */
        [[nodiscard]] auto isDegradedMode() const -> bool;

        // ============================================
        // Context and Configuration Access
        // ============================================

        /**
         * @brief Get the FSM context (read-only)
         */
        [[nodiscard]] auto getContext() const noexcept -> const FSMContext&;

        /**
         * @brief Get the FSM context (mutable)
         */
        [[nodiscard]] auto getContext() noexcept -> FSMContext&;

        /**
         * @brief Get the FSM configuration
         */
        [[nodiscard]] auto getConfiguration() const noexcept -> const FSMConfiguration&;

        /**
         * @brief Get the FSM metrics
         */
        [[nodiscard]] auto getMetrics() const noexcept -> const FSMMetrics&;

        // ============================================
        // State Persistence
        // ============================================

        /**
         * @brief Save current state to disk for crash recovery
         * @param path Optional path to save state (uses config path if empty)
         * @return true if save succeeded
         */
        [[nodiscard]] auto saveState(const std::string& path = "") const -> bool;

        /**
         * @brief Restore state from disk
         * @param path Optional path to load state from (uses config path if empty)
         * @return true if restore succeeded
         */
        [[nodiscard]] auto restoreState(const std::string& path = "") -> bool;

        // ============================================
        // Diagnostics and Logging
        // ============================================

        /**
         * @brief Get a human-readable state machine status report
         */
        [[nodiscard]] auto getStatusReport() const -> std::string;

        /**
         * @brief Get transition history (recent N transitions)
         * @param count Number of recent transitions to retrieve
         */
        [[nodiscard]] auto getTransitionHistory(int count = 10) const -> std::vector<std::string>;

        /**
         * @brief Enable/disable verbose transition logging
         */
        auto setVerboseLogging(bool enabled) -> void;

        /**
         * @brief Validate FSM configuration
         * @return Empty string if valid, error message otherwise
         */
        [[nodiscard]] static auto validateConfiguration(const FSMConfiguration& config) -> std::string;
    };

} // namespace com::github::doevelopper::atlassians::launcher::fsm

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_APPLICATIONFSM_HPP
