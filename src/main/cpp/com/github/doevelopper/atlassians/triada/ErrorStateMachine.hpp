/**
 * @file ErrorStateMachine.hpp
 * @brief State machine for error management using Boost.Statechart
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * Implements state machine for error handling with states:
 * - Initial/Ready: System operational, no errors
 * - Processing: Normal operation in progress
 * - Success: Operation completed successfully
 * - Warning: Non-critical issue detected
 * - RecoverableError: Error with recovery strategies available
 * - Handling: Actively processing error
 * - Retry: Attempting retry according to policy
 * - FatalError: Critical unrecoverable error
 * - Maintenance: External intervention required
 * - IdleAfterError: Safe state after error handling
 * - Shutdown: Graceful shutdown in progress
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINE_HPP

#include <memory>
#include <string>
#include <string_view>
#include <functional>
#include <optional>
#include <chrono>
#include <atomic>
#include <mutex>

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deferral.hpp>
#include <boost/mpl/list.hpp>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/triada/Error.hpp>

namespace com::github::doevelopper::atlassians::triada
{

// Forward declarations
class ErrorStateMachine;
class StateReady;
class StateProcessing;
class StateSuccess;
class StateWarning;
class StateRecoverableError;
class StateHandling;
class StateRetry;
class StateFatalError;
class StateMaintenance;
class StateIdleAfterError;
class StateShutdown;

namespace sc = boost::statechart;
namespace mpl = boost::mpl;

// ============================================
// Events
// ============================================

/**
 * @brief Base event with optional error context
 */
struct EventBase : sc::event<EventBase>
{
    std::optional<Error> error;
    std::string description;

    EventBase() = default;
    explicit EventBase(std::string_view desc) : description{desc} {}
    explicit EventBase(Error err) : error{std::move(err)} {}
    EventBase(Error err, std::string_view desc) : error{std::move(err)}, description{desc} {}
};

/**
 * @brief Start processing event
 */
struct EvStartProcessing : sc::event<EvStartProcessing>
{
    std::string operationId;
    explicit EvStartProcessing(std::string_view opId = {}) : operationId{opId} {}
};

/**
 * @brief Operation completed successfully
 */
struct EvOperationComplete : sc::event<EvOperationComplete>
{
    std::string result;
};

/**
 * @brief Warning occurred
 */
struct EvWarning : sc::event<EvWarning>
{
    Error warning;
    explicit EvWarning(Error warn) : warning{std::move(warn)} {}
};

/**
 * @brief Error occurred
 */
struct EvError : sc::event<EvError>
{
    Error error;
    explicit EvError(Error err) : error{std::move(err)} {}
};

/**
 * @brief Fatal error occurred
 */
struct EvFatalError : sc::event<EvFatalError>
{
    Error error;
    explicit EvFatalError(Error err) : error{std::move(err)} {}
};

/**
 * @brief Start handling error
 */
struct EvStartHandling : sc::event<EvStartHandling> {};

/**
 * @brief Handling completed
 */
struct EvHandlingComplete : sc::event<EvHandlingComplete>
{
    bool success{false};
};

/**
 * @brief Initiate retry
 */
struct EvRetry : sc::event<EvRetry>
{
    std::size_t attemptNumber{0};

    EvRetry() = default;
    explicit EvRetry(std::size_t attempt) : attemptNumber{attempt} {}
};

/**
 * @brief Retry succeeded
 */
struct EvRetrySuccess : sc::event<EvRetrySuccess> {};

/**
 * @brief Retry failed
 */
struct EvRetryFailed : sc::event<EvRetryFailed>
{
    Error error;
    bool hasMoreAttempts{false};
    explicit EvRetryFailed(Error err, bool more = false)
        : error{std::move(err)}, hasMoreAttempts{more} {}
};

/**
 * @brief Recovery complete
 */
struct EvRecoveryComplete : sc::event<EvRecoveryComplete>
{
    bool success{false};
};

/**
 * @brief Enter maintenance mode
 */
struct EvEnterMaintenance : sc::event<EvEnterMaintenance>
{
    std::string reason;
    explicit EvEnterMaintenance(std::string_view r = {}) : reason{r} {}
};

/**
 * @brief Exit maintenance mode
 */
struct EvExitMaintenance : sc::event<EvExitMaintenance> {};

/**
 * @brief Initiate shutdown
 */
struct EvShutdown : sc::event<EvShutdown>
{
    bool emergency{false};
    std::string reason;
};

/**
 * @brief Reset to ready state
 */
struct EvReset : sc::event<EvReset> {};

/**
 * @brief Acknowledge warning
 */
struct EvAcknowledge : sc::event<EvAcknowledge> {};

/**
 * @brief Timeout event
 */
