/*******************************************************************
 * @file    main.cpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Test runner main entry point
 *
 * This file contains the main entry point for the test executable.
 * It initializes the logging system, configures the test driver,
 * and runs all registered Google Tests.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include <log4cxx/logger.h>

#include <com/github/doevelopper/atlassians/utdriver/TestDriver.hpp>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::atlassians::logging;
using namespace com::github::doevelopper::atlassians::utdriver;

namespace
{
    // Logger for main entry point
    log4cxx::LoggerPtr logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.test.main");

    /**
     * @brief Extract program name from command line argument
     * @param argv0 The first command line argument (program path)
     * @return The program name without path and extension
     */
    [[nodiscard]] std::string extractProgramName(const char* argv0)
    {
        if (argv0 == nullptr || argv0[0] == '\0')
        {
            return "TestSuite";
        }

        try
        {
            std::filesystem::path programPath(argv0);
            std::string filename = programPath.stem().string();

            // Remove common test suffixes for cleaner display
            const std::vector<std::string> suffixes = {
                ".test", "_test", "-test", ".tests", "_tests", "-tests"
            };

            for (const auto& suffix : suffixes)
            {
                if (filename.length() > suffix.length())
                {
                    auto pos = filename.rfind(suffix);
                    if (pos != std::string::npos &&
                        pos == filename.length() - suffix.length())
                    {
                        filename = filename.substr(0, pos);
                        break;
                    }
                }
            }

            return filename.empty() ? "TestSuite" : filename;
        }
        catch (...)
        {
            return "TestSuite";
        }
    }

    /**
     * @brief Initialize the logging subsystem
     * @return true if initialization succeeded
     */
    bool initializeLogging()
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
}  // anonymous namespace

/**
 * @brief Main entry point of the test runner
 *
 * This function:
 * 1. Initializes the logging system
 * 2. Configures the test driver with desired options
 * 3. Runs all registered Google Tests
 * 4. Returns appropriate exit code
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return EXIT_SUCCESS (0) if all tests pass, EXIT_FAILURE (1) otherwise
 *
 * @note Command line arguments are passed through to Google Test.
 *       Use --help to see available Google Test options.
 *
 * @example
 * # Run all tests
 * ./Premisses.test.bin
 *
 * # Run specific test suite
 * ./Premisses.test.bin --gtest_filter=CalculatorTest.*
 *
 * # Run with verbose output
 * ./Premisses.test.bin --gtest_print_time=1
 *
 * # Generate XML report
 * ./Premisses.test.bin --gtest_output=xml:test_results.xml
 */
int main(int argc, char** argv)
{
    // Extract program name for logging
    const std::string programName = extractProgramName(argc > 0 ? argv[0] : nullptr);

    // Initialize logging first
    if (!initializeLogging())
    {
        std::cerr << "Warning: Logging initialization failed, continuing with defaults"
                  << std::endl;
    }

    LOG4CXX_INFO(logger, "========================================");
    LOG4CXX_INFO(logger, "  " << programName << " Test Suite Starting");
    LOG4CXX_INFO(logger, "========================================");

    std::int32_t runStatus = EXIT_SUCCESS;

    try
    {
        // Configure the test driver with desired options
        auto config = TestDriverConfig::Builder()
            .withColorOutput(true)
            .withPrintTimePerTest(true)
            .withBreakOnFailure(false)
            .withRepeatCount(1)
            .build();

        // Create and initialize the test driver
        auto testDriver = std::make_unique<TestDriver>(config);

        // Execute all tests (combines setup and run)
        runStatus = testDriver->execute(argc, argv);

        // Cleanup (ignore return value - we already have the test result)
        static_cast<void>(testDriver->teardown());

        LOG4CXX_INFO(logger, "Test execution completed with status: " << runStatus);
    }
    catch (const char* str)
    {
        LOG4CXX_ERROR(logger, "Exception (C-string): " << str);
        std::cerr << "Error: " << str << std::endl;
        runStatus = EXIT_FAILURE;
    }
    catch (const std::string& str)
    {
        LOG4CXX_ERROR(logger, "Exception (std::string): " << str);
        std::cerr << "Error: " << str << std::endl;
        runStatus = EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Exception: " << e.what());
        std::cerr << "Error: " << e.what() << std::endl;
        runStatus = EXIT_FAILURE;
    }
    catch (...)
    {
        LOG4CXX_ERROR(logger, "Unknown exception caught");
        std::cerr << "Error: Unknown exception" << std::endl;
        runStatus = EXIT_FAILURE;
    }

    LOG4CXX_INFO(logger, "========================================");
    LOG4CXX_INFO(logger, "  " << programName << " Test Suite " <<
                 (runStatus == 0 ? "PASSED" : "FAILED"));
    LOG4CXX_INFO(logger, "========================================");

    return (runStatus == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
