/*******************************************************************
 * @file    TestDriver.cpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Google Test Driver implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>
#include <span>

#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/utdriver/TestDriver.hpp>
#include <com/github/doevelopper/atlassians/utdriver/TestEventListener.hpp>
#include <com/github/doevelopper/atlassians/utdriver/TestEventListenerConfigure.hpp>
#include <com/github/doevelopper/atlassians/utdriver/CustomTestEnvironment.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::atlassians::utdriver;
using namespace com::github::doevelopper::atlassians::logging;

// Initialize static logger
log4cxx::LoggerPtr TestDriver::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.TestDriver");

// ============================================
// TestDriver Implementation
// ============================================

TestDriver::TestDriver() noexcept
    : m_config{}
    , m_moduleUnderTest{"PremissesTestSuite"}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    static_cast<void>(initializeLogging());
    LOG4CXX_TRACE(logger, "TestDriver default constructor called");
}

TestDriver::TestDriver(const TestDriverConfig& config) noexcept
    : m_config{config}
    , m_moduleUnderTest{"PremissesTestSuite"}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    static_cast<void>(initializeLogging());
    LOG4CXX_TRACE(logger, "TestDriver constructed with custom configuration");
}

TestDriver::TestDriver(std::string_view suiteName, std::uint32_t iterationCount) noexcept
    : m_config{}
    , m_moduleUnderTest{suiteName}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    m_config.repeatCount = iterationCount;
    static_cast<void>(initializeLogging());
    LOG4CXX_DEBUG(logger, "TestDriver constructed for suite: " << suiteName
                           << " with " << iterationCount << " iteration(s)");
}

TestDriver::~TestDriver() noexcept
{
    LOG4CXX_TRACE(logger, "TestDriver destructor called");
    if (m_isInitialized)
    {
        static_cast<void>(teardown());
    }
}

TestDriver::TestDriver(TestDriver&& other) noexcept
    : m_config{std::move(other.m_config)}
    , m_moduleUnderTest{std::move(other.m_moduleUnderTest)}
    , m_isInitialized{other.m_isInitialized}
    , m_lastResult{other.m_lastResult}
{
    other.m_isInitialized = false;
    LOG4CXX_TRACE(logger, "TestDriver move constructor called");
}

TestDriver& TestDriver::operator=(TestDriver&& other) noexcept
{
    if (this != &other)
    {
        m_config = std::move(other.m_config);
        m_moduleUnderTest = std::move(other.m_moduleUnderTest);
        m_isInitialized = other.m_isInitialized;
        m_lastResult = other.m_lastResult;
        other.m_isInitialized = false;
    }
    LOG4CXX_TRACE(logger, "TestDriver move assignment called");
    return *this;
}

auto TestDriver::initializeLogging() -> bool
{
    try
    {
        auto& initializer = LoggingInitializer::getInstance();
        if (!initializer.isInitialized())
        {
            auto strategy = std::make_unique<DefaultInitializationStrategy>();
            return initializer.initialize(std::move(strategy));
        }
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
        return false;
    }
}

auto TestDriver::setup(int argc, char* argv[]) -> bool
{
    LOG4CXX_INFO(logger, "TestDriver::setup() - Initializing test framework");

    if (m_isInitialized)
    {
        LOG4CXX_WARN(logger, "TestDriver already initialized, skipping setup");
        return true;
    }

    try
    {
        // Extract module name from executable path
        extractModuleName(argv);
        LOG4CXX_DEBUG(logger, "Module under test: " << m_moduleUnderTest);

        // Initialize Google Mock (includes Google Test)
        ::testing::InitGoogleMock(&argc, argv);
        LOG4CXX_DEBUG(logger, "GoogleMock initialized");

        // Apply configuration flags
        applyConfiguration();

        // Register custom test environment
        registerTestEnvironment();

        // Register custom event listeners
        registerEventListeners();

        // Call hook for subclass customization
        onEnvironmentSetup();

        m_isInitialized = true;
        LOG4CXX_INFO(logger, "TestDriver setup completed successfully");
        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "TestDriver setup failed: " << e.what());
        m_lastResult = ExecutionResult::InitializationError;
        return false;
    }
}

void TestDriver::applyConfiguration()
{
    LOG4CXX_TRACE(logger, "Applying test configuration");

    auto& flags = ::testing::GTEST_FLAG(shuffle);
    flags = m_config.shuffleTests;

    ::testing::GTEST_FLAG(break_on_failure) = m_config.breakOnFailure;
    ::testing::GTEST_FLAG(color) = m_config.colorOutput ? "yes" : "no";
    ::testing::GTEST_FLAG(print_time) = m_config.printTimePerTest;
    ::testing::GTEST_FLAG(repeat) = static_cast<int>(m_config.repeatCount);
    ::testing::GTEST_FLAG(throw_on_failure) = m_config.throwOnFailure;

    if (m_config.filter.has_value())
    {
        ::testing::GTEST_FLAG(filter) = m_config.filter.value();
        LOG4CXX_DEBUG(logger, "Test filter applied: " << m_config.filter.value());
    }

    if (m_config.outputXmlPath.has_value())
    {
        ::testing::GTEST_FLAG(output) = "xml:" + m_config.outputXmlPath.value();
        LOG4CXX_DEBUG(logger, "XML output path: " << m_config.outputXmlPath.value());
    }
}

void TestDriver::registerTestEnvironment()
{
    LOG4CXX_TRACE(logger, "Registering custom test environment");

    // Google Test takes ownership of the environment pointer
    static ::testing::Environment* const testEnv [[maybe_unused]] =
        ::testing::AddGlobalTestEnvironment(new CustomTestEnvironment());

    LOG4CXX_DEBUG(logger, "CustomTestEnvironment registered");
}

void TestDriver::registerEventListeners()
{
    LOG4CXX_TRACE(logger, "Registering custom event listeners");

    ::testing::TestEventListeners& listeners =
        ::testing::UnitTest::GetInstance()->listeners();

    // Get and configure the default printer
    auto* defaultPrinter = listeners.Release(listeners.default_result_printer());

    // Delete any duplicate default printers (sometimes created by GMock)
    if (auto* duplicate = listeners.Release(listeners.default_result_printer()))
    {
        delete duplicate;
    }

    // Build configurable listener that wraps the default printer
    auto* configurableListener = TestEventListenerConfigure::Delegate(defaultPrinter)
        .showProgramStartEnd()
        .showIterationsStartEnd()
        .showTestSuite()
        .showTestNames()
        .showSuccesses()
        .showFailures()
        .showInlineFailures()
        .showEnvironment()
        .build();

    listeners.Append(configurableListener);
    LOG4CXX_DEBUG(logger, "TestEventListenerConfigure registered");

    // Add our custom event listener for logging
    listeners.Append(new TestEventListener());
    LOG4CXX_DEBUG(logger, "TestEventListener registered");
}

auto TestDriver::run(int argc, char* argv[]) -> int
{
    LOG4CXX_INFO(logger, "TestDriver::run() - Executing tests");

    if (!m_isInitialized)
    {
        LOG4CXX_WARN(logger, "TestDriver not initialized, calling setup first");
        if (argc > 0 && argv != nullptr)
        {
            if (!setup(argc, argv))
            {
                LOG4CXX_ERROR(logger, "Failed to initialize TestDriver");
                return static_cast<int>(ExecutionResult::InitializationError);
            }
        }
        else
        {
            LOG4CXX_ERROR(logger, "Cannot auto-initialize without command line args");
            return static_cast<int>(ExecutionResult::ConfigurationError);
        }
    }

    LOG4CXX_INFO(logger, "Running all tests...");
    const int result = RUN_ALL_TESTS();

    m_lastResult = (result == 0) ? ExecutionResult::Success : ExecutionResult::TestFailure;

    LOG4CXX_INFO(logger, "Test execution completed with result: " << result);
    LOG4CXX_INFO(logger, getTestSummary());

    return result;
}

auto TestDriver::execute(int argc, char* argv[]) -> int
{
    LOG4CXX_TRACE(logger, "Combined setup and run");

    if (!setup(argc, argv))
    {
        return static_cast<int>(ExecutionResult::InitializationError);
    }

    return run(argc, argv);
}

auto TestDriver::teardown() noexcept -> bool
{
    LOG4CXX_INFO(logger, "Cleaning up resources");

    if (!m_isInitialized)
    {
        LOG4CXX_DEBUG(logger, "TestDriver not initialized, nothing to teardown");
        return true;
    }

    try
    {
        // Call hook for subclass cleanup
        onEnvironmentTeardown();

        m_isInitialized = false;
        LOG4CXX_INFO(logger, "TestDriver teardown completed");
        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "TestDriver teardown failed: " << e.what());
        return false;
    }
}

void TestDriver::extractModuleName(char* argv[])
{
    if (argv == nullptr || argv[0] == nullptr)
    {
        m_moduleUnderTest = "UnknownModule";
        return;
    }

    try
    {
        // Use std::filesystem for portable path handling
        std::filesystem::path execPath(argv[0]);
        std::string filename = execPath.filename().string();

        // Convert to uppercase for consistency
        std::transform(filename.begin(), filename.end(), filename.begin(),
                       [](unsigned char c) { return std::toupper(c); });

        m_moduleUnderTest = filename;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_WARN(logger, "Failed to extract module name: " << e.what());
        m_moduleUnderTest = "UnknownModule";
    }
}

auto TestDriver::getModuleUnderTest() const noexcept -> std::string_view
{
    return m_moduleUnderTest;
}

auto TestDriver::isInitialized() const noexcept -> bool
{
    return m_isInitialized;
}

auto TestDriver::getExecutionResult() const noexcept -> ExecutionResult
{
    return m_lastResult;
}

auto TestDriver::getConfig() const noexcept -> const TestDriverConfig&
{
    return m_config;
}

void TestDriver::notYetImplemented()
{
    GTEST_SKIP() << "Test not yet implemented";
}

auto TestDriver::getTestSummary() -> std::string
{
    const auto* unitTest = ::testing::UnitTest::GetInstance();
    std::ostringstream summary;

    summary << "\n========================================"
            << "\n  TEST SUMMARY"
            << "\n========================================";
    summary << "\n  Total test suites: " << unitTest->total_test_suite_count();
    summary << "\n  Total tests:       " << unitTest->total_test_count();
    summary << "\n  Tests run:         " << unitTest->test_to_run_count();
    summary << "\n  Passed:            " << unitTest->successful_test_count();
    summary << "\n  Failed:            " << unitTest->failed_test_count();
    summary << "\n  Skipped:           " << unitTest->skipped_test_count();
    summary << "\n  Disabled:          " << unitTest->disabled_test_count();
    summary << "\n  Elapsed time:      " << unitTest->elapsed_time() << " ms";
    summary << "\n========================================\n";

    return summary.str();
}

void TestDriver::onEnvironmentSetup()
{
    LOG4CXX_TRACE(logger, "TestDriver::onEnvironmentSetup() - hook called");
    // Default implementation does nothing
    // Subclasses can override for custom setup
}

void TestDriver::onEnvironmentTeardown()
{
    LOG4CXX_TRACE(logger, "TestDriver::onEnvironmentTeardown() - hook called");
    // Default implementation does nothing
    // Subclasses can override for custom teardown
}
