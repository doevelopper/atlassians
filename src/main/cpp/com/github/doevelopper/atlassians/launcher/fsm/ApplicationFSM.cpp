/*******************************************************************
 * @file   ApplicationFSM.cpp
 * @brief  Implementation of the Application Lifecycle FSM
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/fsm/ApplicationFSM.hpp>

#include <sstream>
#include <iomanip>
#include <fstream>
#include <deque>

// Boost.MSM includes
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/front/functor_row.hpp>

using namespace com::github::doevelopper::atlassians::launcher::fsm;

namespace msm = boost::msm;
namespace mpl = boost::mpl;
using namespace msm::front;

// ============================================
// Logger initialization
// ============================================

log4cxx::LoggerPtr ApplicationFSM::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.fsm.ApplicationFSM"));

// Separate logger for internal state machine implementation
namespace
{
    log4cxx::LoggerPtr implLogger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.fsm.ApplicationFSM.Impl"));

    auto& getAppFSMLogger()
    {
        return implLogger;
    }
}

// ============================================
// Boost.MSM State Machine Definition
// ============================================

namespace
{
    /**
     * @brief Internal state machine definition using Boost.MSM
     */
    struct ApplicationFSM_Impl : public msm::front::state_machine_def<ApplicationFSM_Impl>
    {
        // Shared context
        std::shared_ptr<FSMContext> context;

        // Reference to parent Impl for accessing diagnostic data
        struct ImplAccess* implAccess{nullptr};

        explicit ApplicationFSM_Impl(std::shared_ptr<FSMContext> ctx, ImplAccess* access = nullptr)
            : context(std::move(ctx))
            , implAccess(access)
        {}

        // ============================================
        // Helper functions for entry/exit actions
        // ============================================

        auto logEntry(const std::string& stateName) -> void
        {
            LOG4CXX_INFO(getAppFSMLogger(), "Entering state: " << stateName);
            if (context)
            {
                context->setCurrentState(stateName);
                auto& phase = context->getPhaseData(stateName);
                phase.phaseStartTime = std::chrono::system_clock::now();
            }
        }

        auto logExit(const std::string& stateName) -> void
        {
            LOG4CXX_INFO(getAppFSMLogger(), "Exiting state: " << stateName);
        }

        auto logTransition(const std::string& from, const std::string& to, const std::string& event) -> void;

        auto getVerboseLogging() const -> bool;

        auto getTransitionHistory() const -> const std::deque<std::string>*;

        // ============================================
        // Entry/Exit Actions for States
        // ============================================

        template <class Event, class FSM>
        auto on_entry(Event const&, FSM&) -> void
        {
            LOG4CXX_INFO(getAppFSMLogger(), "FSM Started");
        }

        template <class Event, class FSM>
        auto on_exit(Event const&, FSM&) -> void
        {
            LOG4CXX_INFO(getAppFSMLogger(), "FSM Stopped");
        }

        // ============================================
        // State Definitions with Entry/Exit Actions
        // ============================================

        struct PreValidationState : public msm::front::state<>
        {
            std::shared_ptr<FSMContext> ctx;

            explicit PreValidationState(std::shared_ptr<FSMContext> c = nullptr) : ctx(c) {}

            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("PreValidation");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Validating filesystem permissions");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Checking minimum system resources");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("PreValidation");
            }
        };

        struct ConfigLoadingState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("ConfigLoading");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Loading configuration from multiple sources");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("ConfigLoading");
            }
        };

        struct ConfigActivationState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("ConfigActivation");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Activating configuration");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("ConfigActivation");
            }
        };

        struct LicenseValidationState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("LicenseValidation");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Validating license");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("LicenseValidation");
            }
        };

        struct CoreServicesBootstrapState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("CoreServicesBootstrap");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Bootstrapping core services");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("CoreServicesBootstrap");
                if (fsm.context)
                {
                    fsm.context->markPhaseCompleted("CoreInitializing");
                }
            }
        };

        struct PluginDiscoveryState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("PluginDiscovery");
                LOG4CXX_DEBUG(getAppFSMLogger(), "Discovering plugins");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("PluginDiscovery");
            }
        };

        struct ActiveState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("Active");
                LOG4CXX_INFO(getAppFSMLogger(), "Application is now fully operational");
                if (fsm.context)
                {
                    fsm.context->setDegradedMode(false);
                }
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("Active");
            }
        };

        struct DegradedModeStateImpl : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const& evt, FSM& fsm)
            {
                fsm.logEntry("DegradedMode");
                LOG4CXX_WARN(getAppFSMLogger(), "Entering degraded mode");
                if (fsm.context)
                {
                    fsm.context->setDegradedMode(true);
                }
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("DegradedMode");
            }
        };

        struct CoreRecoveringState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("CoreRecovering");
                LOG4CXX_WARN(getAppFSMLogger(), "Attempting core recovery");
                if (fsm.context)
                {
                    int retries = fsm.context->incrementPhaseRetry("CoreInitializing");
                    LOG4CXX_INFO(getAppFSMLogger(), "Core recovery attempt: " << retries);
                }
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("CoreRecovering");
            }
        };

        struct CoreFailedState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("CoreFailed");
                LOG4CXX_ERROR(getAppFSMLogger(), "Core initialization failed - terminal state");
                if (fsm.context)
                {
                    fsm.context->markPhaseFailed("CoreInitializing", "Recovery exhausted");
                    fsm.context->setEmergencyMode(true);
                }
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("CoreFailed");
            }
        };

        struct PreShutdownState : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("PreShutdown");
                LOG4CXX_INFO(getAppFSMLogger(), "Pre-shutdown: rejecting new requests");
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("PreShutdown");
            }
        };

        struct ShutdownCompleteStateImpl : public msm::front::state<>
        {
            template <class Event, class FSM>
            void on_entry(Event const&, FSM& fsm)
            {
                fsm.logEntry("ShutdownComplete");
                LOG4CXX_INFO(getAppFSMLogger(), "Shutdown complete - terminal state");
                if (fsm.context)
                {
                    fsm.context->markPhaseCompleted("ShuttingDown");
                }
            }

            template <class Event, class FSM>
            void on_exit(Event const&, FSM& fsm)
            {
                fsm.logExit("ShutdownComplete");
            }
        };

        // ============================================
        // Initial State Declaration (after all states are defined)
        // ============================================

        using initial_state = PreValidationState;

        // ============================================
        // Transition Table
        // ============================================

        struct transition_table : mpl::vector<
            //    Start                      Event                    Next                       Action    Guard
            //  +---------------------------+------------------------+---------------------------+---------+------+
            Row < PreValidationState,        PreValidationPassed,     ConfigLoadingState                         >,
            Row < PreValidationState,        PreValidationFailed,     CoreRecoveringState                        >,
            Row < ConfigLoadingState,        ConfigLoaded,            ConfigActivationState                      >,
            Row < ConfigLoadingState,        ConfigLoadFailed,        CoreRecoveringState                        >,
            Row < ConfigActivationState,     ConfigActivated,         LicenseValidationState                     >,
            Row < ConfigActivationState,     ConfigActivationFailed,  CoreFailedState                            >,
            Row < LicenseValidationState,    LicenseValidated,        CoreServicesBootstrapState                 >,
            Row < LicenseValidationState,    LicenseCheckFailed,      CoreRecoveringState                        >,
            Row < CoreServicesBootstrapState,CoreInitialized,         PluginDiscoveryState                       >,
            Row < CoreServicesBootstrapState,ServiceBootstrapFailed,  CoreRecoveringState                        >,
            Row < CoreRecoveringState,       CoreRecoverySucceeded,   PreValidationState                         >,
            Row < CoreRecoveringState,       CoreRecoveryExhausted,   CoreFailedState                            >,
            Row < PluginDiscoveryState,      PluginsDiscovered,       ActiveState                                >,
            Row < ActiveState,               HealthDegraded,          DegradedModeStateImpl                      >,
            Row < DegradedModeStateImpl,     HealthRestored,          ActiveState                                >,
            Row < ActiveState,               ShutdownInitiated,       PreShutdownState                           >,
            Row < DegradedModeStateImpl,     ShutdownInitiated,       PreShutdownState                           >,
            Row < PreShutdownState,          PreShutdownComplete,     ShutdownCompleteStateImpl                  >
        > {};

        // ============================================
        // Guard Conditions
        // ============================================

        // Can be extended with guard functions like:
        // bool canRecover(const Event& evt) { return retryCount < maxRetries; }

        // ============================================
        // Exception Handling
        // ============================================

        template <class FSM,class Event>
        void no_transition(Event const& e, FSM&, int state)
        {
            LOG4CXX_WARN(getAppFSMLogger(),
                "No transition found for event in state: " << state);
        }
    };

    // ImplAccess struct for diagnostic data - defined BEFORE methods that use it
    struct ImplAccess
    {
        std::deque<std::string> transitionHistory;
        bool verboseLogging{false};
    };

    // Implement deferred methods that need ImplAccess (NOW it's a complete type)
    inline auto ApplicationFSM_Impl::logTransition(const std::string& from, const std::string& to, const std::string& event) -> void
    {
        if (getVerboseLogging() || LOG4CXX_UNLIKELY(getAppFSMLogger()->isDebugEnabled()))
        {
            LOG4CXX_DEBUG(getAppFSMLogger(),
                "Transition: " << from << " -> " << to << " [" << event << "]");
        }

        if (auto* history = getTransitionHistory())
        {
            std::ostringstream oss;
            oss << from << " -> " << to << " [" << event << "]";
            const_cast<std::deque<std::string>*>(history)->push_back(oss.str());

            if (history->size() > 100)
            {
                const_cast<std::deque<std::string>*>(history)->pop_front();
            }
        }
    }

    inline auto ApplicationFSM_Impl::getVerboseLogging() const -> bool
    {
        if (implAccess)
            return implAccess->verboseLogging;
        return false;
    }

    inline auto ApplicationFSM_Impl::getTransitionHistory() const -> const std::deque<std::string>*
    {
        if (implAccess)
            return &implAccess->transitionHistory;
        return nullptr;
    }

    // Create the actual state machine type
    using ApplicationFSM_SM = msm::back::state_machine<ApplicationFSM_Impl>;

} // anonymous namespace

