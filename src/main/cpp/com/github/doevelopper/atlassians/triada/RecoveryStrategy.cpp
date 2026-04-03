/**
 * @file RecoveryStrategy.cpp
 * @brief Implementation of error recovery strategies
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/RecoveryStrategy.hpp>

#include <algorithm>
#include <cmath>
#include <random>

using namespace com::github::doevelopper::atlassians::triada;

namespace
{
    /**
     * @brief Calculate fibonacci number for backoff
     */
    auto fibonacciNumber(std::size_t n) -> std::size_t
    {
        if (n <= 1) return 1;
        std::size_t a = 1, b = 1;
        for (std::size_t i = 2; i <= n; ++i)
        {
            std::size_t c = a + b;
            a = b;
            b = c;
        }
        return b;
    }
}

log4cxx::LoggerPtr RetryPolicy::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.RetryPolicy"));

log4cxx::LoggerPtr RetryExecutor::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.RetryExecutor"));

log4cxx::LoggerPtr CircuitBreaker::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.CircuitBreaker"));

// ============================================
// RetryPolicy Implementation
// ============================================

RetryPolicy::RetryPolicy() noexcept = default;

RetryPolicy::RetryPolicy(
    std::size_t maxAttempts,
    BackoffType backoff,
    Duration initialDelay,
    Duration maxDelay) noexcept
    : m_maxAttempts{maxAttempts}
    , m_backoffType{backoff}
    , m_initialDelay{initialDelay}
    , m_maxDelay{maxDelay}
{
}

auto RetryPolicy::noRetry() -> RetryPolicy
{
    return RetryPolicy{1, BackoffType::None, Duration{0}, Duration{0}};
}

auto RetryPolicy::fixed(std::size_t maxAttempts, Duration delay) -> RetryPolicy
{
    return RetryPolicy{maxAttempts, BackoffType::Fixed, delay, delay};
}

auto RetryPolicy::linear(
    std::size_t maxAttempts,
    Duration initialDelay,
    Duration maxDelay) -> RetryPolicy
{
    return RetryPolicy{maxAttempts, BackoffType::Linear, initialDelay, maxDelay};
}

auto RetryPolicy::exponential(
    std::size_t maxAttempts,
    Duration initialDelay,
    Duration maxDelay,
    double multiplier) -> RetryPolicy
{
    RetryPolicy policy{maxAttempts, BackoffType::Exponential, initialDelay, maxDelay};
    policy.m_multiplier = multiplier;
    return policy;
}

auto RetryPolicy::fibonacci(
    std::size_t maxAttempts,
    Duration baseUnit,
    Duration maxDelay) -> RetryPolicy
{
    return RetryPolicy{maxAttempts, BackoffType::Fibonacci, baseUnit, maxDelay};
}

auto RetryPolicy::maxAttempts() const noexcept -> std::size_t
{
    return m_maxAttempts;
}

auto RetryPolicy::backoffType() const noexcept -> BackoffType
{
    return m_backoffType;
}

auto RetryPolicy::initialDelay() const noexcept -> Duration
{
    return m_initialDelay;
}

auto RetryPolicy::maxDelay() const noexcept -> Duration
{
    return m_maxDelay;
}

auto RetryPolicy::multiplier() const noexcept -> double
{
    return m_multiplier;
}

auto RetryPolicy::jitterFactor() const noexcept -> double
{
    return m_jitterFactor;
}

auto RetryPolicy::withMaxAttempts(std::size_t attempts) -> RetryPolicy&
{
    m_maxAttempts = attempts;
    return *this;
}

auto RetryPolicy::withBackoff(BackoffType backoff) -> RetryPolicy&
{
    m_backoffType = backoff;
    return *this;
}

auto RetryPolicy::withInitialDelay(Duration delay) -> RetryPolicy&
{
    m_initialDelay = delay;
    return *this;
}

auto RetryPolicy::withMaxDelay(Duration delay) -> RetryPolicy&
{
    m_maxDelay = delay;
    return *this;
}

auto RetryPolicy::withMultiplier(double mult) -> RetryPolicy&
{
    m_multiplier = mult;
    return *this;
}

auto RetryPolicy::withJitter(double factor) -> RetryPolicy&
{
    m_jitterFactor = std::clamp(factor, 0.0, 1.0);
    return *this;
}

