/*******************************************************************
 * @file    BddEnvironment.cpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   Custom BDD test environment implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <iostream>
#include <exception>

#include <com/github/doevelopper/premisses/bdddriver/BddEnvironment.hpp>
#include <com/github/doevelopper/premisses/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/premisses/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::premisses::bdddriver;
using namespace com::github::doevelopper::premisses::logging;

// Initialize static logger
log4cxx::LoggerPtr BddEnvironment::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.premisses.bdddriver.BddEnvironment");

// ============================================
// BddEnvironment Implementation
// ============================================

BddEnvironment::BddEnvironment() noexcept
    : m_isSetup{false}
{
    // Constructor is called during static initialization
    // Don't do heavy work here - save it for SetUp()
}

BddEnvironment::~BddEnvironment() noexcept
{
    if (m_isSetup)
    {
        try
        {
            TearDown();
        }
        catch (...)
        {
            // Suppress exceptions in destructor
        }
    }
}

auto BddEnvironment::getInstance() -> BddEnvironment&
{
    static BddEnvironment instance;
    return instance;
}

void BddEnvironment::SetUp()
{
    if (m_isSetup)
    {
        return;
    }

    try
    {
        // Initialize logging first
        if (!initializeLogging())
        {
            std::cerr << "Warning: Logging initialization failed in BDD environment"
                      << std::endl;
        }

        LOG4CXX_INFO(logger, "========================================");
        LOG4CXX_INFO(logger, "  BDD Test Environment Setup");
        LOG4CXX_INFO(logger, "========================================");

        // Call hook for custom setup
        onEnvironmentSetup();

        m_isSetup = true;

        LOG4CXX_INFO(logger, "BDD environment setup completed successfully");
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "BDD environment setup failed: " << e.what());
        std::cerr << "BDD environment setup failed: " << e.what() << std::endl;
        throw;
    }
}

void BddEnvironment::TearDown()
{
    if (!m_isSetup)
    {
        return;
    }

    try
    {
        LOG4CXX_INFO(logger, "========================================");
        LOG4CXX_INFO(logger, "  BDD Test Environment Teardown");
        LOG4CXX_INFO(logger, "========================================");

        // Call hook for custom teardown
        onEnvironmentTeardown();

        LOG4CXX_INFO(logger, "BDD environment teardown completed successfully");

        // Cleanup logging
        cleanupLogging();

        m_isSetup = false;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "BDD environment teardown failed: " << e.what());
        std::cerr << "BDD environment teardown failed: " << e.what() << std::endl;
    }
}

auto BddEnvironment::initializeLogging() -> bool
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

void BddEnvironment::cleanupLogging() noexcept
{
    try
    {
        auto& initializer = LoggingInitializer::getInstance();
        if (initializer.isInitialized())
        {
            initializer.shutdown();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to cleanup logging: " << e.what() << std::endl;
    }
}

void BddEnvironment::onEnvironmentSetup()
{
    LOG4CXX_TRACE(logger, "BddEnvironment::onEnvironmentSetup() - hook called");

    // Default implementation - can be overridden by subclasses
    // Add any additional setup logic here

    LOG4CXX_DEBUG(logger, "BDD environment custom setup completed");
}

void BddEnvironment::onEnvironmentTeardown()
{
    LOG4CXX_TRACE(logger, "BddEnvironment::onEnvironmentTeardown() - hook called");

    // Default implementation - can be overridden by subclasses
    // Add any additional teardown logic here

    LOG4CXX_DEBUG(logger, "BDD environment custom teardown completed");
}
