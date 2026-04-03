/*******************************************************************
 * @file   FSMEvents.hpp
 * @brief  Event definitions for the Application Lifecycle FSM
 *
 * Defines all events that trigger state transitions in the
 * hierarchical state machine. Events are grouped by phase.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMEVENTS_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMEVENTS_HPP

#include <string>
#include <chrono>
#include <memory>

namespace com::github::doevelopper::atlassians::launcher::fsm
{
    // ============================================
    // Base Event Class
    // ============================================

    struct BaseEvent
    {
        std::chrono::system_clock::time_point timestamp;
        std::string source;

        BaseEvent() : timestamp(std::chrono::system_clock::now()), source("") {}
        explicit BaseEvent(std::string_view src)
            : timestamp(std::chrono::system_clock::now()), source(src) {}

        virtual ~BaseEvent() = default;
    };

    // ============================================
    // PHASE 0: Core Infrastructure Events
    // ============================================

    struct InitializationStarted : BaseEvent { };
    struct PreValidationPassed : BaseEvent { };
    struct PreValidationFailed : BaseEvent
    {
        std::string reason;
        int retryCount{0};
        explicit PreValidationFailed(std::string_view r = "", int retry = 0)
            : reason(r), retryCount(retry) {}
    };

    struct ConfigLoaded : BaseEvent { };
    struct ConfigLoadFailed : BaseEvent
    {
        std::string reason;
        explicit ConfigLoadFailed(std::string_view r = "") : reason(r) {}
    };

    struct ConfigActivated : BaseEvent { };
    struct ConfigActivationFailed : BaseEvent
    {
        std::string reason;
        explicit ConfigActivationFailed(std::string_view r = "") : reason(r) {}
    };

    struct LicenseValidated : BaseEvent { };
    struct LicenseCheckFailed : BaseEvent
    {
        std::string reason;
        bool gracePeriodAvailable{false};
        explicit LicenseCheckFailed(std::string_view r = "", bool grace = false)
            : reason(r), gracePeriodAvailable(grace) {}
    };

    struct CoreInitialized : BaseEvent { };
    struct ServiceBootstrapFailed : BaseEvent
    {
        std::string reason;
        explicit ServiceBootstrapFailed(std::string_view r = "") : reason(r) {}
    };

    struct CoreInitializationFailed : BaseEvent
    {
        std::string reason;
        explicit CoreInitializationFailed(std::string_view r = "") : reason(r) {}
    };

    struct CoreRecoveryAttempt : BaseEvent
    {
        int attemptNumber{0};
        explicit CoreRecoveryAttempt(int attempt = 0) : attemptNumber(attempt) {}
    };

    struct CoreRecoverySucceeded : BaseEvent { };
    struct CoreRecoveryExhausted : BaseEvent { };

    // ============================================
    // PHASE 1: Plugin Ecosystem Events
    // ============================================

    struct PluginsDiscovered : BaseEvent
    {
        int pluginCount{0};
        explicit PluginsDiscovered(int count = 0) : pluginCount(count) {}
    };

    struct ManifestParseError : BaseEvent
    {
        std::string pluginName;
        std::string reason;
        ManifestParseError(std::string_view plugin = "", std::string_view r = "")
            : pluginName(plugin), reason(r) {}
    };

    struct PluginsLoaded : BaseEvent
    {
        int successCount{0};
        explicit PluginsLoaded(int count = 0) : successCount(count) {}
    };

    struct PluginLoadFailed : BaseEvent
    {
        std::string pluginName;
        std::string reason;
        bool critical{false};
        PluginLoadFailed(std::string_view plugin = "", std::string_view r = "", bool crit = false)
            : pluginName(plugin), reason(r), critical(crit) {}
    };

    struct PluginLicensesBound : BaseEvent { };
    struct PluginLicenseInvalid : BaseEvent
    {
        std::string pluginName;
        explicit PluginLicenseInvalid(std::string_view plugin = "") : pluginName(plugin) {}
    };

    struct PluginsRegistered : BaseEvent { };
    struct PluginActivationFailed : BaseEvent
    {
        std::string pluginName;
        std::string reason;
        PluginActivationFailed(std::string_view plugin = "", std::string_view r = "")
            : pluginName(plugin), reason(r) {}
    };

    // ============================================
    // PHASE 2: Event Infrastructure Events
    // ============================================

    struct BusProvisioned : BaseEvent { };
    struct EventSystemFailed : BaseEvent
    {
        std::string reason;
        explicit EventSystemFailed(std::string_view r = "") : reason(r) {}
    };

    struct PublishersReady : BaseEvent { };
    struct PublisherRecovery : BaseEvent
    {
        std::string reason;
        explicit PublisherRecovery(std::string_view r = "") : reason(r) {}
    };

    struct SubscribersRegistered : BaseEvent { };
    struct SubscriberRecovery : BaseEvent
    {
        std::string reason;
        explicit SubscriberRecovery(std::string_view r = "") : reason(r) {}
    };

    struct EventSystemReady : BaseEvent { };
    struct EventFlowFailed : BaseEvent
    {
        std::string reason;
        std::chrono::milliseconds latency{0};
        EventFlowFailed(std::string_view r = "", std::chrono::milliseconds lat = std::chrono::milliseconds{0})
            : reason(r), latency(lat) {}
    };

    // ============================================
    // PHASE 3: IPC & Distributed Binding Events
    // ============================================

    struct ChannelsProvisioned : BaseEvent { };
    struct IPCRecovery : BaseEvent
    {
        std::string reason;
        explicit IPCRecovery(std::string_view r = "") : reason(r) {}
    };

    struct ProcessesBound : BaseEvent
    {
        int processCount{0};
        explicit ProcessesBound(int count = 0) : processCount(count) {}
    };

    struct BindingNegotiationFailed : BaseEvent
    {
        std::string processName;
        std::string reason;
        BindingNegotiationFailed(std::string_view proc = "", std::string_view r = "")
            : processName(proc), reason(r) {}
    };

    struct HandshakeCompleted : BaseEvent { };
    struct HandshakeFailed : BaseEvent
    {
        std::string processName;
        std::string reason;
        HandshakeFailed(std::string_view proc = "", std::string_view r = "")
            : processName(proc), reason(r) {}
    };

    struct IPCReady : BaseEvent { };
    struct FlowControlFailed : BaseEvent
    {
        std::string reason;
        explicit FlowControlFailed(std::string_view r = "") : reason(r) {}
    };

    // ============================================
    // PHASE 4: Runtime Readiness Events
    // ============================================

    struct HealthChecksPassed : BaseEvent { };
    struct DegradedMode : BaseEvent
    {
        std::string reason;
        explicit DegradedMode(std::string_view r = "") : reason(r) {}
    };

    struct MetricsBaselineCaptured : BaseEvent { };
    struct MetricsFailed : BaseEvent
    {
        std::string reason;
        explicit MetricsFailed(std::string_view r = "") : reason(r) {}
    };

    struct ReadinessValidated : BaseEvent { };
    struct StartupRollback : BaseEvent
    {
        std::string reason;
        explicit StartupRollback(std::string_view r = "") : reason(r) {}
    };

    // ============================================
    // PHASE 5: Operational Runtime Events
    // ============================================

    struct ApplicationStarting : BaseEvent { };
    struct HealthDegraded : BaseEvent
    {
        std::string component;
        std::string reason;
        HealthDegraded(std::string_view comp = "", std::string_view r = "")
            : component(comp), reason(r) {}
    };

    struct HealthRestored : BaseEvent { };

    struct MaintenanceRequested : BaseEvent
    {
        std::string reason;
        explicit MaintenanceRequested(std::string_view r = "") : reason(r) {}
    };

    struct MaintenanceCompleted : BaseEvent { };

    // ============================================
    // PHASE 6: Shutdown Sequence Events
    // ============================================

    struct ShutdownInitiated : BaseEvent
    {
        std::string reason;
        bool graceful{true};
        ShutdownInitiated(std::string_view r = "", bool grace = true)
            : reason(r), graceful(grace) {}
    };

    struct PreShutdownComplete : BaseEvent { };
    struct ServicesTerminated : BaseEvent { };
    struct ShutdownComplete : BaseEvent { };
    struct CleanupFinished : BaseEvent { };

    struct EmergencyShutdown : BaseEvent
    {
        std::string reason;
        explicit EmergencyShutdown(std::string_view r = "") : reason(r) {}
    };

    // ============================================
    // Generic/Orthogonal Events
    // ============================================

    struct Timeout : BaseEvent
    {
        std::string operation;
        std::chrono::milliseconds duration{0};
        Timeout(std::string_view op = "", std::chrono::milliseconds dur = std::chrono::milliseconds{0})
            : operation(op), duration(dur) {}
    };

    struct RecoveryAttempt : BaseEvent
    {
        std::string phase;
        int attemptNumber{0};
        RecoveryAttempt(std::string_view p = "", int attempt = 0)
            : phase(p), attemptNumber(attempt) {}
    };

    struct CriticalError : BaseEvent
    {
        std::string phase;
        std::string reason;
        int errorCode{0};
        CriticalError(std::string_view p = "", std::string_view r = "", int code = 0)
            : phase(p), reason(r), errorCode(code) {}
    };

    struct ConfigurationReloaded : BaseEvent { };
    struct PluginHotReload : BaseEvent
    {
        std::string pluginName;
        explicit PluginHotReload(std::string_view plugin = "") : pluginName(plugin) {}
    };

} // namespace com::github::doevelopper::atlassians::launcher::fsm

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_FSM_FSMEVENTS_HPP