namespace
{
    // Calculate Fibonacci number
    auto fibonacci(std::size_t n) -> std::size_t
    {
        if (n <= 1) return n;

        std::size_t a = 0, b = 1;
        for (std::size_t i = 2; i <= n; ++i)
        {
            std::size_t c = a + b;
            a = b;
            b = c;
        }
        return b;
    }
}

auto RetryPolicy::calculateDelay(std::size_t attempt) const -> Duration
{
    if (attempt == 0 || m_backoffType == BackoffType::None)
    {
        return Duration{0};
    }

    Duration delay;

    switch (m_backoffType)
    {
        case BackoffType::Fixed:
            delay = m_initialDelay;
            break;

        case BackoffType::Linear:
            delay = Duration{m_initialDelay.count() * static_cast<long long>(attempt)};
            break;

        case BackoffType::Exponential:
        {
            double multiplied = m_initialDelay.count() * std::pow(m_multiplier, attempt - 1);
            delay = Duration{static_cast<long long>(multiplied)};
            break;
        }

        case BackoffType::Fibonacci:
        {
            auto fibValue = fibonacciNumber(attempt + 1);
            delay = Duration(m_initialDelay.count() * static_cast<long long>(fibValue));
            break;
        }

        default:
            delay = m_initialDelay;
            break;
    }

    // Cap at max delay
    return std::min(delay, m_maxDelay);
}

auto RetryPolicy::calculateDelayWithJitter(std::size_t attempt) const -> Duration
{
    auto baseDelay = calculateDelay(attempt);

    if (m_jitterFactor <= 0.0 || baseDelay.count() == 0)
    {
        return baseDelay;
    }

    // Add random jitter
    static thread_local std::random_device rd;
    static thread_local std::mt19937 gen{rd()};

    auto jitterRange = static_cast<long long>(baseDelay.count() * m_jitterFactor);
    std::uniform_int_distribution<long long> dist{-jitterRange, jitterRange};

    auto jitteredDelay = baseDelay.count() + dist(gen);
    return Duration{std::max(0LL, jitteredDelay)};
}

// ============================================
// RetryContext Implementation
// ============================================

RetryContext::RetryContext() noexcept
    : m_startTime{Clock::now()}
{
}

RetryContext::RetryContext(const RetryPolicy& policy) noexcept
    : m_maxAttempts{policy.maxAttempts()}
    , m_startTime{Clock::now()}
{
}

auto RetryContext::attempt() const noexcept -> std::size_t
{
    return m_attempt;
}

auto RetryContext::hasMoreAttempts() const noexcept -> bool
{
    return m_attempt < m_maxAttempts;
}

auto RetryContext::isFirstAttempt() const noexcept -> bool
{
    return m_attempt == 0;
}

auto RetryContext::isLastAttempt() const noexcept -> bool
{
    return m_attempt >= m_maxAttempts - 1;
}

auto RetryContext::startTime() const noexcept -> TimePoint
{
    return m_startTime;
}

auto RetryContext::elapsedTime() const noexcept -> std::chrono::milliseconds
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - m_startTime);
}

auto RetryContext::lastError() const noexcept -> const Error&
{
    static Error emptyError;
    return m_errors.empty() ? emptyError : m_errors.back();
}

auto RetryContext::errors() const noexcept -> const std::vector<Error>&
{
    return m_errors;
}

auto RetryContext::incrementAttempt() -> void
{
    ++m_attempt;
}

auto RetryContext::recordError(Error err) -> void
{
    m_errors.push_back(std::move(err));
}

auto RetryContext::reset() -> void
{
    m_attempt = 0;
    m_startTime = Clock::now();
    m_errors.clear();
}

// ============================================
// RetryExecutor Implementation
// ============================================

RetryExecutor::RetryExecutor(RetryPolicy policy)
    : m_policy{std::move(policy)}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RetryExecutor::~RetryExecutor() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RetryExecutor::RetryExecutor(RetryExecutor&&) noexcept = default;
RetryExecutor& RetryExecutor::operator=(RetryExecutor&&) noexcept = default;

auto RetryExecutor::setRetryPredicate(RetryPredicate predicate) -> RetryExecutor&
{
    m_retryPredicate = std::move(predicate);
    return *this;
}

auto RetryExecutor::setBeforeRetry(BeforeRetryCallback callback) -> RetryExecutor&
{
    m_beforeRetry = std::move(callback);
    return *this;
}

auto RetryExecutor::setAfterRetry(AfterRetryCallback callback) -> RetryExecutor&
{
    m_afterRetry = std::move(callback);
    return *this;
}

