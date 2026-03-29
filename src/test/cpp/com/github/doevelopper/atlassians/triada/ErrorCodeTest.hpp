/**
 * @file ErrorCodeTest.hpp
 * @brief Unit tests for ErrorCode class
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODETEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODETEST_HPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/premisses/triada/ErrorCode.hpp>

namespace com::github::doevelopper::premisses::triada::test
{
    class ErrorCodeTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ErrorCodeTest();
        ErrorCodeTest(const ErrorCodeTest&) = delete;
        ErrorCodeTest(ErrorCodeTest&&) = delete;
        auto operator=(const ErrorCodeTest&) -> ErrorCodeTest& = delete;
        auto operator=(ErrorCodeTest&&) -> ErrorCodeTest& = delete;
        virtual ~ErrorCodeTest();

        auto SetUp() -> void override;
        auto TearDown() -> void override;

    protected:
        // Test helper methods
        auto verifyErrorCodeFields(
            const ErrorCode& code,
            ErrorSeverity expectedSeverity,
            std::uint8_t expectedReserved,
            ServiceId expectedService,
            std::uint16_t expectedMission,
            std::uint64_t expectedCodeValue) -> void;

    private:
    };

    // Parameterized test fixture for severity level tests
    class ErrorSeverityTest : public ::testing::TestWithParam<std::pair<ErrorSeverity, std::string_view>>
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ErrorSeverityTest() = default;
        virtual ~ErrorSeverityTest() = default;
    };

    // Parameterized test fixture for service ID tests
    class ServiceIdTest : public ::testing::TestWithParam<std::pair<ServiceId, std::string_view>>
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:
        ServiceIdTest() = default;
        virtual ~ServiceIdTest() = default;
    };
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_TRIADA_ERRORCODETEST_HPP