// ============================================
// PIMPL Struct
// ============================================

struct ApplicationFSM::Impl
{
    ImplAccess diagnostics;  // Must be declared before stateMachine for proper initialization order
    std::unique_ptr<ApplicationFSM_SM> stateMachine;

    explicit Impl(std::shared_ptr<FSMContext> context)
    {
        // Create state machine with context and diagnostics access
        stateMachine = std::make_unique<ApplicationFSM_SM>(context, &diagnostics);
    }
};

// ============================================
// ApplicationFSM Implementation
// ============================================

ApplicationFSM::ApplicationFSM(const FSMConfiguration& config)
    : m_context(std::make_shared<FSMContext>(config))
    , m_impl(std::make_unique<Impl>(m_context))
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

ApplicationFSM::~ApplicationFSM()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

ApplicationFSM::ApplicationFSM(ApplicationFSM&&) noexcept = default;
auto ApplicationFSM::operator=(ApplicationFSM&&) noexcept -> ApplicationFSM& = default;

auto ApplicationFSM::start() -> void
{
    LOG4CXX_INFO(logger, "Starting Application FSM");
    m_impl->stateMachine->start();
    LOG4CXX_INFO(logger, "FSM started and entered initial state: " << getCurrentStateName());
}

auto ApplicationFSM::stop(bool graceful) -> void
{
    LOG4CXX_INFO(logger, "Stopping Application FSM (graceful=" << std::boolalpha << graceful << ")");
    m_context->requestShutdown();

    if (graceful)
    {
        processShutdownInitiated("User requested stop", true);
    }
    else
    {
        m_impl->stateMachine->process_event(EmergencyShutdown{"Forced stop"});
    }

    m_impl->stateMachine->stop();
}

