/**
 * @file ErrorTest.hpp
 * @brief Unit tests for Error and Result classes
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORTEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/triada/Error.hpp>

namespace com::github::doevelopper::atlassians::triada::test
{
    class ErrorTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ErrorTest();
        ErrorTest(const ErrorTest&) = delete;
        ErrorTest(ErrorTest&&) = delete;
        auto operator=(const ErrorTest&) -> ErrorTest& = delete;
        auto operator=(ErrorTest&&) -> ErrorTest& = delete;
        virtual ~ErrorTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
        auto createTestError() -> Error;
        auto createTestErrorWithChain() -> Error;

    private:
    };

    class ResultTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ResultTest();
        ResultTest(const ResultTest&) = delete;
        ResultTest(ResultTest&&) = delete;
        auto operator=(const ResultTest&) -> ResultTest& = delete;
        auto operator=(ResultTest&&) -> ResultTest& = delete;
        virtual ~ResultTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
    private:
    };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORTEST_HPP
