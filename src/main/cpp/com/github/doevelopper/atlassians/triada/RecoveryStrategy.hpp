/**
 * @file RecoveryStrategy.hpp
 * @brief Error recovery strategies with configurable policies
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 *
 * Provides robust strategies to recover from errors:
 * - Automatic retry mechanisms with configurable policies
 * - Fallback procedures
 * - Graceful degradation
 * - Transaction rollback capabilities
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGY_HPP

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <random>
#include <thread>
#include <vector>
#include <atomic>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/triada/Error.hpp>

namespace com::github::doevelopper::atlassians::triada
{

/**
 * @brief Backoff strategy type
 */
enum class BackoffType
{
    None,           ///< No delay between retries
    Fixed,          ///< Fixed delay between retries
    Linear,         ///< Linearly increasing delay
    Exponential,    ///< Exponentially increasing delay (with jitter)
    Fibonacci       ///< Fibonacci sequence delay
};

/**
 * @brief Retry policy configuration
 *
 * Configures how retry operations should behave:
 * - Maximum attempts
 * - Backoff strategy
 * - Delay parameters
 * - Jitter for distributed systems
 */
class RetryPolicy
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using Duration = std::chrono::milliseconds;
    using Clock = std::chrono::steady_clock;

    /**
     * @brief Default constructor - 3 retries with exponential backoff
     */
    RetryPolicy() noexcept;

    /**
     * @brief Construct with specific parameters
     * @param maxAttempts Maximum number of attempts (including initial)
     * @param backoff Backoff strategy type
     * @param initialDelay Initial delay between retries
     * @param maxDelay Maximum delay cap
     */
    RetryPolicy(
        std::size_t maxAttempts,
        BackoffType backoff,
        Duration initialDelay,
        Duration maxDelay = Duration{30000}) noexcept;

    ~RetryPolicy() noexcept = default;
    RetryPolicy(const RetryPolicy&) = default;
    RetryPolicy& operator=(const RetryPolicy&) = default;
    RetryPolicy(RetryPolicy&&) noexcept = default;
    RetryPolicy& operator=(RetryPolicy&&) noexcept = default;

    // ============================================
    // Factory Methods
    // ============================================

    /**
     * @brief Create policy with no retries
     */
    [[nodiscard]] static auto noRetry() -> RetryPolicy;

    /**
     * @brief Create policy with fixed delay
     */
    [[nodiscard]] static auto fixed(
        std::size_t maxAttempts,
        Duration delay) -> RetryPolicy;

    /**
     * @brief Create policy with linear backoff
     */
    [[nodiscard]] static auto linear(
        std::size_t maxAttempts,
        Duration initialDelay,
        Duration maxDelay = Duration{30000}) -> RetryPolicy;

    /**
     * @brief Create policy with exponential backoff
     */
    [[nodiscard]] static auto exponential(
        std::size_t maxAttempts,
        Duration initialDelay,
        Duration maxDelay = Duration{30000},
        double multiplier = 2.0) -> RetryPolicy;

    /**
     * @brief Create policy with Fibonacci backoff
     */
    [[nodiscard]] static auto fibonacci(
        std::size_t maxAttempts,
        Duration baseUnit,
        Duration maxDelay = Duration{30000}) -> RetryPolicy;

    // ============================================
    // Accessors
    // ============================================

    [[nodiscard]] auto maxAttempts() const noexcept -> std::size_t;
    [[nodiscard]] auto backoffType() const noexcept -> BackoffType;
    [[nodiscard]] auto initialDelay() const noexcept -> Duration;
    [[nodiscard]] auto maxDelay() const noexcept -> Duration;
    [[nodiscard]] auto multiplier() const noexcept -> double;
    [[nodiscard]] auto jitterFactor() const noexcept -> double;

    // ============================================
    // Modifiers
    // ============================================

    auto withMaxAttempts(std::size_t attempts) -> RetryPolicy&;
    auto withBackoff(BackoffType backoff) -> RetryPolicy&;
    auto withInitialDelay(Duration delay) -> RetryPolicy&;
    auto withMaxDelay(Duration delay) -> RetryPolicy&;
    auto withMultiplier(double mult) -> RetryPolicy&;
    auto withJitter(double factor) -> RetryPolicy&;

    // ============================================
    // Delay Calculation
    // ============================================

    /**
     * @brief Calculate delay for a given attempt number
     * @param attempt Attempt number (0-based)
     * @return Delay duration for this attempt
     */
    [[nodiscard]] auto calculateDelay(std::size_t attempt) const -> Duration;

    /**
     * @brief Calculate delay with jitter
     */
    [[nodiscard]] auto calculateDelayWithJitter(std::size_t attempt) const -> Duration;

