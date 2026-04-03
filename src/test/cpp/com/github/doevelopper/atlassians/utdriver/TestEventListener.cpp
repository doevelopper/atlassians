/*******************************************************************
 * @file    TestEventListener.cpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Custom Google Test Event Listener implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/utdriver/TestEventListener.hpp>

#include <iomanip>
#include <sstream>

using namespace com::github::doevelopper::atlassians::utdriver;

// Initialize static logger
log4cxx::LoggerPtr TestEventListener::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.TestEventListener");

// ============================================
// Constructors & Destructor
// ============================================

TestEventListener::TestEventListener() noexcept
    : m_totalAssertions{0}
    , m_failedAssertions{0}
{
    LOG4CXX_TRACE(logger, "TestEventListener constructed");
}

TestEventListener::~TestEventListener() noexcept
{
    LOG4CXX_TRACE(logger, "TestEventListener destructor");
}

// ============================================
// Test Program Lifecycle Events
// ============================================

void TestEventListener::OnTestProgramStart(const ::testing::UnitTest& unit_test)
{
    m_programStartTime = std::chrono::steady_clock::now();
    m_totalAssertions = 0;
    m_failedAssertions = 0;

    LOG4CXX_INFO(logger, "");
    LOG4CXX_INFO(logger, "╔══════════════════════════════════════════════════════════════╗");
    LOG4CXX_INFO(logger, "║              GOOGLE TEST PROGRAM STARTED                     ║");
    LOG4CXX_INFO(logger, "╠══════════════════════════════════════════════════════════════╣");
    LOG4CXX_INFO(logger, "║  Test Suites:     " << std::setw(6) << unit_test.total_test_suite_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Total Tests:     " << std::setw(6) << unit_test.total_test_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Tests to Run:    " << std::setw(6) << unit_test.test_to_run_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Disabled Tests:  " << std::setw(6) << unit_test.disabled_test_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "╚══════════════════════════════════════════════════════════════╝");
}

void TestEventListener::OnTestProgramEnd(const ::testing::UnitTest& unit_test)
{
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTime - m_programStartTime);

    LOG4CXX_INFO(logger, "");
    LOG4CXX_INFO(logger, "╔══════════════════════════════════════════════════════════════╗");
    LOG4CXX_INFO(logger, "║              GOOGLE TEST PROGRAM FINISHED                    ║");
    LOG4CXX_INFO(logger, "╠══════════════════════════════════════════════════════════════╣");
    LOG4CXX_INFO(logger, "║  Tests Run:       " << std::setw(6) << unit_test.test_to_run_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Passed:          " << std::setw(6) << unit_test.successful_test_count()
                 << "  ✓                                   ║");

    if (unit_test.failed_test_count() > 0)
    {
        LOG4CXX_ERROR(logger, "║  Failed:          " << std::setw(6) << unit_test.failed_test_count()
                     << "  ✗                                   ║");
    }
    else
    {
        LOG4CXX_INFO(logger, "║  Failed:          " << std::setw(6) << unit_test.failed_test_count()
                     << "                                      ║");
    }

    LOG4CXX_INFO(logger, "║  Skipped:         " << std::setw(6) << unit_test.skipped_test_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Disabled:        " << std::setw(6) << unit_test.disabled_test_count()
                 << "                                      ║");
    LOG4CXX_INFO(logger, "║  Total Duration:  " << std::setw(6) << duration.count()
                 << " ms                                  ║");
    LOG4CXX_INFO(logger, "║  Assertions:      " << std::setw(6) << m_totalAssertions
                 << " (" << m_failedAssertions << " failed)                      ║");
    LOG4CXX_INFO(logger, "╚══════════════════════════════════════════════════════════════╝");

    if (unit_test.Passed())
    {
        LOG4CXX_INFO(logger, "[  PASSED  ] All tests passed successfully!");
    }
    else
    {
        LOG4CXX_ERROR(logger, "[  FAILED  ] Some tests failed. See details above.");
    }
}

// ============================================
// Test Iteration Events
// ============================================

void TestEventListener::OnTestIterationStart(const ::testing::UnitTest& unit_test,
                                             int iteration)
{
    LOG4CXX_DEBUG(logger, "");
    LOG4CXX_DEBUG(logger, "────────────────────────────────────────────────────────────────");
    LOG4CXX_DEBUG(logger, "  Test Iteration " << (iteration + 1) << " of "
                  << ::testing::GTEST_FLAG(repeat) << " started");
    LOG4CXX_DEBUG(logger, "  Running " << unit_test.test_to_run_count() << " test(s)");
    LOG4CXX_DEBUG(logger, "────────────────────────────────────────────────────────────────");
}

void TestEventListener::OnTestIterationEnd(const ::testing::UnitTest& unit_test,
                                           int iteration)
{
    LOG4CXX_DEBUG(logger, "────────────────────────────────────────────────────────────────");
    LOG4CXX_DEBUG(logger, "  Test Iteration " << (iteration + 1) << " completed");
    LOG4CXX_DEBUG(logger, "  Passed: " << unit_test.successful_test_count()
                  << ", Failed: " << unit_test.failed_test_count()
                  << ", Skipped: " << unit_test.skipped_test_count());
    LOG4CXX_DEBUG(logger, "────────────────────────────────────────────────────────────────");
}

// ============================================
// Environment Events
// ============================================

void TestEventListener::OnEnvironmentsSetUpStart(const ::testing::UnitTest& /*unit_test*/)
{
    LOG4CXX_DEBUG(logger, "[----------] Global test environment set-up starting");
}

