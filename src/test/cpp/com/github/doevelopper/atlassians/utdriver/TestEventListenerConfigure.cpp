/*******************************************************************
 * @file    TestEventListenerConfigure.cpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Configurable Google Test Event Listener implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/utdriver/TestEventListenerConfigure.hpp>

using namespace com::github::doevelopper::atlassians::utdriver;

// Initialize static loggers
log4cxx::LoggerPtr TestEventListenerConfigure::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.TestEventListenerConfigure");

log4cxx::LoggerPtr TestEventListenerConfigure::Delegate::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.TestEventListenerConfigure.Delegate");

// ============================================
// Delegate Builder Implementation
// ============================================

TestEventListenerConfigure::Delegate::Delegate(::testing::TestEventListener* eventListener)
    : m_delegateEventListener{eventListener}
    , m_options{}
{
    LOG4CXX_TRACE(logger, "Delegate builder created with event listener");
}

TestEventListenerConfigure::Delegate::Delegate(::testing::TestEventListener* eventListener,
                                               const Options& preset)
    : m_delegateEventListener{eventListener}
    , m_options{preset}
{
    LOG4CXX_TRACE(logger, "Delegate builder created with preset options");
}

TestEventListenerConfigure::Delegate::~Delegate() noexcept
{
    LOG4CXX_TRACE(logger, "Delegate builder destructor");
}

auto TestEventListenerConfigure::Delegate::showProgramStartEnd() noexcept -> Delegate&
{
    m_options.programStartEnd = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showIterationsStartEnd() noexcept -> Delegate&
{
    m_options.iterationsStartEnd = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showTestCases() noexcept -> Delegate&
{
    m_options.testCases = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showTestNames() noexcept -> Delegate&
{
    m_options.testNames = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showTestSuite() noexcept -> Delegate&
{
    m_options.testSuite = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showSuccesses() noexcept -> Delegate&
{
    m_options.successes = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showFailures() noexcept -> Delegate&
{
    m_options.failures = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showInlineFailures() noexcept -> Delegate&
{
    m_options.inlineFailures = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showEnvironment() noexcept -> Delegate&
{
    m_options.environment = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showSkip() noexcept -> Delegate&
{
    m_options.skip = true;
    return *this;
}

auto TestEventListenerConfigure::Delegate::withPreset(const Options& preset) noexcept -> Delegate&
{
    m_options = preset;
    return *this;
}

auto TestEventListenerConfigure::Delegate::showAll() noexcept -> Delegate&
{
    m_options = Options::all();
    return *this;
}

auto TestEventListenerConfigure::Delegate::build() -> TestEventListenerConfigure*
{
    LOG4CXX_DEBUG(logger, "Building TestEventListenerConfigure");
    return new TestEventListenerConfigure(m_delegateEventListener, m_options);
}

auto TestEventListenerConfigure::Delegate::buildUnique() -> std::unique_ptr<TestEventListenerConfigure>
{
    LOG4CXX_DEBUG(logger, "Building TestEventListenerConfigure (unique_ptr)");
    return std::unique_ptr<TestEventListenerConfigure>(
        new TestEventListenerConfigure(m_delegateEventListener, m_options));
}

// ============================================
// TestEventListenerConfigure Implementation
// ============================================

TestEventListenerConfigure::TestEventListenerConfigure(
    ::testing::TestEventListener* eventListener,
    const Options& options)
    : m_eventListener{eventListener}
    , m_options{options}
{
    LOG4CXX_TRACE(logger, "TestEventListenerConfigure constructed");
    LOG4CXX_DEBUG(logger, "Options: "
                  << "programStartEnd=" << m_options.programStartEnd
                  << ", testSuite=" << m_options.testSuite
                  << ", testNames=" << m_options.testNames
                  << ", failures=" << m_options.failures);
}

TestEventListenerConfigure::~TestEventListenerConfigure() noexcept
{
    LOG4CXX_TRACE(logger, "TestEventListenerConfigure destructor");
}

auto TestEventListenerConfigure::getOptions() const noexcept -> const Options&
{
    return m_options;
}

// ============================================
// Event Handler Implementations
// ============================================

void TestEventListenerConfigure::OnTestProgramStart(const ::testing::UnitTest& unit_test)
{
    if (m_options.programStartEnd && m_eventListener)
    {
        m_eventListener->OnTestProgramStart(unit_test);
    }
}

void TestEventListenerConfigure::OnTestIterationStart(const ::testing::UnitTest& unit_test,
                                                       int iteration)
{
    if (m_options.iterationsStartEnd && m_eventListener)
    {
        m_eventListener->OnTestIterationStart(unit_test, iteration);
    }
}

void TestEventListenerConfigure::OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test)
{
    if (m_options.environment && m_eventListener)
    {
        m_eventListener->OnEnvironmentsSetUpStart(unit_test);
    }
}

void TestEventListenerConfigure::OnEnvironmentsSetUpEnd(const ::testing::UnitTest& unit_test)
{
    if (m_options.environment && m_eventListener)
    {
        m_eventListener->OnEnvironmentsSetUpEnd(unit_test);
    }
}

void TestEventListenerConfigure::OnTestSuiteStart(const ::testing::TestSuite& test_suite)
{
    if (m_options.testSuite && m_eventListener)
    {
        m_eventListener->OnTestSuiteStart(test_suite);
    }
}

void TestEventListenerConfigure::OnTestCaseStart(const ::testing::TestCase& test_case)
{
    if (m_options.testCases && m_eventListener)
    {
        m_eventListener->OnTestCaseStart(test_case);
    }
}

void TestEventListenerConfigure::OnTestStart(const ::testing::TestInfo& test_info)
{
    if (m_options.testNames && m_eventListener)
    {
        m_eventListener->OnTestStart(test_info);
    }
}

void TestEventListenerConfigure::OnTestDisabled(const ::testing::TestInfo& test_info)
{
    if (m_options.skip && m_eventListener)
    {
        m_eventListener->OnTestDisabled(test_info);
    }
}

void TestEventListenerConfigure::OnTestPartResult(
    const ::testing::TestPartResult& test_part_result)
{
    if (!m_eventListener)
    {
        return;
    }

    // Show success assertions if enabled
    if (m_options.successes && test_part_result.passed())
    {
        m_eventListener->OnTestPartResult(test_part_result);
        return;
    }

    // Show failure assertions if enabled
    if (m_options.failures && test_part_result.failed())
    {
        m_eventListener->OnTestPartResult(test_part_result);
        return;
    }

    // Show skipped if enabled
    if (m_options.skip && test_part_result.skipped())
    {
        m_eventListener->OnTestPartResult(test_part_result);
        return;
    }
}

void TestEventListenerConfigure::OnTestEnd(const ::testing::TestInfo& test_info)
{
    if (!m_eventListener)
    {
        return;
    }

    const auto* result = test_info.result();

    // Show inline failures
    if (m_options.inlineFailures && result->Failed())
    {
        m_eventListener->OnTestEnd(test_info);
        return;
    }

    // Show successes
    if (m_options.successes && result->Passed())
    {
        m_eventListener->OnTestEnd(test_info);
        return;
    }

    // Show skipped
    if (m_options.skip && result->Skipped())
    {
        m_eventListener->OnTestEnd(test_info);
        return;
    }

    // Show test names (without filtering by result)
    if (m_options.testNames)
    {
        m_eventListener->OnTestEnd(test_info);
    }
}

void TestEventListenerConfigure::OnTestSuiteEnd(const ::testing::TestSuite& test_suite)
{
    if (m_options.testSuite && m_eventListener)
    {
        m_eventListener->OnTestSuiteEnd(test_suite);
    }
}

void TestEventListenerConfigure::OnTestCaseEnd(const ::testing::TestCase& test_case)
{
    if (m_options.testCases && m_eventListener)
    {
        m_eventListener->OnTestCaseEnd(test_case);
    }
}

void TestEventListenerConfigure::OnEnvironmentsTearDownStart(
    const ::testing::UnitTest& unit_test)
{
    if (m_options.environment && m_eventListener)
    {
        m_eventListener->OnEnvironmentsTearDownStart(unit_test);
    }
}

void TestEventListenerConfigure::OnEnvironmentsTearDownEnd(const ::testing::UnitTest& unit_test)
{
    if (m_options.environment && m_eventListener)
    {
        m_eventListener->OnEnvironmentsTearDownEnd(unit_test);
    }
}

void TestEventListenerConfigure::OnTestIterationEnd(const ::testing::UnitTest& unit_test,
                                                     int iteration)
{
    if (m_options.iterationsStartEnd && m_eventListener)
    {
        m_eventListener->OnTestIterationEnd(unit_test, iteration);
    }
}

void TestEventListenerConfigure::OnTestProgramEnd(const ::testing::UnitTest& unit_test)
{
    if (m_options.programStartEnd && m_eventListener)
    {
        m_eventListener->OnTestProgramEnd(unit_test);
    }
}