private:
    std::size_t m_maxAttempts{3};
    BackoffType m_backoffType{BackoffType::Exponential};
    Duration m_initialDelay{100};
    Duration m_maxDelay{30000};
    double m_multiplier{2.0};
    double m_jitterFactor{0.1};  // 10% jitter
};

/**
 * @brief Retry context for tracking retry state
 */
class RetryContext
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    RetryContext() noexcept;
    explicit RetryContext(const RetryPolicy& policy) noexcept;

    [[nodiscard]] auto attempt() const noexcept -> std::size_t;
    [[nodiscard]] auto hasMoreAttempts() const noexcept -> bool;
    [[nodiscard]] auto isFirstAttempt() const noexcept -> bool;
    [[nodiscard]] auto isLastAttempt() const noexcept -> bool;
    [[nodiscard]] auto startTime() const noexcept -> TimePoint;
    [[nodiscard]] auto elapsedTime() const noexcept -> std::chrono::milliseconds;
    [[nodiscard]] auto lastError() const noexcept -> const Error&;
    [[nodiscard]] auto errors() const noexcept -> const std::vector<Error>&;

    auto incrementAttempt() -> void;
    auto recordError(Error err) -> void;
    auto reset() -> void;

private:
    std::size_t m_attempt{0};
    std::size_t m_maxAttempts{3};
    TimePoint m_startTime;
    std::vector<Error> m_errors;
};

/**
 * @brief Retry executor for automatic retry with policy
 */