void TestEventListener::OnEnvironmentsSetUpEnd(const ::testing::UnitTest& /*unit_test*/)
{
    LOG4CXX_DEBUG(logger, "[----------] Global test environment set-up completed");
}

void TestEventListener::OnEnvironmentsTearDownStart(const ::testing::UnitTest& /*unit_test*/)
{
    LOG4CXX_DEBUG(logger, "[----------] Global test environment tear-down starting");
}

void TestEventListener::OnEnvironmentsTearDownEnd(const ::testing::UnitTest& /*unit_test*/)
{
    LOG4CXX_DEBUG(logger, "[----------] Global test environment tear-down completed");
}

// ============================================
// Test Suite Events
// ============================================

void TestEventListener::OnTestSuiteStart(const ::testing::TestSuite& test_suite)
{
    const std::string suiteName = formatSuiteName(test_suite);
    m_suiteStartTimes[suiteName] = std::chrono::steady_clock::now();

    LOG4CXX_INFO(logger, "");
    LOG4CXX_INFO(logger, "[==========] " << test_suite.test_to_run_count()
                 << " test(s) from " << suiteName);
}

void TestEventListener::OnTestSuiteEnd(const ::testing::TestSuite& test_suite)
{
    const std::string suiteName = formatSuiteName(test_suite);

    auto endTime = std::chrono::steady_clock::now();
    auto startTimeIt = m_suiteStartTimes.find(suiteName);

    std::string durationStr = "?";
    if (startTimeIt != m_suiteStartTimes.end())
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTimeIt->second);
        durationStr = std::to_string(duration.count()) + " ms";
        m_suiteStartTimes.erase(startTimeIt);
    }

    LOG4CXX_INFO(logger, "[==========] " << test_suite.test_to_run_count()
                 << " test(s) from " << suiteName << " completed (" << durationStr << ")");

    if (test_suite.failed_test_count() > 0)
    {
        LOG4CXX_WARN(logger, "[  FAILED  ] " << test_suite.failed_test_count()
                     << " test(s) failed in " << suiteName);
    }
}

void TestEventListener::OnTestCaseStart(const ::testing::TestCase& test_case)
{
    // Delegate to OnTestSuiteStart for backward compatibility
    LOG4CXX_TRACE(logger, "[DEPRECATED] OnTestCaseStart called for: " << test_case.name());
}

void TestEventListener::OnTestCaseEnd(const ::testing::TestCase& test_case)
{
    // Delegate to OnTestSuiteEnd for backward compatibility
    LOG4CXX_TRACE(logger, "[DEPRECATED] OnTestCaseEnd called for: " << test_case.name());
}

