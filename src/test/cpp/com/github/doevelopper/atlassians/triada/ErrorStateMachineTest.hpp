/**
 * @file ErrorStateMachineTest.hpp
 * @brief Unit tests for error state machine
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINETEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINETEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/triada/ErrorStateMachine.hpp>

namespace com::github::doevelopper::atlassians::triada::test
{
    class ErrorStateMachineTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ErrorStateMachineTest();
        ErrorStateMachineTest(const ErrorStateMachineTest&) = delete;
        ErrorStateMachineTest(ErrorStateMachineTest&&) = delete;
        auto operator=(const ErrorStateMachineTest&) -> ErrorStateMachineTest& = delete;
        auto operator=(ErrorStateMachineTest&&) -> ErrorStateMachineTest& = delete;
        virtual ~ErrorStateMachineTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
        auto createTestError(ErrorSeverity severity = ErrorSeverity::Error) -> Error;
        
        std::unique_ptr<ErrorStateManager> m_stateManager;
        std::vector<std::pair<std::string, std::string>> m_stateTransitions;
        std::vector<Error> m_reportedErrors;

    private:
    };

    class ErrorStateManagerTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ErrorStateManagerTest();
        ErrorStateManagerTest(const ErrorStateManagerTest&) = delete;
        ErrorStateManagerTest(ErrorStateManagerTest&&) = delete;
        auto operator=(const ErrorStateManagerTest&) -> ErrorStateManagerTest& = delete;
        auto operator=(ErrorStateManagerTest&&) -> ErrorStateManagerTest& = delete;
        virtual ~ErrorStateManagerTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
        std::unique_ptr<ErrorStateManager> m_manager;

    private:
    };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORSTATEMACHINETEST_HPP