class RetryExecutor
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using RetryPredicate = std::function<bool(const Error&, const RetryContext&)>;
    using BeforeRetryCallback = std::function<void(const RetryContext&)>;
    using AfterRetryCallback = std::function<void(const RetryContext&, bool success)>;

    /**
     * @brief Construct with retry policy
     */
    explicit RetryExecutor(RetryPolicy policy);

    ~RetryExecutor() noexcept;
    RetryExecutor(const RetryExecutor&) = delete;
    RetryExecutor& operator=(const RetryExecutor&) = delete;
    RetryExecutor(RetryExecutor&&) noexcept;
    RetryExecutor& operator=(RetryExecutor&&) noexcept;

    // ============================================
    // Configuration
    // ============================================

    /**
     * @brief Set predicate to determine if error is retryable
     */
    auto setRetryPredicate(RetryPredicate predicate) -> RetryExecutor&;

    /**
     * @brief Set callback before each retry
     */
    auto setBeforeRetry(BeforeRetryCallback callback) -> RetryExecutor&;

    /**
     * @brief Set callback after each retry attempt
     */
    auto setAfterRetry(AfterRetryCallback callback) -> RetryExecutor&;

    // ============================================
    // Execution
    // ============================================

    /**
     * @brief Execute operation with retry
     * @tparam F Function returning Result<T>
     * @param operation Operation to execute
     * @return Result from successful attempt or last error
     */
    template<typename F>
    [[nodiscard]] auto execute(F&& operation) -> std::invoke_result_t<F>
    {
        using ResultType = std::invoke_result_t<F>;

        RetryContext ctx{m_policy};

        while (ctx.hasMoreAttempts())
        {
            if (!ctx.isFirstAttempt())
            {
                auto delay = m_policy.calculateDelayWithJitter(ctx.attempt() - 1);

                if (m_beforeRetry)
                {
                    m_beforeRetry(ctx);
                }

                std::this_thread::sleep_for(delay);
            }

            auto result = std::invoke(std::forward<F>(operation));

            if (result)
            {
                if (m_afterRetry)
                {
                    m_afterRetry(ctx, true);
                }
                return result;
            }

            ctx.recordError(result.error());

            bool shouldRetry = true;
            if (m_retryPredicate)
            {
                shouldRetry = m_retryPredicate(result.error(), ctx);
            }
            else
            {
                // Default: retry only recoverable errors
                shouldRetry = result.error().isRecoverable();
            }

            if (m_afterRetry)
            {
                m_afterRetry(ctx, false);
            }

            if (!shouldRetry)
            {
                return result;
            }

            ctx.incrementAttempt();
        }

        // Return last error with aggregated context
        auto finalError = ctx.lastError();
        finalError.withVariable("total_attempts", ctx.attempt());
        finalError.withVariable("elapsed_ms", ctx.elapsedTime().count());

        return ResultType{std::move(finalError)};
    }

    /**
     * @brief Execute void operation with retry
     */
    template<typename F>
    requires std::is_same_v<std::invoke_result_t<F>, Status>
    [[nodiscard]] auto execute(F&& operation) -> Status
    {
        RetryContext ctx{m_policy};

        while (ctx.hasMoreAttempts())
        {
            if (!ctx.isFirstAttempt())
            {
                auto delay = m_policy.calculateDelayWithJitter(ctx.attempt() - 1);

                if (m_beforeRetry)
                {
                    m_beforeRetry(ctx);
                }

                std::this_thread::sleep_for(delay);
            }

            auto result = std::invoke(std::forward<F>(operation));

            if (result)
            {
                if (m_afterRetry)
                {
                    m_afterRetry(ctx, true);
                }
                return result;
            }

            ctx.recordError(result.error());

            bool shouldRetry = true;
            if (m_retryPredicate)
            {
                shouldRetry = m_retryPredicate(result.error(), ctx);
            }
            else
            {
                shouldRetry = result.error().isRecoverable();
            }

            if (m_afterRetry)
            {
                m_afterRetry(ctx, false);
            }

            if (!shouldRetry)
            {
                return result;
            }

            ctx.incrementAttempt();
        }

        auto finalError = ctx.lastError();
        finalError.withVariable("total_attempts", ctx.attempt());
        finalError.withVariable("elapsed_ms", ctx.elapsedTime().count());

        return Status{std::move(finalError)};
    }

private:
    RetryPolicy m_policy;
    RetryPredicate m_retryPredicate;
    BeforeRetryCallback m_beforeRetry;
    AfterRetryCallback m_afterRetry;
};

/**
 * @brief Fallback chain for graceful degradation
 *
 * Allows defining a chain of fallback operations that are
 * tried in sequence if the primary operation fails.
 */
template<typename T>
class FallbackChain
{
public:
    using Operation = std::function<Result<T>()>;
    using OperationWithContext = std::function<Result<T>(const Error&)>;

    FallbackChain() = default;

    /**
     * @brief Add primary operation
     */
    auto withPrimary(Operation op) -> FallbackChain&
    {
        m_primary = std::move(op);
        return *this;
    }

    /**
     * @brief Add fallback operation
     */
    auto withFallback(Operation op) -> FallbackChain&
    {
        m_fallbacks.push_back([op = std::move(op)](const Error&) {
            return op();
        });
        return *this;
    }

    /**
     * @brief Add fallback with access to previous error
     */
    auto withFallback(OperationWithContext op) -> FallbackChain&
    {
        m_fallbacks.push_back(std::move(op));
        return *this;
    }