auto ApplicationFSM::reset() -> void
{
    LOG4CXX_INFO(logger, "Resetting Application FSM");
    m_context->reset();
    m_impl->stateMachine->stop();
    m_impl->stateMachine->start();
}

// ============================================
// Event Processing Methods
// ============================================

auto ApplicationFSM::processInitializationStarted() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: InitializationStarted");
    m_impl->stateMachine->process_event(InitializationStarted{});
    return true;
}

auto ApplicationFSM::processPreValidationPassed() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: PreValidationPassed");
    m_impl->stateMachine->process_event(PreValidationPassed{});
    return true;
}

auto ApplicationFSM::processPreValidationFailed(const std::string& reason, int retryCount) -> bool
{
    LOG4CXX_WARN(logger, "Processing: PreValidationFailed - " << reason);
    m_impl->stateMachine->process_event(PreValidationFailed{reason, retryCount});
    return true;
}

auto ApplicationFSM::processConfigLoaded() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: ConfigLoaded");
    m_impl->stateMachine->process_event(ConfigLoaded{});
    return true;
}

auto ApplicationFSM::processConfigLoadFailed(const std::string& reason) -> bool
{
    LOG4CXX_WARN(logger, "Processing: ConfigLoadFailed - " << reason);
    m_impl->stateMachine->process_event(ConfigLoadFailed{reason});
    return true;
}