// ============================================
// CircuitBreaker Implementation
// ============================================

CircuitBreaker::CircuitBreaker()
    : m_config{}
    , m_lastStateChange{Clock::now()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CircuitBreaker::CircuitBreaker(Config config)
    : m_config{std::move(config)}
    , m_lastStateChange{Clock::now()}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CircuitBreaker::~CircuitBreaker() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto CircuitBreaker::state() const noexcept -> State
{
    return m_state.load(std::memory_order_acquire);
}

auto CircuitBreaker::isAllowingRequests() const noexcept -> bool
{
    auto currentState = state();
    return currentState == State::Closed || currentState == State::HalfOpen;
}

auto CircuitBreaker::failureCount() const noexcept -> std::size_t
{
    return m_failureCount.load(std::memory_order_relaxed);
}

auto CircuitBreaker::successCount() const noexcept -> std::size_t
{
    return m_successCount.load(std::memory_order_relaxed);
}

auto CircuitBreaker::failureRate() const noexcept -> double
{
    auto total = m_totalCalls.load(std::memory_order_relaxed);
    if (total == 0)
    {
        return 0.0;
    }
    return static_cast<double>(m_failureCount.load(std::memory_order_relaxed)) /
           static_cast<double>(total);
}

auto CircuitBreaker::tryAcquire() -> bool
{
    auto currentState = state();

    switch (currentState)
    {
        case State::Closed:
            return true;

        case State::Open:
        {
            std::lock_guard lock{m_mutex};
            auto elapsed = Clock::now() - m_lastStateChange;
            if (elapsed >= m_config.timeout)
            {
                transitionTo(State::HalfOpen);
                return true;
            }
            return false;
        }

        case State::HalfOpen:
            // Allow limited requests in half-open state
            return true;

        default:
            return false;
    }
}

auto CircuitBreaker::recordSuccess() -> void
{
    m_successCount.fetch_add(1, std::memory_order_relaxed);
    m_totalCalls.fetch_add(1, std::memory_order_relaxed);

    auto currentState = state();

    if (currentState == State::HalfOpen)
    {
        std::lock_guard lock{m_mutex};
        if (m_successCount.load(std::memory_order_relaxed) >= m_config.successThreshold)
        {
            transitionTo(State::Closed);
        }
    }
}

auto CircuitBreaker::recordFailure() -> void
{
    m_failureCount.fetch_add(1, std::memory_order_relaxed);
    m_totalCalls.fetch_add(1, std::memory_order_relaxed);

    auto currentState = state();

    if (currentState == State::HalfOpen)
    {
        std::lock_guard lock{m_mutex};
        transitionTo(State::Open);
        return;
    }

    if (currentState == State::Closed)
    {
        std::lock_guard lock{m_mutex};

        bool shouldOpen = false;

        // Check absolute failure threshold
        if (m_failureCount.load(std::memory_order_relaxed) >= m_config.failureThreshold)
        {
            shouldOpen = true;
        }

        // Check failure rate threshold
        auto total = m_totalCalls.load(std::memory_order_relaxed);
        if (total >= m_config.minimumCalls && failureRate() >= m_config.failureRateThreshold)
        {
            shouldOpen = true;
        }

        if (shouldOpen)
        {
            transitionTo(State::Open);
        }
    }
}

auto CircuitBreaker::transitionTo(State newState) -> void
{
    auto oldState = m_state.exchange(newState, std::memory_order_release);

    if (oldState != newState)
    {
        m_lastStateChange = Clock::now();

        // Reset counters on state change
        if (newState == State::Closed || newState == State::HalfOpen)
        {
            m_failureCount.store(0, std::memory_order_relaxed);
            m_successCount.store(0, std::memory_order_relaxed);
            m_totalCalls.store(0, std::memory_order_relaxed);
        }

        LOG4CXX_INFO(logger, "Circuit breaker state changed from "
            << static_cast<int>(oldState) << " to " << static_cast<int>(newState));
    }
}

auto CircuitBreaker::reset() -> void
{
    std::lock_guard lock{m_mutex};
    transitionTo(State::Closed);
}

auto CircuitBreaker::forceOpen() -> void
{
    std::lock_guard lock{m_mutex};
    transitionTo(State::Open);
}

auto CircuitBreaker::forceClose() -> void
{
    std::lock_guard lock{m_mutex};
    transitionTo(State::Closed);
}