    /**
     * @brief Add default value fallback
     */
    auto withDefault(T value) -> FallbackChain&
    {
        m_fallbacks.push_back([value = std::move(value)](const Error&) {
            return Result<T>{value};
        });
        return *this;
    }

    /**
     * @brief Execute chain
     */
    [[nodiscard]] auto execute() -> Result<T>
    {
        if (!m_primary)
        {
            return Error::create(
                ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0, 1),
                "No primary operation defined");
        }

        auto result = m_primary();
        if (result)
        {
            return result;
        }

        Error lastError = result.error();

        for (auto& fallback : m_fallbacks)
        {
            result = fallback(lastError);
            if (result)
            {
                return result;
            }
            lastError = result.error();
        }

        return Result<T>{lastError};
    }

private:
    Operation m_primary;
    std::vector<OperationWithContext> m_fallbacks;
};

/**
 * @brief Circuit breaker for preventing cascading failures
 */
class CircuitBreaker
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    enum class State
    {
        Closed,     ///< Normal operation, allowing requests
        Open,       ///< Failing fast, not allowing requests
        HalfOpen    ///< Testing if service has recovered
    };

    using Duration = std::chrono::milliseconds;
    using Clock = std::chrono::steady_clock;

    /**
     * @brief Configuration for circuit breaker
     */
    struct Config
    {
        std::size_t failureThreshold = 5;      ///< Failures before opening
        std::size_t successThreshold = 2;      ///< Successes to close from half-open
        Duration timeout = Duration{30000};     ///< Time before trying half-open
        double failureRateThreshold = 0.5;      ///< Failure rate threshold (0-1)
        std::size_t minimumCalls = 10;          ///< Minimum calls before rate calculation
    };

    CircuitBreaker();
    explicit CircuitBreaker(Config config);
    ~CircuitBreaker() noexcept;

    CircuitBreaker(const CircuitBreaker&) = delete;
    CircuitBreaker& operator=(const CircuitBreaker&) = delete;
    CircuitBreaker(CircuitBreaker&&) = delete;
    CircuitBreaker& operator=(CircuitBreaker&&) = delete;

    // ============================================
    // State Queries
    // ============================================

    [[nodiscard]] auto state() const noexcept -> State;
    [[nodiscard]] auto isAllowingRequests() const noexcept -> bool;
    [[nodiscard]] auto failureCount() const noexcept -> std::size_t;
    [[nodiscard]] auto successCount() const noexcept -> std::size_t;
    [[nodiscard]] auto failureRate() const noexcept -> double;

    // ============================================
    // Execution
    // ============================================

    /**
     * @brief Execute operation through circuit breaker
     */
    template<typename F>
    [[nodiscard]] auto execute(F&& operation) -> std::invoke_result_t<F>
    {
        using ResultType = std::invoke_result_t<F>;

        if (!tryAcquire())
        {
            return ResultType{Error::create(
                ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0, 2),
                "Circuit breaker is open")};
        }

        auto result = std::invoke(std::forward<F>(operation));

        if (result)
        {
            recordSuccess();
        }
        else
        {
            recordFailure();
        }

        return result;
    }

    // ============================================
    // Manual Control
    // ============================================

    auto reset() -> void;
    auto forceOpen() -> void;
    auto forceClose() -> void;

private:
    auto tryAcquire() -> bool;
    auto recordSuccess() -> void;
    auto recordFailure() -> void;
    auto transitionTo(State newState) -> void;

    Config m_config;
    std::atomic<State> m_state{State::Closed};
    std::atomic<std::size_t> m_failureCount{0};
    std::atomic<std::size_t> m_successCount{0};
    std::atomic<std::size_t> m_totalCalls{0};
    Clock::time_point m_lastStateChange;
    mutable std::mutex m_mutex;
};

} // namespace com::github::doevelopper::atlassians::triada

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGY_HPP