auto ApplicationFSM::processConfigActivated() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: ConfigActivated");
    m_impl->stateMachine->process_event(ConfigActivated{});
    return true;
}

auto ApplicationFSM::processLicenseValidated() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: LicenseValidated");
    m_impl->stateMachine->process_event(LicenseValidated{});
    return true;
}

auto ApplicationFSM::processCoreInitialized() -> bool
{
    LOG4CXX_DEBUG(logger, "Processing: CoreInitialized");
    m_impl->stateMachine->process_event(CoreInitialized{});
    return true;
}

auto ApplicationFSM::processShutdownInitiated(const std::string& reason, bool graceful) -> bool
{
    LOG4CXX_INFO(logger, "Processing: ShutdownInitiated - " << reason);
    m_impl->stateMachine->process_event(ShutdownInitiated{reason, graceful});
    return true;
}

// ============================================
// State Query Methods
// ============================================

auto ApplicationFSM::getCurrentStateName() const -> std::string
{
    return m_context->getCurrentStateName();
}

auto ApplicationFSM::isInErrorState() const -> bool
{
    const auto& stateName = getCurrentStateName();
    return stateName.find("Failed") != std::string::npos ||
           stateName.find("Recovery") != std::string::npos ||
           stateName.find("Degraded") != std::string::npos;
}

auto ApplicationFSM::isInTerminalState() const -> bool
{
    const auto& stateName = getCurrentStateName();
    return stateName == "CoreFailed" ||
           stateName == "ShutdownComplete" ||
           stateName == "EmergencyShutdown";
}

auto ApplicationFSM::isOperational() const -> bool
{
    return getCurrentStateName() == "Active";
}

auto ApplicationFSM::isDegradedMode() const -> bool
{
    return m_context->isDegradedMode();
}

// ============================================
// Context Access Methods
// ============================================

auto ApplicationFSM::getContext() const noexcept -> const FSMContext&
{
    return *m_context;
}

auto ApplicationFSM::getContext() noexcept -> FSMContext&
{
    return *m_context;
}

auto ApplicationFSM::getConfiguration() const noexcept -> const FSMConfiguration&
{
    return m_context->getConfig();
}

auto ApplicationFSM::getMetrics() const noexcept -> const FSMMetrics&
{
    return m_context->getMetrics();
}