struct EvTimeout : sc::event<EvTimeout>
{
    std::string context;
    explicit EvTimeout(std::string_view ctx = {}) : context{ctx} {}
};

// ============================================
// State Machine Context
// ============================================

/**
 * @brief Context shared across all states
 */
struct StateMachineContext
{
    std::optional<Error> currentError;
    std::optional<Error> lastError;
    std::string currentOperation;
    std::size_t retryCount{0};
    std::size_t maxRetries{3};
    std::chrono::steady_clock::time_point stateEntryTime;
    std::string correlationId;

    auto reset() -> void
    {
        currentError.reset();
        currentOperation.clear();
        retryCount = 0;
    }
};

// ============================================
// State Machine
// ============================================

/**
 * @brief Error management state machine
 */
class ErrorStateMachine : public sc::state_machine<ErrorStateMachine, StateReady>
{
public:
    static log4cxx::LoggerPtr logger;  // Public for state class access

    using StateChangeCallback = std::function<void(std::string_view fromState, std::string_view toState)>;
    using ErrorCallback = std::function<void(const Error&)>;

    ErrorStateMachine();
    ~ErrorStateMachine() noexcept;

    ErrorStateMachine(const ErrorStateMachine&) = delete;
    ErrorStateMachine& operator=(const ErrorStateMachine&) = delete;

    // ============================================
    // Context Access
    // ============================================

    [[nodiscard]] auto context() -> StateMachineContext&;
    [[nodiscard]] auto context() const -> const StateMachineContext&;

    // ============================================
    // State Queries
    // ============================================

    [[nodiscard]] auto currentStateName() const -> std::string;
    [[nodiscard]] auto isInErrorState() const -> bool;
    [[nodiscard]] auto isOperational() const -> bool;
    [[nodiscard]] auto canProcess() const -> bool;

    // ============================================
    // Callbacks
    // ============================================

    auto setStateChangeCallback(StateChangeCallback callback) -> void;
    auto setErrorCallback(ErrorCallback callback) -> void;

    auto notifyStateChange(std::string_view fromState, std::string_view toState) -> void;
    auto notifyError(const Error& error) -> void;

    // ============================================
    // Configuration
    // ============================================

    auto setMaxRetries(std::size_t max) -> void;
    auto setCorrelationId(std::string_view id) -> void;

private:
    StateMachineContext m_context;
    StateChangeCallback m_stateChangeCallback;
    ErrorCallback m_errorCallback;
    mutable std::mutex m_mutex;
};

// ============================================
// State Definitions
// ============================================

/**
 * @brief Ready state - system operational, no errors
 */
class StateReady : public sc::simple_state<StateReady, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvStartProcessing, StateProcessing>,
        sc::transition<EvWarning, StateWarning>,
        sc::transition<EvError, StateRecoverableError>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvShutdown, StateShutdown>,
        sc::transition<EvEnterMaintenance, StateMaintenance>
    >;

    StateReady();
    ~StateReady();

    static constexpr std::string_view name() { return "Ready"; }
};

/**
 * @brief Processing state - normal operation in progress
 */
class StateProcessing : public sc::simple_state<StateProcessing, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvOperationComplete, StateSuccess>,
        sc::transition<EvWarning, StateWarning>,
        sc::transition<EvError, StateRecoverableError>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvTimeout, StateRecoverableError>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateProcessing();
    ~StateProcessing();

    static constexpr std::string_view name() { return "Processing"; }
};

/**
 * @brief Success state - operation completed successfully
 */
class StateSuccess : public sc::simple_state<StateSuccess, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvReset, StateReady>,
        sc::transition<EvStartProcessing, StateProcessing>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateSuccess();
    ~StateSuccess();

    static constexpr std::string_view name() { return "Success"; }
};

/**
 * @brief Warning state - non-critical issue detected
 */
class StateWarning : public sc::simple_state<StateWarning, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvAcknowledge, StateReady>,
        sc::transition<EvError, StateRecoverableError>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvReset, StateReady>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateWarning();
    ~StateWarning();

    static constexpr std::string_view name() { return "Warning"; }
};

/**
 * @brief Recoverable error state
 */
class StateRecoverableError : public sc::simple_state<StateRecoverableError, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvStartHandling, StateHandling>,
        sc::transition<EvRetry, StateRetry>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvRecoveryComplete, StateIdleAfterError>,
        sc::transition<EvEnterMaintenance, StateMaintenance>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateRecoverableError();
    ~StateRecoverableError();

    static constexpr std::string_view name() { return "RecoverableError"; }
};

/**
 * @brief Handling state - actively processing error
 */
class StateHandling : public sc::simple_state<StateHandling, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvHandlingComplete, StateIdleAfterError>,
        sc::transition<EvRetry, StateRetry>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvTimeout, StateRecoverableError>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateHandling();
    ~StateHandling();

    static constexpr std::string_view name() { return "Handling"; }
};

