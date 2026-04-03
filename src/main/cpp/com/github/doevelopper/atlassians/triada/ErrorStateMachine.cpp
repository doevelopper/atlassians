/**
 * @file ErrorStateMachine.cpp
 * @brief Implementation of error management state machine
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorStateMachine.hpp>

using namespace com::github::doevelopper::atlassians::triada;

log4cxx::LoggerPtr ErrorStateMachine::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorStateMachine"));

log4cxx::LoggerPtr ErrorStateManager::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorStateManager"));

// ============================================
// ErrorStateMachine Implementation
// ============================================

ErrorStateMachine::ErrorStateMachine()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorStateMachine::~ErrorStateMachine() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorStateMachine::context() -> StateMachineContext&
{
    return m_context;
}

auto ErrorStateMachine::context() const -> const StateMachineContext&
{
    return m_context;
}

auto ErrorStateMachine::currentStateName() const -> std::string
{
    // This is a simplified implementation
    // In practice, you'd query the actual current state
    return "Unknown";
}

auto ErrorStateMachine::isInErrorState() const -> bool
{
    // Check if in any error-related state
    return false;  // Would check actual state
}

auto ErrorStateMachine::isOperational() const -> bool
{
    return !isInErrorState();
}

auto ErrorStateMachine::canProcess() const -> bool
{
    return isOperational();
}

auto ErrorStateMachine::setStateChangeCallback(StateChangeCallback callback) -> void
{
    std::lock_guard lock{m_mutex};
    m_stateChangeCallback = std::move(callback);
}

auto ErrorStateMachine::setErrorCallback(ErrorCallback callback) -> void
{
    std::lock_guard lock{m_mutex};
    m_errorCallback = std::move(callback);
}

auto ErrorStateMachine::notifyStateChange(std::string_view fromState, std::string_view toState) -> void
{
    LOG4CXX_INFO(logger, "State transition: " << fromState << " -> " << toState);

    std::lock_guard lock{m_mutex};
    if (m_stateChangeCallback)
    {
        m_stateChangeCallback(fromState, toState);
    }
}

auto ErrorStateMachine::notifyError(const Error& error) -> void
{
    LOG4CXX_ERROR(logger, "Error in state machine: " << error.toString());

    std::lock_guard lock{m_mutex};
    if (m_errorCallback)
    {
        m_errorCallback(error);
    }
}

auto ErrorStateMachine::setMaxRetries(std::size_t max) -> void
{
    m_context.maxRetries = max;
}

auto ErrorStateMachine::setCorrelationId(std::string_view id) -> void
{
    m_context.correlationId = id;
}

// ============================================
// State Implementations
// ============================================

StateReady::StateReady()
{
    // NOTE: Cannot access context<>() or outermost_context() in initial state constructor
    // because state machine is not fully initialized yet
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Ready");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.context().reset();
}

StateReady::~StateReady()
{
    // Cannot safely call context<>() in destructor of initial state
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Ready");
    machine.notifyStateChange("Ready", "");
}

StateProcessing::StateProcessing()
{
    // Entry actions - safe since this is not the initial state
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Processing");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Processing");
}

StateProcessing::~StateProcessing()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Processing");
}

StateSuccess::StateSuccess()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Success");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Success");
}

StateSuccess::~StateSuccess()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Success");
}

StateWarning::StateWarning()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Warning");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Warning");
}

StateWarning::~StateWarning()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Warning");
}

StateRecoverableError::StateRecoverableError()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: RecoverableError");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "RecoverableError");

    // if (machine.context().currentError)
    // {
    //     machine.notifyError(*machine.context().currentError);
    // }
}

StateRecoverableError::~StateRecoverableError()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: RecoverableError");
}

StateHandling::StateHandling()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Handling");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Handling");
}

StateHandling::~StateHandling()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Handling");
}

StateRetry::StateRetry()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Retry");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.context().retryCount++;
    // machine.notifyStateChange("", "Retry");

    // LOG4CXX_INFO(machine.logger,
    //     "Retry attempt " << machine.context().retryCount
    //     << " of " << machine.context().maxRetries);
}

StateRetry::~StateRetry()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Retry");
}

StateFatalError::StateFatalError()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: FatalError");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "FatalError");

    // if (machine.context().currentError)
    // {
    //     LOG4CXX_FATAL(machine.logger,
    //         "Fatal error: " << machine.context().currentError->toString());
    //     machine.notifyError(*machine.context().currentError);
    // }
}

StateFatalError::~StateFatalError()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: FatalError");
}

StateMaintenance::StateMaintenance()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Maintenance");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Maintenance");
}

StateMaintenance::~StateMaintenance()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Maintenance");
}

StateIdleAfterError::StateIdleAfterError()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: IdleAfterError");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "IdleAfterError");

    // // Save current error as last error
    // if (machine.context().currentError)
    // {
    //     machine.context().lastError = machine.context().currentError;
    // }
}

StateIdleAfterError::~StateIdleAfterError()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: IdleAfterError");
}

StateShutdown::StateShutdown()
{
    // Entry actions
    // auto& machine = context<ErrorStateMachine>();
    // LOG4CXX_DEBUG(machine.logger, "Entering state: Shutdown");
    // machine.context().stateEntryTime = std::chrono::steady_clock::now();
    // machine.notifyStateChange("", "Shutdown");

    // LOG4CXX_INFO(machine.logger, "System shutdown initiated");
}

StateShutdown::~StateShutdown()
{
    // Exit actions
    auto& machine = context<ErrorStateMachine>();
    LOG4CXX_DEBUG(machine.logger, "Exiting state: Shutdown");
}

// ============================================
// ErrorStateManager Implementation
// ============================================

ErrorStateManager::ErrorStateManager()
    : m_stateMachine{std::make_unique<ErrorStateMachine>()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorStateManager::~ErrorStateManager() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    stop();
}

auto ErrorStateManager::start() -> void
{
    std::lock_guard lock{m_mutex};

    if (!m_running)
    {
        LOG4CXX_INFO(logger, "Starting error state manager");
        LOG4CXX_DEBUG(logger, "About to call initiate()");
        m_stateMachine->initiate();
        LOG4CXX_DEBUG(logger, "initiate() completed");
        m_running = true;
        LOG4CXX_INFO(logger, "Error state manager started successfully");
    }
}

auto ErrorStateManager::stop() -> void
{
    std::lock_guard lock{m_mutex};

    if (m_running)
    {
        LOG4CXX_INFO(logger, "Stopping error state manager");
        m_stateMachine->terminate();
        m_running = false;
    }
}

auto ErrorStateManager::isRunning() const -> bool
{
    return m_running.load(std::memory_order_acquire);
}

auto ErrorStateManager::stateMachine() -> ErrorStateMachine&
{
    return *m_stateMachine;
}

auto ErrorStateManager::stateMachine() const -> const ErrorStateMachine&
{
    return *m_stateMachine;
}

auto ErrorStateManager::startProcessing(std::string_view operationId) -> void
{
    processEvent(EvStartProcessing{operationId});
}

auto ErrorStateManager::completeOperation() -> void
{
    processEvent(EvOperationComplete{});
}

auto ErrorStateManager::reportWarning(Error warning) -> void
{
    processEvent(EvWarning{std::move(warning)});
}

auto ErrorStateManager::reportError(Error error) -> void
{
    {
        std::lock_guard lock{m_mutex};
        if (m_stateMachine)
        {
            m_stateMachine->context().currentError = error;
        }
    }
    processEvent(EvError{std::move(error)});
}

auto ErrorStateManager::reportFatalError(Error error) -> void
{
    {
        std::lock_guard lock{m_mutex};
        if (m_stateMachine)
        {
            m_stateMachine->context().currentError = error;
        }
    }
    processEvent(EvFatalError{std::move(error)});
}

auto ErrorStateManager::initiateRetry() -> void
{
    std::size_t attempt = 0;
    {
        std::lock_guard lock{m_mutex};
        if (m_stateMachine)
        {
            attempt = m_stateMachine->context().retryCount;
        }
    }
    processEvent(EvRetry{attempt});
}

auto ErrorStateManager::enterMaintenance(std::string_view reason) -> void
{
    processEvent(EvEnterMaintenance{reason});
}

auto ErrorStateManager::exitMaintenance() -> void
{
    processEvent(EvExitMaintenance{});
}

auto ErrorStateManager::initiateShutdown(bool emergency, std::string_view reason) -> void
{
    EvShutdown event;
    event.emergency = emergency;
    event.reason = reason;
    processEvent(event);
}

auto ErrorStateManager::reset() -> void
{
    processEvent(EvReset{});
}

auto ErrorStateManager::currentState() const -> std::string
{
    std::lock_guard lock{m_mutex};
    if (m_stateMachine)
    {
        return m_stateMachine->currentStateName();
    }
    return "Unknown";
}

auto ErrorStateManager::isOperational() const -> bool
{
    std::lock_guard lock{m_mutex};
    return m_running && m_stateMachine && m_stateMachine->isOperational();
}

auto ErrorStateManager::canProcess() const -> bool
{
    std::lock_guard lock{m_mutex};
    return m_running && m_stateMachine && m_stateMachine->canProcess();
}

auto ErrorStateManager::isInErrorState() const -> bool
{
    std::lock_guard lock{m_mutex};
    return m_stateMachine && m_stateMachine->isInErrorState();
}

auto ErrorStateManager::setStateChangeCallback(StateChangeCallback callback) -> void
{
    std::lock_guard lock{m_mutex};
    if (m_stateMachine)
    {
        m_stateMachine->setStateChangeCallback(std::move(callback));
    }
}

auto ErrorStateManager::setErrorCallback(ErrorCallback callback) -> void
{
    std::lock_guard lock{m_mutex};
    if (m_stateMachine)
    {
        m_stateMachine->setErrorCallback(std::move(callback));
    }
}
