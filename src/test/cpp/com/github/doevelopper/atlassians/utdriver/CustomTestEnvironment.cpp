/*******************************************************************
 * @file    CustomTestEnvironment.cpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Custom Google Test Environment implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/utdriver/CustomTestEnvironment.hpp>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <random>
#include <sstream>

using namespace com::github::doevelopper::atlassians::utdriver;

// Initialize static logger
log4cxx::LoggerPtr CustomTestEnvironment::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.CustomTestEnvironment");

// ============================================
// Constructors & Destructor
// ============================================

CustomTestEnvironment::CustomTestEnvironment() noexcept
    : m_config{}
    , m_isSetUp{false}
    , m_testPort{0}
{
    LOG4CXX_TRACE(logger, "CustomTestEnvironment default constructor");
}

CustomTestEnvironment::CustomTestEnvironment(const Config& config) noexcept
    : m_config{config}
    , m_isSetUp{false}
    , m_testPort{0}
{
    LOG4CXX_TRACE(logger, "CustomTestEnvironment constructed with custom config");
}

CustomTestEnvironment::~CustomTestEnvironment() noexcept
{
    LOG4CXX_TRACE(logger, "CustomTestEnvironment destructor");
    // Note: Google Test owns this object - destructor called automatically
    // Do not perform cleanup here - use TearDown() instead
}

// ============================================
// Google Test Environment Interface
// ============================================

void CustomTestEnvironment::SetUp()
{
    LOG4CXX_INFO(logger, "========================================");
    LOG4CXX_INFO(logger, "  GLOBAL TEST ENVIRONMENT SETUP");
    LOG4CXX_INFO(logger, "========================================");

    try
    {
        // Generate random port for network tests
        m_testPort = generateRandomPort();
        LOG4CXX_DEBUG(logger, "Generated test port: " << m_testPort);

        // Configure environment variables
        configureEnvironment();

        // Create temporary directory if needed
        if (m_config.tempDirectory.has_value())
        {
            m_tempDirectory = m_config.tempDirectory.value();
        }
        else
        {
            m_tempDirectory = createTempDirectory("premisses_test_");
        }
        LOG4CXX_DEBUG(logger, "Temporary directory: " << m_tempDirectory);

        // Execute registered setup callbacks
        executeCallbacks(m_setupCallbacks);

        m_isSetUp = true;
        LOG4CXX_INFO(logger, "Global test environment setup completed");
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Environment setup failed: " << e.what());
        throw;  // Re-throw to fail the test run
    }
}

void CustomTestEnvironment::TearDown()
{
    LOG4CXX_INFO(logger, "========================================");
    LOG4CXX_INFO(logger, "  GLOBAL TEST ENVIRONMENT TEARDOWN");
    LOG4CXX_INFO(logger, "========================================");

    try
    {
        // Execute registered teardown callbacks (in reverse order)
        std::vector<EnvironmentCallback> reversedCallbacks(
            m_teardownCallbacks.rbegin(), m_teardownCallbacks.rend());
        executeCallbacks(reversedCallbacks);

        // Clean up temporary directory
        if (!m_tempDirectory.empty() && std::filesystem::exists(m_tempDirectory))
        {
            LOG4CXX_DEBUG(logger, "Cleaning up temp directory: " << m_tempDirectory);
            std::error_code ec;
            std::filesystem::remove_all(m_tempDirectory, ec);
            if (ec)
            {
                LOG4CXX_WARN(logger, "Failed to remove temp directory: " << ec.message());
            }
        }

        // Clean up environment variables
        cleanupEnvironment();

        m_isSetUp = false;
        LOG4CXX_INFO(logger, "Global test environment teardown completed");
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Environment teardown failed: " << e.what());
        // Don't re-throw in teardown - log and continue
    }
}

// ============================================
// Configuration & State
// ============================================

auto CustomTestEnvironment::getTempDirectory() const noexcept -> std::optional<std::string>
{
    if (m_tempDirectory.empty())
    {
        return std::nullopt;
    }
    return m_tempDirectory;
}

auto CustomTestEnvironment::getTestPort() const noexcept -> std::uint16_t
{
    return m_testPort;
}

auto CustomTestEnvironment::isSetUp() const noexcept -> bool
{
    return m_isSetUp;
}

// ============================================
// Hook Registration
// ============================================

void CustomTestEnvironment::addSetUpCallback(EnvironmentCallback callback)
{
    if (callback)
    {
        m_setupCallbacks.push_back(std::move(callback));
        LOG4CXX_TRACE(logger, "Setup callback registered");
    }
}

void CustomTestEnvironment::addTearDownCallback(EnvironmentCallback callback)
{
    if (callback)
    {
        m_teardownCallbacks.push_back(std::move(callback));
        LOG4CXX_TRACE(logger, "Teardown callback registered");
    }
}

// ============================================
// Static Utilities
// ============================================

auto CustomTestEnvironment::generateRandomPort() -> std::uint16_t
{
    std::random_device rd;
    std::mt19937 gen(rd());
    // Use port range 10000-60000 to avoid conflicts
    std::uniform_int_distribution<std::uint16_t> dist(10000, 60000);
    return dist(gen);
}

auto CustomTestEnvironment::createTempDirectory(std::string_view prefix) -> std::string
{
    // Get system temp directory
    std::filesystem::path tempBase = std::filesystem::temp_directory_path();

    // Generate unique suffix using timestamp and random number
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1000, 9999);

    std::ostringstream dirname;
    dirname << prefix << timestamp << "_" << dist(gen);

    std::filesystem::path tempDir = tempBase / dirname.str();

    std::error_code ec;
    std::filesystem::create_directories(tempDir, ec);
    if (ec)
    {
        throw std::runtime_error("Failed to create temp directory: " + ec.message());
    }

    return tempDir.string();
}

// ============================================
// Private Methods
// ============================================

void CustomTestEnvironment::configureEnvironment()
{
    LOG4CXX_TRACE(logger, "Configuring test environment variables");

    constexpr int overwrite = 1;

    // Network configuration for tests
    setenv("MASTER_PORT_5984_TCP_ADDR", "127.0.0.1", overwrite);
    setenv("MASTER_PORT_5984_TCP_PORT", std::to_string(m_testPort).c_str(), overwrite);

    // Logging configuration
    setenv("SPLOG_STDERR", "1", overwrite);
    setenv("LOG4CXX_CONFIGURATION_WATCH_SECONDS", "120", 0);  // Don't overwrite existing

    if (m_config.logConfigPath.has_value())
    {
        setenv("LOG4CXX_CONFIGURATION",
               m_config.logConfigPath.value().c_str(), overwrite);
        LOG4CXX_DEBUG(logger, "Log config path: " << m_config.logConfigPath.value());
    }
    else
    {
        // Default log configuration path
        setenv("LOG4CXX_CONFIGURATION", "/etc/configs/nightOwl.xml", 0);
    }

    // Test-specific environment variables
    if (!m_tempDirectory.empty())
    {
        setenv("TEST_TMPDIR", m_tempDirectory.c_str(), overwrite);
    }

    LOG4CXX_DEBUG(logger, "Environment variables configured");
}

void CustomTestEnvironment::cleanupEnvironment()
{
    LOG4CXX_TRACE(logger, "Cleaning up test environment variables");

    // Remove test-specific environment variables
    unsetenv("MASTER_PORT_5984_TCP_ADDR");
    unsetenv("MASTER_PORT_5984_TCP_PORT");
    unsetenv("TEST_TMPDIR");
    // Note: Don't remove LOG4CXX_CONFIGURATION - might affect logging cleanup

    LOG4CXX_DEBUG(logger, "Environment variables cleaned up");
}

void CustomTestEnvironment::executeCallbacks(
    const std::vector<EnvironmentCallback>& callbacks)
{
    for (const auto& callback : callbacks)
    {
        try
        {
            if (callback)
            {
                callback();
            }
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Callback execution failed: " << e.what());
            throw;
        }
    }
}
