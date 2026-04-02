/*******************************************************************
 * @file    BddDriver.cpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   Cucumber-CPP BDD Driver implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <cstring>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>
#include <cucumber-cpp/internal/connectors/wire/WireServer.hpp>
#include <cucumber-cpp/internal/connectors/wire/WireProtocol.hpp>
#include <cucumber-cpp/internal/CukeEngineImpl.hpp>

#include <com/github/doevelopper/premisses/bdddriver/BddDriver.hpp>
#include <com/github/doevelopper/premisses/bdddriver/BddEnvironment.hpp>
#include <com/github/doevelopper/premisses/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::premisses::bdddriver;
using namespace com::github::doevelopper::premisses::logging;

// Initialize static logger
log4cxx::LoggerPtr BddDriver::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.premisses.bdddriver.BddDriver");

// ============================================
// BddDriver Implementation
// ============================================

BddDriver::BddDriver() noexcept
    : m_config{}
    , m_suiteUnderTest{"PremissesBddSuite"}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    static_cast<void>(initializeLogging());
    LOG4CXX_TRACE(logger, "BddDriver default constructor called");
}

BddDriver::BddDriver(const BddDriverConfig& config) noexcept
    : m_config{config}
    , m_suiteUnderTest{"PremissesBddSuite"}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    static_cast<void>(initializeLogging());
    LOG4CXX_TRACE(logger, "BddDriver constructed with custom configuration");
}

BddDriver::BddDriver(std::string_view suiteName) noexcept
    : m_config{}
    , m_suiteUnderTest{suiteName}
    , m_isInitialized{false}
    , m_lastResult{ExecutionResult::Success}
{
    static_cast<void>(initializeLogging());
    LOG4CXX_DEBUG(logger, "BddDriver constructed for suite: " << suiteName);
}

BddDriver::~BddDriver() noexcept
{
    LOG4CXX_TRACE(logger, "BddDriver destructor called");
    if (m_isInitialized)
    {
        static_cast<void>(teardown());
    }
}

BddDriver::BddDriver(BddDriver&& other) noexcept
    : m_config{std::move(other.m_config)}
    , m_suiteUnderTest{std::move(other.m_suiteUnderTest)}
    , m_isInitialized{other.m_isInitialized}
    , m_lastResult{other.m_lastResult}
{
    other.m_isInitialized = false;
    LOG4CXX_TRACE(logger, "BddDriver move constructor called");
}

BddDriver& BddDriver::operator=(BddDriver&& other) noexcept
{
    if (this != &other)
    {
        m_config = std::move(other.m_config);
        m_suiteUnderTest = std::move(other.m_suiteUnderTest);
        m_isInitialized = other.m_isInitialized;
        m_lastResult = other.m_lastResult;
        other.m_isInitialized = false;
    }
    LOG4CXX_TRACE(logger, "BddDriver move assignment called");
    return *this;
}

auto BddDriver::initializeLogging() -> bool
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

auto BddDriver::setup(int argc, char* argv[]) -> bool
{
    LOG4CXX_INFO(logger, "BddDriver::setup() - Initializing BDD framework");

    if (m_isInitialized)
    {
        LOG4CXX_WARN(logger, "BddDriver already initialized, skipping setup");
        return true;
    }

    try
    {
        // Extract suite name from executable path
        extractSuiteName(argv);
        LOG4CXX_DEBUG(logger, "Suite under test: " << m_suiteUnderTest);

        // Parse command line arguments
        parseCommandLineArgs(argc, argv);

        // Apply configuration
        applyConfiguration();

        // Register custom BDD environment
        registerBddEnvironment();

        // Call hook for subclass customization
        onEnvironmentSetup();

        m_isInitialized = true;
        LOG4CXX_INFO(logger, "BddDriver setup completed successfully");
        LOG4CXX_INFO(logger, "Wire protocol listening on port: " << m_config.wirePort);

        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "BddDriver setup failed: " << e.what());
        m_lastResult = ExecutionResult::InitializationError;
        return false;
    }
}

void BddDriver::parseCommandLineArgs(int argc, char* argv[])
{
    LOG4CXX_TRACE(logger, "Parsing command line arguments");

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg == "--port" && i + 1 < argc)
        {
            try
            {
                m_config.wirePort = static_cast<std::uint16_t>(std::stoi(argv[++i]));
                LOG4CXX_DEBUG(logger, "Wire port set to: " << m_config.wirePort);
            }
            catch (const std::exception& e)
            {
                LOG4CXX_WARN(logger, "Invalid port number: " << argv[i] << ", using default");
            }
        }
        else if (arg == "--verbose" || arg == "-v")
        {
            m_config.verboseOutput = true;
            LOG4CXX_DEBUG(logger, "Verbose output enabled");
        }
        else if (arg == "--no-color")
        {
            m_config.colorOutput = false;
            LOG4CXX_DEBUG(logger, "Color output disabled");
        }
        else if (arg == "--filter" && i + 1 < argc)
        {
            m_config.stepsFilter = argv[++i];
            LOG4CXX_DEBUG(logger, "Steps filter set to: " << m_config.stepsFilter.value());
        }
    }
}

void BddDriver::applyConfiguration()
{
    LOG4CXX_TRACE(logger, "Applying BDD configuration");

    // Configuration is primarily applied through cucumber-cpp command line args
    // which are handled by the cucumber ruby process, not here

    if (m_config.verboseOutput)
    {
        LOG4CXX_DEBUG(logger, "Verbose mode enabled");
    }

    if (!m_config.colorOutput)
    {
        LOG4CXX_DEBUG(logger, "Color output disabled");
    }
}

void BddDriver::registerBddEnvironment()
{
    LOG4CXX_TRACE(logger, "Registering custom BDD environment");

    // The BDD environment will be automatically registered through
    // static initialization when BddEnvironment.cpp is linked

    LOG4CXX_DEBUG(logger, "BddEnvironment registered");
}

auto BddDriver::run() -> int
{
    LOG4CXX_INFO(logger, "BddDriver::run() - Starting cucumber-cpp wire server");

    if (!m_isInitialized)
    {
        LOG4CXX_ERROR(logger, "BddDriver not initialized, cannot run");
        return static_cast<int>(ExecutionResult::InitializationError);
    }

    try
    {
        LOG4CXX_INFO(logger, "Starting cucumber-cpp wire server on port " << m_config.wirePort);
        LOG4CXX_INFO(logger, "Waiting for cucumber test runner to connect...");

        // Create CukeEngine and wire protocol components
        cucumber::internal::CukeEngineImpl cukeEngine;
        cucumber::internal::JsonWireMessageCodec codec;
        cucumber::internal::WireProtocolHandler wireProtocolHandler(codec, cukeEngine);
        cucumber::internal::TCPSocketServer server(&wireProtocolHandler);

        // Start listening on the configured port
        server.listen(m_config.wirePort);

        LOG4CXX_INFO(logger, "Listening on port " << m_config.wirePort);

        // Accept connections and process requests
        // This will block until the cucumber runner finishes
        server.acceptOnce();

        LOG4CXX_INFO(logger, "BDD execution completed successfully");

        m_lastResult = ExecutionResult::Success;
        return 0;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "BDD execution failed: " << e.what());
        m_lastResult = ExecutionResult::ServerError;
        return static_cast<int>(ExecutionResult::ServerError);
    }
}

auto BddDriver::execute(int argc, char* argv[]) -> int
{
    LOG4CXX_TRACE(logger, "Combined setup and run");

    if (!setup(argc, argv))
    {
        return static_cast<int>(ExecutionResult::InitializationError);
    }

    return run();
}

auto BddDriver::teardown() noexcept -> bool
{
    LOG4CXX_INFO(logger, "Cleaning up resources");

    if (!m_isInitialized)
    {
        LOG4CXX_DEBUG(logger, "BddDriver not initialized, nothing to teardown");
        return true;
    }

    try
    {
        // Call hook for subclass cleanup
        onEnvironmentTeardown();

        m_isInitialized = false;
        LOG4CXX_INFO(logger, "BddDriver teardown completed");
        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "BddDriver teardown failed: " << e.what());
        return false;
    }
}

void BddDriver::extractSuiteName(char* argv[])
{
    if (argv == nullptr || argv[0] == nullptr)
    {
        m_suiteUnderTest = "UnknownSuite";
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

        m_suiteUnderTest = filename;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_WARN(logger, "Failed to extract suite name: " << e.what());
        m_suiteUnderTest = "UnknownSuite";
    }
}

auto BddDriver::getSuiteUnderTest() const noexcept -> std::string_view
{
    return m_suiteUnderTest;
}

auto BddDriver::isInitialized() const noexcept -> bool
{
    return m_isInitialized;
}

auto BddDriver::getExecutionResult() const noexcept -> ExecutionResult
{
    return m_lastResult;
}

auto BddDriver::getConfig() const noexcept -> const BddDriverConfig&
{
    return m_config;
}

auto BddDriver::getWirePort() const noexcept -> std::uint16_t
{
    return m_config.wirePort;
}

auto BddDriver::getStepDefinitionsCount() -> std::size_t
{
    // This would require accessing cucumber-cpp internal registry
    // For now, return 0 as a placeholder
    // In practice, cucumber will log the registered steps
    return 0;
}

void BddDriver::onEnvironmentSetup()
{
    LOG4CXX_TRACE(logger, "BddDriver::onEnvironmentSetup() - hook called");
    // Default implementation does nothing
    // Subclasses can override for custom setup
}

void BddDriver::onEnvironmentTeardown()
{
    LOG4CXX_TRACE(logger, "BddDriver::onEnvironmentTeardown() - hook called");
    // Default implementation does nothing
    // Subclasses can override for custom teardown
}