/**
 * @brief Retry state - attempting retry
 */
class StateRetry : public sc::simple_state<StateRetry, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvRetrySuccess, StateReady>,
        sc::transition<EvRetryFailed, StateRecoverableError>,
        sc::transition<EvFatalError, StateFatalError>,
        sc::transition<EvTimeout, StateRecoverableError>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateRetry();
    ~StateRetry();

    static constexpr std::string_view name() { return "Retry"; }
};

/**
 * @brief Fatal error state - unrecoverable
 */
class StateFatalError : public sc::simple_state<StateFatalError, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvShutdown, StateShutdown>,
        sc::transition<EvEnterMaintenance, StateMaintenance>
    >;

    StateFatalError();
    ~StateFatalError();

    static constexpr std::string_view name() { return "FatalError"; }
};

/**
 * @brief Maintenance state - external intervention required
 */
class StateMaintenance : public sc::simple_state<StateMaintenance, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvExitMaintenance, StateReady>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateMaintenance();
    ~StateMaintenance();

    static constexpr std::string_view name() { return "Maintenance"; }
};

/**
 * @brief Idle after error state - safe state awaiting instructions
 */
class StateIdleAfterError : public sc::simple_state<StateIdleAfterError, ErrorStateMachine>
{
public:
    using reactions = mpl::list<
        sc::transition<EvReset, StateReady>,
        sc::transition<EvStartProcessing, StateProcessing>,
        sc::transition<EvEnterMaintenance, StateMaintenance>,
        sc::transition<EvShutdown, StateShutdown>
    >;

    StateIdleAfterError();
    ~StateIdleAfterError();

    static constexpr std::string_view name() { return "IdleAfterError"; }
};

/**
 * @brief Shutdown state - graceful shutdown in progress
 */
class StateShutdown : public sc::simple_state<StateShutdown, ErrorStateMachine>
{
public:
    using reactions = mpl::list<>;  // Terminal state

    StateShutdown();
    ~StateShutdown();

    static constexpr std::string_view name() { return "Shutdown"; }
};

/**
 * @brief Helper class for managing error state machine
 */
class ErrorStateManager
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using StateChangeCallback = ErrorStateMachine::StateChangeCallback;
    using ErrorCallback = ErrorStateMachine::ErrorCallback;

    ErrorStateManager();
    ~ErrorStateManager() noexcept;

    ErrorStateManager(const ErrorStateManager&) = delete;
    ErrorStateManager& operator=(const ErrorStateManager&) = delete;
    ErrorStateManager(ErrorStateManager&&) = delete;
    ErrorStateManager& operator=(ErrorStateManager&&) = delete;

    // ============================================
    // Lifecycle
    // ============================================

    auto start() -> void;
    auto stop() -> void;
    [[nodiscard]] auto isRunning() const -> bool;

    // ============================================
    // State Machine Access
    // ============================================

    [[nodiscard]] auto stateMachine() -> ErrorStateMachine&;
    [[nodiscard]] auto stateMachine() const -> const ErrorStateMachine&;

    // ============================================
    // Event Processing
    // ============================================

    template<typename Event>
    auto processEvent(const Event& event) -> void
    {
        std::lock_guard lock{m_mutex};
        if (m_running && m_stateMachine)
        {
            m_stateMachine->process_event(event);
        }
    }

    // ============================================
    // Convenience Methods
    // ============================================

    auto startProcessing(std::string_view operationId = {}) -> void;
    auto completeOperation() -> void;
    auto reportWarning(Error warning) -> void;
    auto reportError(Error error) -> void;
    auto reportFatalError(Error error) -> void;
    auto initiateRetry() -> void;
    auto enterMaintenance(std::string_view reason = {}) -> void;
    auto exitMaintenance() -> void;
    auto initiateShutdown(bool emergency = false, std::string_view reason = {}) -> void;
    auto reset() -> void;

    // ============================================
    // Queries
    // ============================================

    [[nodiscard]] auto currentState() const -> std::string;
    [[nodiscard]] auto isOperational() const -> bool;
    [[nodiscard]] auto canProcess() const -> bool;
    [[nodiscard]] auto isInErrorState() const -> bool;

    // ============================================
    // Callbacks
    // ============================================

    auto setStateChangeCallback(StateChangeCallback callback) -> void;
    auto setErrorCallback(ErrorCallback callback) -> void;

private:
    std::unique_ptr<ErrorStateMachine> m_stateMachine;
    std::atomic<bool> m_running{false};
    mutable std::mutex m_mutex;
};

} // namespace com::github::doevelopper::atlassians::triada

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINE_HPP
