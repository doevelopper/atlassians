/*******************************************************************
 * @file   FSMStates.hpp
 * @brief  State definitions for the Application Lifecycle FSM
 *
 * Defines all states in the hierarchical state machine following
 * the 6-phase lifecycle model with orthogonal failure handling.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMSTATES_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMSTATES_HPP

#include <string>
#include <string_view>

namespace com::github::doevelopper::atlassians::launcher::fsm
{
    // ============================================
    // State Interface and Base Class
    // ============================================

    /**
     * @brief Base interface for all FSM states
     */
    class IFSMState
    {
    public:
        virtual ~IFSMState() = default;

        /**
         * @brief Get the state name
         */
        [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the parent state name (for hierarchical states)
         */
        [[nodiscard]] virtual auto getParentName() const noexcept -> std::string_view = 0;

        /**
         * @brief Check if this is a terminal state
         */
        [[nodiscard]] virtual auto isTerminal() const noexcept -> bool = 0;

        /**
         * @brief Check if this is an error/recovery state
         */
        [[nodiscard]] virtual auto isErrorState() const noexcept -> bool = 0;
    };

    /**
     * @brief Abstract base class for all states with common functionality
     */
    class BaseFSMState : public IFSMState
    {
    protected:
        std::string m_name;
        std::string m_parentName;
        bool m_isTerminal;
        bool m_isErrorState;

    public:
        BaseFSMState(std::string_view name,
                     std::string_view parent = "",
                     bool terminal = false,
                     bool errorState = false) noexcept
            : m_name(name)
            , m_parentName(parent)
            , m_isTerminal(terminal)
            , m_isErrorState(errorState)
        {}

        [[nodiscard]] auto getName() const noexcept -> std::string_view override
        {
            return m_name;
        }

        [[nodiscard]] auto getParentName() const noexcept -> std::string_view override
        {
            return m_parentName;
        }

        [[nodiscard]] auto isTerminal() const noexcept -> bool override
        {
            return m_isTerminal;
        }

        [[nodiscard]] auto isErrorState() const noexcept -> bool override
        {
            return m_isErrorState;
        }
    };

    // ============================================
    // PHASE 0: Core Infrastructure States
    // ============================================

    // Parent state for entire Core Initialization phase
    struct CoreInitializing : BaseFSMState
    {
        CoreInitializing() : BaseFSMState("CoreInitializing") {}
    };

    struct PreValidation : BaseFSMState
    {
        PreValidation() : BaseFSMState("PreValidation", "CoreInitializing") {}
    };

    struct ConfigLoading : BaseFSMState
    {
        ConfigLoading() : BaseFSMState("ConfigLoading", "CoreInitializing") {}
    };

    struct ConfigActivation : BaseFSMState
    {
        ConfigActivation() : BaseFSMState("ConfigActivation", "CoreInitializing") {}
    };

    struct LicenseValidation : BaseFSMState
    {
        LicenseValidation() : BaseFSMState("LicenseValidation", "CoreInitializing") {}
    };

    struct CoreServicesBootstrap : BaseFSMState
    {
        CoreServicesBootstrap() : BaseFSMState("CoreServicesBootstrap", "CoreInitializing") {}
    };

    // Recovery states for Phase 0
    struct CoreRecovering : BaseFSMState
    {
        CoreRecovering() : BaseFSMState("CoreRecovering", "", false, true) {}
    };

    struct ConfigRecovery : BaseFSMState
    {
        ConfigRecovery() : BaseFSMState("ConfigRecovery", "CoreRecovering", false, true) {}
    };

    struct LicenseRecovery : BaseFSMState
    {
        LicenseRecovery() : BaseFSMState("LicenseRecovery", "CoreRecovering", false, true) {}
    };

    struct CoreFailed : BaseFSMState
    {
        CoreFailed() : BaseFSMState("CoreFailed", "", true, true) {}
    };

    // ============================================
    // PHASE 1: Plugin Ecosystem States
    // ============================================

    struct PluginSystemInitializing : BaseFSMState
    {
        PluginSystemInitializing() : BaseFSMState("PluginSystemInitializing") {}
    };

    struct PluginDiscovery : BaseFSMState
    {
        PluginDiscovery() : BaseFSMState("PluginDiscovery", "PluginSystemInitializing") {}
    };

    struct PluginLoading : BaseFSMState
    {
        PluginLoading() : BaseFSMState("PluginLoading", "PluginSystemInitializing") {}
    };

    struct PluginLicenseBinding : BaseFSMState
    {
        PluginLicenseBinding() : BaseFSMState("PluginLicenseBinding", "PluginSystemInitializing") {}
    };

    struct PluginRegistration : BaseFSMState
    {
        PluginRegistration() : BaseFSMState("PluginRegistration", "PluginSystemInitializing") {}
    };

    // Recovery states for Phase 1
    struct PluginRecovery : BaseFSMState
    {
        PluginRecovery() : BaseFSMState("PluginRecovery", "", false, true) {}
    };

    struct PluginDiscoveryFailed : BaseFSMState
    {
        PluginDiscoveryFailed() : BaseFSMState("PluginDiscoveryFailed", "PluginRecovery", false, true) {}
    };

    struct PluginLicenseFailed : BaseFSMState
    {
        PluginLicenseFailed() : BaseFSMState("PluginLicenseFailed", "PluginRecovery", false, true) {}
    };

    struct PluginActivationFailedState : BaseFSMState
    {
        PluginActivationFailedState() : BaseFSMState("PluginActivationFailedState", "PluginRecovery", false, true) {}
    };

    // ============================================
    // PHASE 2: Event Infrastructure States
    // ============================================

    struct EventSystemInitializing : BaseFSMState
    {
        EventSystemInitializing() : BaseFSMState("EventSystemInitializing") {}
    };

    struct BusProvisioning : BaseFSMState
    {
        BusProvisioning() : BaseFSMState("BusProvisioning", "EventSystemInitializing") {}
    };

    struct PublisherSetup : BaseFSMState
    {
        PublisherSetup() : BaseFSMState("PublisherSetup", "EventSystemInitializing") {}
    };

    struct SubscriberRegistration : BaseFSMState
    {
        SubscriberRegistration() : BaseFSMState("SubscriberRegistration", "EventSystemInitializing") {}
    };

    struct EventFlowTest : BaseFSMState
    {
        EventFlowTest() : BaseFSMState("EventFlowTest", "EventSystemInitializing") {}
    };

    // Recovery states for Phase 2
    struct EventSystemRecovering : BaseFSMState
    {
        EventSystemRecovering() : BaseFSMState("EventSystemRecovering", "", false, true) {}
    };

    struct PublisherRecoveryState : BaseFSMState
    {
        PublisherRecoveryState() : BaseFSMState("PublisherRecoveryState", "EventSystemRecovering", false, true) {}
    };

    struct SubscriberRecoveryState : BaseFSMState
    {
        SubscriberRecoveryState() : BaseFSMState("SubscriberRecoveryState", "EventSystemRecovering", false, true) {}
    };

    struct EventSystemFailedState : BaseFSMState
    {
        EventSystemFailedState() : BaseFSMState("EventSystemFailedState", "", true, true) {}
    };

    // ============================================
    // PHASE 3: IPC & Distributed Binding States
    // ============================================

    struct IPCInitializing : BaseFSMState
    {
        IPCInitializing() : BaseFSMState("IPCInitializing") {}
    };

    struct ChannelProvisioning : BaseFSMState
    {
        ChannelProvisioning() : BaseFSMState("ChannelProvisioning", "IPCInitializing") {}
    };

    struct ProcessBinding : BaseFSMState
    {
        ProcessBinding() : BaseFSMState("ProcessBinding", "IPCInitializing") {}
    };

    struct HandshakeVerification : BaseFSMState
    {
        HandshakeVerification() : BaseFSMState("HandshakeVerification", "IPCInitializing") {}
    };

    struct FlowControlSetup : BaseFSMState
    {
        FlowControlSetup() : BaseFSMState("FlowControlSetup", "IPCInitializing") {}
    };

    // Recovery states for Phase 3
    struct IPCRecoveryState : BaseFSMState
    {
        IPCRecoveryState() : BaseFSMState("IPCRecoveryState", "", false, true) {}
    };

    struct BindingNegotiationFailedState : BaseFSMState
    {
        BindingNegotiationFailedState() : BaseFSMState("BindingNegotiationFailedState", "IPCRecoveryState", false, true) {}
    };

    struct HandshakeFailedState : BaseFSMState
    {
        HandshakeFailedState() : BaseFSMState("HandshakeFailedState", "IPCRecoveryState", false, true) {}
    };

    struct FlowControlFailedState : BaseFSMState
    {
        FlowControlFailedState() : BaseFSMState("FlowControlFailedState", "IPCRecoveryState", false, true) {}
    };

    // ============================================
    // PHASE 4: Runtime Readiness States
    // ============================================

    struct ReadinessChecking : BaseFSMState
    {
        ReadinessChecking() : BaseFSMState("ReadinessChecking") {}
    };

    struct HealthProbes : BaseFSMState
    {
        HealthProbes() : BaseFSMState("HealthProbes", "ReadinessChecking") {}
    };

    struct MetricsBaseline : BaseFSMState
    {
        MetricsBaseline() : BaseFSMState("MetricsBaseline", "ReadinessChecking") {}
    };

    struct FinalValidation : BaseFSMState
    {
        FinalValidation() : BaseFSMState("FinalValidation", "ReadinessChecking") {}
    };

    // Recovery state for Phase 4
    struct StartupRollbackState : BaseFSMState
    {
        StartupRollbackState() : BaseFSMState("StartupRollbackState", "", false, true) {}
    };

    // ============================================
    // PHASE 5: Operational Runtime States
    // ============================================

    struct Running : BaseFSMState
    {
        Running() : BaseFSMState("Running") {}
    };

    struct Active : BaseFSMState
    {
        Active() : BaseFSMState("Active", "Running") {}
    };

    struct DegradedModeState : BaseFSMState
    {
        DegradedModeState() : BaseFSMState("DegradedModeState", "Running", false, false) {}
    };

    struct Maintenance : BaseFSMState
    {
        Maintenance() : BaseFSMState("Maintenance", "Running") {}
    };

    // ============================================
    // PHASE 6: Shutdown Sequence States
    // ============================================

    struct ShuttingDown : BaseFSMState
    {
        ShuttingDown() : BaseFSMState("ShuttingDown") {}
    };

    struct PreShutdown : BaseFSMState
    {
        PreShutdown() : BaseFSMState("PreShutdown", "ShuttingDown") {}
    };

    struct ServiceTeardown : BaseFSMState
    {
        ServiceTeardown() : BaseFSMState("ServiceTeardown", "ShuttingDown") {}
    };

    struct FinalCleanup : BaseFSMState
    {
        FinalCleanup() : BaseFSMState("FinalCleanup", "ShuttingDown") {}
    };

    struct ShutdownCompleteState : BaseFSMState
    {
        ShutdownCompleteState() : BaseFSMState("ShutdownCompleteState", "", true, false) {}
    };

    struct EmergencyShutdownState : BaseFSMState
    {
        EmergencyShutdownState() : BaseFSMState("EmergencyShutdownState", "", true, true) {}
    };

} // namespace com::github::doevelopper::atlassians::launcher::fsm

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMSTATES_HPP
