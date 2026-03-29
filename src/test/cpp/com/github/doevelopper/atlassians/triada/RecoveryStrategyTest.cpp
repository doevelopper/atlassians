/**
 * @file RecoveryStrategyTest.cpp
 * @brief Unit tests for recovery strategy classes implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/premisses/triada/RecoveryStrategyTest.hpp>

#include <thread>
#include <atomic>

using namespace com::github::doevelopper::premisses::triada;
using namespace com::github::doevelopper::premisses::triada::test;

log4cxx::LoggerPtr RetryPolicyTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.test.RetryPolicyTest"));

log4cxx::LoggerPtr CircuitBreakerTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.test.CircuitBreakerTest"));

log4cxx::LoggerPtr FallbackChainTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.test.FallbackChainTest"));

// ============================================
// RetryPolicyTest Implementation
// ============================================

RetryPolicyTest::RetryPolicyTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

RetryPolicyTest::~RetryPolicyTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto RetryPolicyTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto RetryPolicyTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// RetryPolicy Construction Tests
// ============================================

TEST_F(RetryPolicyTest, DefaultConstructor_SetsDefaultValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    RetryPolicy policy;
    
    EXPECT_EQ(policy.maxAttempts(), 3);
    EXPECT_EQ(policy.backoffType(), BackoffType::Exponential);
}

TEST_F(RetryPolicyTest, FixedBackoff_CreatesPolicy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fixed(3, std::chrono::milliseconds(100));
    
    EXPECT_EQ(policy.maxAttempts(), 3);
    EXPECT_EQ(policy.backoffType(), BackoffType::Fixed);
}

TEST_F(RetryPolicyTest, ExponentialBackoff_CreatesPolicy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::exponential(
        5,
        std::chrono::milliseconds(100),
        std::chrono::seconds(5)
    );
    
    EXPECT_EQ(policy.maxAttempts(), 5);
    EXPECT_EQ(policy.backoffType(), BackoffType::Exponential);
}

TEST_F(RetryPolicyTest, LinearBackoff_CreatesPolicy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::linear(4, std::chrono::milliseconds(100));
    
    EXPECT_EQ(policy.maxAttempts(), 4);
    EXPECT_EQ(policy.backoffType(), BackoffType::Linear);
}

TEST_F(RetryPolicyTest, FibonacciBackoff_CreatesPolicy)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fibonacci(5, std::chrono::milliseconds(100));
    
    EXPECT_EQ(policy.maxAttempts(), 5);
    EXPECT_EQ(policy.backoffType(), BackoffType::Fibonacci);
}

// ============================================
// Backoff Calculation Tests
// ============================================

TEST_F(RetryPolicyTest, FixedBackoff_ReturnsConstantDelay)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fixed(5, std::chrono::milliseconds(100));
    
    // Attempt 0 returns 0 (no delay for first try)
    EXPECT_EQ(policy.calculateDelay(0), std::chrono::milliseconds(0));
    // Attempts 1+ return the fixed delay
    EXPECT_EQ(policy.calculateDelay(1), std::chrono::milliseconds(100));
    EXPECT_EQ(policy.calculateDelay(5), std::chrono::milliseconds(100));
}

TEST_F(RetryPolicyTest, ExponentialBackoff_IncreasesExponentially)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::exponential(
        10,
        std::chrono::milliseconds(100),
        std::chrono::seconds(60)
    );
    
    auto delay0 = policy.calculateDelay(0);
    auto delay1 = policy.calculateDelay(1);
    auto delay2 = policy.calculateDelay(2);
    
    // Should be increasing
    EXPECT_GE(delay1.count(), delay0.count());
    EXPECT_GE(delay2.count(), delay1.count());
}

TEST_F(RetryPolicyTest, ExponentialBackoff_CapsAtMaxDelay)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::exponential(
        10,
        std::chrono::milliseconds(100),
        std::chrono::milliseconds(500)
    );
    
    // After a few iterations, should cap at maxDelay
    EXPECT_LE(policy.calculateDelay(10).count(), 500);
}

// ============================================
// Retry Context Tests
// ============================================

TEST_F(RetryPolicyTest, RetryContext_TracksAttempts)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    RetryPolicy policy = RetryPolicy::fixed(3, std::chrono::milliseconds(10));
    RetryContext ctx(policy);
    
    EXPECT_EQ(ctx.attempt(), 0);
    EXPECT_TRUE(ctx.hasMoreAttempts());
    EXPECT_TRUE(ctx.isFirstAttempt());
    
    ctx.incrementAttempt();
    EXPECT_EQ(ctx.attempt(), 1);
    EXPECT_FALSE(ctx.isFirstAttempt());
}

// ============================================
// RetryExecutor Tests
// ============================================

TEST_F(RetryPolicyTest, RetryExecutor_SucceedsOnFirstAttempt)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fixed(3, std::chrono::milliseconds(1));
    RetryExecutor executor(policy);
    
    auto result = executor.execute([]() -> Result<int> {
        return Result<int>(42);
    });
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 42);
}

TEST_F(RetryPolicyTest, RetryExecutor_RetriesOnFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fixed(5, std::chrono::milliseconds(1));
    RetryExecutor executor(policy);
    std::atomic<int> attempts{0};
    
    auto result = executor.execute([&attempts]() -> Result<int> {
        attempts++;
        if (attempts < 3)
        {
            auto err = Error::create(
                ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0x01, 0x01),
                "Transient error"
            );
            return Result<int>(std::move(err));
        }
        return Result<int>(42);
    });
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(attempts.load(), 3);
}

TEST_F(RetryPolicyTest, RetryExecutor_FailsAfterMaxAttempts)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto policy = RetryPolicy::fixed(2, std::chrono::milliseconds(1));
    RetryExecutor executor(policy);
    
    auto result = executor.execute([]() -> Result<int> {
        auto err = Error::create(
            ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0x01, 0x01),
            "Persistent error"
        );
        return Result<int>(std::move(err));
    });
    
    EXPECT_TRUE(result.hasError());
}

// ============================================
// CircuitBreakerTest Implementation
// ============================================

CircuitBreakerTest::CircuitBreakerTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CircuitBreakerTest::~CircuitBreakerTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto CircuitBreakerTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto CircuitBreakerTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// CircuitBreaker State Tests
// ============================================

TEST_F(CircuitBreakerTest, InitialState_IsClosed)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker breaker;
    
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Closed);
    EXPECT_TRUE(breaker.isAllowingRequests());
}

TEST_F(CircuitBreakerTest, OpensAfterFailureThreshold)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker::Config config;
    config.failureThreshold = 3;
    config.timeout = std::chrono::milliseconds(100);
    CircuitBreaker breaker(config);
    
    // Record failures through execute
    for (int i = 0; i < 3; ++i)
    {
        breaker.execute([]() -> Result<int> {
            return Error::create(
                ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
                "Failure"
            );
        });
    }
    
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Open);
    EXPECT_FALSE(breaker.isAllowingRequests());
}

TEST_F(CircuitBreakerTest, StaysClosedAfterSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker::Config config;
    config.failureThreshold = 3;
    CircuitBreaker breaker(config);
    
    // Record some failures but not enough to open
    breaker.execute([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Failure"
        );
    });
    breaker.execute([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Failure"
        );
    });
    
    // Record success
    breaker.execute([]() -> Result<int> {
        return Result<int>(42);
    });
    
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Closed);
    EXPECT_TRUE(breaker.isAllowingRequests());
}

TEST_F(CircuitBreakerTest, Execute_SuccessfulOperation)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker breaker;
    
    auto result = breaker.execute([]() -> Result<int> {
        return Result<int>(42);
    });
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 42);
}

TEST_F(CircuitBreakerTest, Execute_FailsWhenOpen)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker::Config config;
    config.failureThreshold = 1;
    config.timeout = std::chrono::seconds(60);
    CircuitBreaker breaker(config);
    
    // Open the circuit
    breaker.execute([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Failure"
        );
    });
    
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Open);
    
    // Execute should fail immediately
    auto result = breaker.execute([]() -> Result<int> {
        return Result<int>(42);
    });
    
    EXPECT_TRUE(result.hasError());
}

TEST_F(CircuitBreakerTest, Reset_ClosesCircuit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker::Config config;
    config.failureThreshold = 1;
    config.timeout = std::chrono::seconds(60);
    CircuitBreaker breaker(config);
    
    // Open the circuit
    breaker.execute([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Failure"
        );
    });
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Open);
    
    // Reset should close it
    breaker.reset();
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Closed);
    EXPECT_TRUE(breaker.isAllowingRequests());
}

TEST_F(CircuitBreakerTest, ForceOpen_OpensCircuit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker breaker;
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Closed);
    
    breaker.forceOpen();
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Open);
}

TEST_F(CircuitBreakerTest, ForceClose_ClosesCircuit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    CircuitBreaker breaker;
    breaker.forceOpen();
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Open);
    
    breaker.forceClose();
    EXPECT_EQ(breaker.state(), CircuitBreaker::State::Closed);
}

// ============================================
// FallbackChainTest Implementation
// ============================================

FallbackChainTest::FallbackChainTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

FallbackChainTest::~FallbackChainTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto FallbackChainTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto FallbackChainTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// FallbackChain Tests
// ============================================

TEST_F(FallbackChainTest, Execute_PrimarySucceeds)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    chain.withPrimary([]() -> Result<int> {
        return Result<int>(1);
    });
    chain.withFallback([]() -> Result<int> {
        return Result<int>(2);
    });
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 1);
}

TEST_F(FallbackChainTest, Execute_FallsBackOnFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    chain.withPrimary([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Primary failed"
        );
    });
    chain.withFallback([]() -> Result<int> {
        return Result<int>(42);
    });
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 42);
}

TEST_F(FallbackChainTest, Execute_MultipleFallbacks)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<std::string> chain;
    
    chain.withPrimary([]() -> Result<std::string> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Network, 0x01, 0x01),
            "Primary network failed"
        );
    });
    chain.withFallback([]() -> Result<std::string> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Network, 0x01, 0x02),
            "Secondary network failed"
        );
    });
    chain.withFallback([]() -> Result<std::string> {
        return Result<std::string>("Cached data");
    });
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), "Cached data");
}

TEST_F(FallbackChainTest, Execute_AllFail)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    
    chain.withPrimary([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Primary failed"
        );
    });
    chain.withFallback([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x02),
            "Fallback failed"
        );
    });
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasError());
}

TEST_F(FallbackChainTest, Execute_NoPrimary)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasError());
}

TEST_F(FallbackChainTest, WithDefault_ReturnsDefaultOnFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    chain.withPrimary([]() -> Result<int> {
        return Error::create(
            ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
            "Primary failed"
        );
    });
    chain.withDefault(0);  // Default value
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 0);
}

TEST_F(FallbackChainTest, ChainableWithFallback)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    FallbackChain<int> chain;
    
    chain.withPrimary([]() -> Result<int> { return Result<int>(1); })
         .withFallback([]() -> Result<int> { return Result<int>(2); })
         .withFallback([]() -> Result<int> { return Result<int>(3); });
    
    auto result = chain.execute();
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), 1);
}