// ============================================
// State Persistence Methods
// ============================================

auto ApplicationFSM::saveState(const std::string& path) const -> bool
{
    const std::string filePath = path.empty() ? m_context->getConfig().stateSnapshotPath : path;

    if (filePath.empty())
    {
        LOG4CXX_WARN(logger, "No state snapshot path configured");
        return false;
    }

    try
    {
        std::ofstream ofs(filePath);
        if (!ofs)
        {
            LOG4CXX_ERROR(logger, "Failed to open state file for writing: " << filePath);
            return false;
        }

        // Write state information (simplified JSON-like format)
        ofs << "{\n";
        ofs << "  \"currentState\": \"" << getCurrentStateName() << "\",\n";
        ofs << "  \"uptime\": " << m_context->getUptime().count() << ",\n";
        ofs << "  \"degradedMode\": " << (m_context->isDegradedMode() ? "true" : "false") << "\n";
        ofs << "}\n";

        LOG4CXX_INFO(logger, "State saved to: " << filePath);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Exception saving state: " << e.what());
        return false;
    }
}

auto ApplicationFSM::restoreState(const std::string& path) -> bool
{
    const std::string filePath = path.empty() ? m_context->getConfig().stateSnapshotPath : path;

    if (filePath.empty())
    {
        LOG4CXX_WARN(logger, "No state snapshot path configured");
        return false;
    }

    try
    {
        std::ifstream ifs(filePath);
        if (!ifs)
        {
            LOG4CXX_WARN(logger, "State file not found: " << filePath);
            return false;
        }

        // TODO: Implement full state restoration
        LOG4CXX_INFO(logger, "State restoration not fully implemented yet");
        return false;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Exception restoring state: " << e.what());
        return false;
    }
}

// ============================================
// Diagnostics Methods
// ============================================

auto ApplicationFSM::getStatusReport() const -> std::string
{
    std::ostringstream oss;

    oss << "=== Application FSM Status Report ===\n";
    oss << "Current State: " << getCurrentStateName() << "\n";
    oss << "Uptime: " << m_context->getUptime().count() << " ms\n";
    oss << "Total Transitions: " << m_context->getMetrics().totalTransitions.load() << "\n";
    oss << "Recovery Attempts: " << m_context->getMetrics().recoveryAttempts.load() << "\n";
    oss << "Degraded Mode: " << (m_context->isDegradedMode() ? "Yes" : "No") << "\n";
    oss << "Emergency Mode: " << (m_context->isEmergencyMode() ? "Yes" : "No") << "\n";
    oss << "Operational: " << (isOperational() ? "Yes" : "No") << "\n";

    return oss.str();
}

auto ApplicationFSM::getTransitionHistory(int count) const -> std::vector<std::string>
{
    const auto& history = m_impl->diagnostics.transitionHistory;
    std::vector<std::string> result;

    int start = std::max(0, static_cast<int>(history.size()) - count);
    for (size_t i = start; i < history.size(); ++i)
    {
        result.push_back(history[i]);
    }

    return result;
}

auto ApplicationFSM::setVerboseLogging(bool enabled) -> void
{
    m_impl->diagnostics.verboseLogging = enabled;
    LOG4CXX_INFO(logger, "Verbose logging " << (enabled ? "enabled" : "disabled"));
}

auto ApplicationFSM::validateConfiguration(const FSMConfiguration& config) -> std::string
{
    std::ostringstream errors;

    if (config.maxCoreRecoveryAttempts < 0)
    {
        errors << "Invalid maxCoreRecoveryAttempts (" << config.maxCoreRecoveryAttempts << ")\n";
    }

    if (config.configLoadTimeout.count() <= 0)
    {
        errors << "Invalid configLoadTimeout\n";
    }

    if (config.shutdownTimeout.count() <= 0)
    {
        errors << "Invalid shutdownTimeout\n";
    }

    return errors.str();
}