// ============================================
// Individual Test Events
// ============================================

void TestEventListener::OnTestStart(const ::testing::TestInfo& test_info)
{
    const std::string testName = formatTestName(test_info);
    m_testStartTimes[testName] = std::chrono::steady_clock::now();

    LOG4CXX_DEBUG(logger, "[ RUN      ] " << testName);
}

void TestEventListener::OnTestDisabled(const ::testing::TestInfo& test_info)
{
    LOG4CXX_WARN(logger, "[ DISABLED ] " << formatTestName(test_info));
}

void TestEventListener::OnTestPartResult(const ::testing::TestPartResult& test_part_result)
{
    m_totalAssertions++;

    if (test_part_result.failed())
    {
        m_failedAssertions++;

        std::ostringstream msg;
        msg << "ASSERTION FAILED: "
            << (test_part_result.file_name() ? test_part_result.file_name() : "<unknown file>")
            << ":" << test_part_result.line_number();

        LOG4CXX_ERROR(logger, msg.str());

        if (test_part_result.summary())
        {
            LOG4CXX_ERROR(logger, "  " << test_part_result.summary());
        }

        if (test_part_result.message())
        {
            LOG4CXX_ERROR(logger, "  Details: " << test_part_result.message());
        }
    }
    else if (test_part_result.skipped())
    {
        LOG4CXX_DEBUG(logger, "[ SKIPPED  ] " << test_part_result.message());
    }
    else
    {
        LOG4CXX_TRACE(logger, "[ OK       ] Assertion passed");
    }
}

void TestEventListener::OnTestEnd(const ::testing::TestInfo& test_info)
{
    const std::string testName = formatTestName(test_info);

    auto endTime = std::chrono::steady_clock::now();
    auto startTimeIt = m_testStartTimes.find(testName);

    std::string durationStr = "?";
    if (startTimeIt != m_testStartTimes.end())
    {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTimeIt->second);
        durationStr = std::to_string(duration.count()) + " ms";
        m_testStartTimes.erase(startTimeIt);
    }

    if (test_info.result()->Passed())
    {
        LOG4CXX_DEBUG(logger, "[       OK ] " << testName << " (" << durationStr << ")");
    }
    else if (test_info.result()->Skipped())
    {
        LOG4CXX_INFO(logger, "[  SKIPPED ] " << testName << " (" << durationStr << ")");
    }
    else
    {
        LOG4CXX_ERROR(logger, "[  FAILED  ] " << testName << " (" << durationStr << ")");
        logFailureDetails(test_info.result());
    }
}

// ============================================
// Private Methods
// ============================================

auto TestEventListener::formatTestName(const ::testing::TestInfo& test_info) -> std::string
{
    std::ostringstream oss;
    oss << test_info.test_suite_name() << "." << test_info.name();

    // Add type parameter if present (for typed tests)
    if (test_info.type_param())
    {
        oss << "<" << test_info.type_param() << ">";
    }

    // Add value parameter if present (for parameterized tests)
    if (test_info.value_param())
    {
        oss << "/" << test_info.value_param();
    }

    return oss.str();
}

auto TestEventListener::formatSuiteName(const ::testing::TestSuite& test_suite) -> std::string
{
    std::ostringstream oss;
    oss << test_suite.name();

    // Add type parameter if present (for typed tests)
    if (test_suite.type_param())
    {
        oss << "<" << test_suite.type_param() << ">";
    }

    return oss.str();
}

void TestEventListener::logFailureDetails(const ::testing::TestResult* result)
{
    if (!result)
    {
        return;
    }

    for (int i = 0; i < result->total_part_count(); ++i)
    {
        const auto& part = result->GetTestPartResult(i);
        if (part.failed())
        {
            LOG4CXX_ERROR(logger, "  Failure #" << (i + 1) << ":");
            LOG4CXX_ERROR(logger, "    Location: "
                         << (part.file_name() ? part.file_name() : "<unknown>")
                         << ":" << part.line_number());
            if (part.summary())
            {
                LOG4CXX_ERROR(logger, "    Summary: " << part.summary());
            }
        }
    }
}
