/*******************************************************************
 * @file    TestEventListener.hpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Custom Google Test Event Listener with comprehensive logging
 *
 * This class provides detailed logging of all test lifecycle events,
 * enabling better debugging and test execution monitoring.
 *
 * The listener inherits from EmptyTestEventListener to only override
 * the methods we care about, leaving others as no-ops.
 *
 * Design Pattern: Observer
 * - Observes test execution events
 * - Decoupled from test logic
 * - Easy to add/remove without modifying test code
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENER_HPP

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include <gtest/gtest.h>
#include <log4cxx/logger.h>

namespace com::github::doevelopper::atlassians::utdriver
{
    /**
     * @class TestEventListener
     * @brief Comprehensive test event listener with logging support
     *
     * Logs all test lifecycle events at appropriate log levels:
     * - INFO: Program start/end, suite start/end
     * - DEBUG: Test start/end, iterations
     * - TRACE: Detailed event information
     * - WARN: Disabled tests, skipped tests
     * - ERROR: Test failures
     *
     * Additional features:
     * - Test timing measurement
     * - Failure analysis logging
     * - Suite statistics tracking
     *
     * @example
     * // Registration (handled by TestDriver)
     * listeners.Append(new TestEventListener());
     */
    class TestEventListener : public ::testing::EmptyTestEventListener
    {
    public:
        // ============================================
        // Constructors & Destructor
        // ============================================

        /**
         * @brief Default constructor
         */
        TestEventListener() noexcept;

        /**
         * @brief Virtual destructor
         * @note Google Test owns this object
         */
        ~TestEventListener() noexcept override;

        // Non-copyable, non-movable (owned by Google Test)
        TestEventListener(const TestEventListener&) = delete;
        TestEventListener& operator=(const TestEventListener&) = delete;
        TestEventListener(TestEventListener&&) = delete;
        TestEventListener& operator=(TestEventListener&&) = delete;

        // ============================================
        // Test Program Lifecycle Events
        // ============================================

        /**
         * @brief Called before any tests run
         * @param unit_test The unit test object
         */
        void OnTestProgramStart(const ::testing::UnitTest& unit_test) override;

        /**
         * @brief Called after all tests complete
         * @param unit_test The unit test object
         */
        void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override;

        // ============================================
        // Test Iteration Events
        // ============================================

        /**
         * @brief Called at the start of each test iteration
         * @param unit_test The unit test object
         * @param iteration Current iteration number (0-indexed)
         */
        void OnTestIterationStart(const ::testing::UnitTest& unit_test,
                                  int iteration) override;

        /**
         * @brief Called at the end of each test iteration
         * @param unit_test The unit test object
         * @param iteration Current iteration number
         */
        void OnTestIterationEnd(const ::testing::UnitTest& unit_test,
                                int iteration) override;

        // ============================================
        // Environment Events
        // ============================================

        /**
         * @brief Called before global environment SetUp
         * @param unit_test The unit test object
         */
        void OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test) override;

        /**
         * @brief Called after global environment SetUp
         * @param unit_test The unit test object
         */
        void OnEnvironmentsSetUpEnd(const ::testing::UnitTest& unit_test) override;

        /**
         * @brief Called before global environment TearDown
         * @param unit_test The unit test object
         */
        void OnEnvironmentsTearDownStart(const ::testing::UnitTest& unit_test) override;

        /**
         * @brief Called after global environment TearDown
         * @param unit_test The unit test object
         */
        void OnEnvironmentsTearDownEnd(const ::testing::UnitTest& unit_test) override;

        // ============================================
        // Test Suite Events
        // ============================================

        /**
         * @brief Called before a test suite starts
         * @param test_suite The test suite object
         */
        void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override;

        /**
         * @brief Called after a test suite completes
         * @param test_suite The test suite object
         */
        void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override;

        // Legacy aliases (deprecated in newer Google Test)
        void OnTestCaseStart(const ::testing::TestCase& test_case) override;
        void OnTestCaseEnd(const ::testing::TestCase& test_case) override;

        // ============================================
        // Individual Test Events
        // ============================================

        /**
         * @brief Called before each test starts
         * @param test_info Information about the test
         */
        void OnTestStart(const ::testing::TestInfo& test_info) override;

        /**
         * @brief Called when a test is disabled
         * @param test_info Information about the disabled test
         */
        void OnTestDisabled(const ::testing::TestInfo& test_info) override;

        /**
         * @brief Called for each test assertion result
         * @param test_part_result The assertion result
         */
        void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;

        /**
         * @brief Called after each test completes
         * @param test_info Information about the completed test
         */
        void OnTestEnd(const ::testing::TestInfo& test_info) override;

    private:
        /**
         * @brief Format test name for logging
         * @param test_info Test information
         * @return Formatted test name string
         */
        [[nodiscard]] static auto formatTestName(
            const ::testing::TestInfo& test_info) -> std::string;

        /**
         * @brief Format test suite name for logging
         * @param test_suite Test suite information
         * @return Formatted suite name string
         */
        [[nodiscard]] static auto formatSuiteName(
            const ::testing::TestSuite& test_suite) -> std::string;

        /**
         * @brief Log failure details
         * @param result Test result containing failures
         */
        void logFailureDetails(const ::testing::TestResult* result);

        // Timing data
        using TimePoint = std::chrono::steady_clock::time_point;
        TimePoint m_programStartTime;
        std::unordered_map<std::string, TimePoint> m_testStartTimes;
        std::unordered_map<std::string, TimePoint> m_suiteStartTimes;

        // Statistics
        std::uint32_t m_totalAssertions{0};
        std::uint32_t m_failedAssertions{0};

        // Logger
        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::atlassians::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENER_HPP
