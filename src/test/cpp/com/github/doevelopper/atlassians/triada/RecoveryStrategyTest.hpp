/**
 * @file RecoveryStrategyTest.hpp
 * @brief Unit tests for recovery strategy classes
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGYTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGYTEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/triada/RecoveryStrategy.hpp>

namespace com::github::doevelopper::atlassians::triada::test
{
    class RetryPolicyTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        RetryPolicyTest();
        RetryPolicyTest(const RetryPolicyTest&) = delete;
        RetryPolicyTest(RetryPolicyTest&&) = delete;
        auto operator=(const RetryPolicyTest&) -> RetryPolicyTest& = delete;
        auto operator=(RetryPolicyTest&&) -> RetryPolicyTest& = delete;
        virtual ~RetryPolicyTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
    private:
    };

    class CircuitBreakerTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        CircuitBreakerTest();
        CircuitBreakerTest(const CircuitBreakerTest&) = delete;
        CircuitBreakerTest(CircuitBreakerTest&&) = delete;
        auto operator=(const CircuitBreakerTest&) -> CircuitBreakerTest& = delete;
        auto operator=(CircuitBreakerTest&&) -> CircuitBreakerTest& = delete;
        virtual ~CircuitBreakerTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
    private:
    };

    class FallbackChainTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        FallbackChainTest();
        FallbackChainTest(const FallbackChainTest&) = delete;
        FallbackChainTest(FallbackChainTest&&) = delete;
        auto operator=(const FallbackChainTest&) -> FallbackChainTest& = delete;
        auto operator=(FallbackChainTest&&) -> FallbackChainTest& = delete;
        virtual ~FallbackChainTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
    private:
    };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_RECOVERYSTRATEGYTEST_HPP
