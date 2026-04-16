/*******************************************************************
 * @file    main.cpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   BDD test runner main entry point
 *
 * This file contains the main entry point for the BDD test executable.
 * It initializes the logging system, configures the BDD driver,
 * and starts the cucumber-cpp wire protocol server.
 *
 * The wire server listens for connections from the cucumber Ruby test
 * runner, which executes feature files and calls step definitions
 * registered in this executable.
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

#include <com/github/doevelopper/premisses/bdddriver/BddDriver.hpp>
#include <com/github/doevelopper/premisses/bdddriver/BddEnvironment.hpp>
#include <com/github/doevelopper/premisses/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/premisses/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::premisses::logging;
using namespace com::github::doevelopper::premisses::bdddriver;

namespace
{
    // Logger for main entry point
    log4cxx::LoggerPtr logger =
        log4cxx::Logger::getLogger("com.github.doevelopper.premisses.bdd.main");

    /**
     * @brief Extract program name from command line argument
     * @param argv0 The first command line argument (program path)
     * @return The program name without path and extension
     */
    [[nodiscard]] std::string extractProgramName(const char* argv0)
    {
        if (argv0 == nullptr || argv0[0] == '\0')
        {
            return "BddSuite";
        }

        try
        {
            std::filesystem::path programPath(argv0);
            std::string filename = programPath.stem().string();

            // Remove common BDD/wire suffixes for cleaner display
            const std::vector<std::string> suffixes = {
                ".wire.steps", ".wire", ".bdd", ".feature", ".steps",
                "_wire_steps", "_wire", "_bdd", "_steps"
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

            return filename.empty() ? "BddSuite" : filename;
        }
        catch (...)
        {
            return "BddSuite";
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

    /**
     * @brief Display usage information
     * @param programName The program name
     */
    void displayUsage(const std::string& programName)
    {
        std::cout << "\nUsage: " << programName << " [OPTIONS]\n\n"
                  << "BDD Test Step Definitions Server (Cucumber Wire Protocol)\n\n"
                  << "This executable starts a wire protocol server that cucumber connects to\n"
                  << "for executing step definitions. It should be started by the BDD test\n"
                  << "framework (via CMake/CTest), not manually.\n\n"
                  << "Options:\n"
                  << "  --port <port>     Wire protocol port (default: 3902)\n"
                  << "  --verbose, -v     Enable verbose output\n"
                  << "  --no-color        Disable colored output\n"
                  << "  --filter <regex>  Filter step definitions by regex\n"
                  << "  --help, -h        Display this help message\n\n"
                  << "Environment:\n"
                  << "  The cucumber Ruby test runner should be configured with a\n"
                  << "  cucumber.wire file that specifies the host and port to connect to.\n\n"
                  << "Example cucumber.wire:\n"
                  << "  host: localhost\n"
                  << "  port: 3902\n"
                  << std::endl;
    }
}  // anonymous namespace

/**
 * @brief Main entry point of the BDD test server
 *
 * This function:
 * 1. Initializes the logging system
 * 2. Configures the BDD driver with desired options
 * 3. Starts the cucumber-cpp wire protocol server
 * 4. Waits for cucumber test runner to connect and run scenarios
 * 5. Returns appropriate exit code
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @return EXIT_SUCCESS (0) if all tests pass, EXIT_FAILURE (1) otherwise
 *
 * @note This executable is designed to be run by the BDD test framework
 *       (CMake/CTest), which manages starting the cucumber Ruby runner
 *       after this server is listening.
 *
 * @example
 * # Normally run via CMake/CTest:
 * ctest -R acceptance.wire
 *
 * # Manual execution (for debugging):
 * ./Premisses.acceptance.wire.steps --port 3902 --verbose
 * # In another terminal:
 * cd features && cucumber
 */
int main(int argc, char** argv)
{
    // Check for help flag early
    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);
        if (arg == "--help" || arg == "-h")
        {
            displayUsage(extractProgramName(argc > 0 ? argv[0] : nullptr));
            return EXIT_SUCCESS;
        }
    }

    // Extract program name for logging
    const std::string programName = extractProgramName(argc > 0 ? argv[0] : nullptr);

    // Initialize logging first
    if (!initializeLogging())
    {
        std::cerr << "Warning: Logging initialization failed, continuing with defaults"
                  << std::endl;
    }

    LOG4CXX_INFO(logger, "========================================");
    LOG4CXX_INFO(logger, "  " << programName << " BDD Suite Starting");
    LOG4CXX_INFO(logger, "========================================");

    std::int32_t runStatus = EXIT_SUCCESS;

    try
    {
        // Initialize BDD environment (global setup)
        auto& bddEnv = BddEnvironment::getInstance();
        bddEnv.SetUp();

        // Configure the BDD driver with desired options
        // Default wire port is 3902 (matches cucumber.wire files)
        auto config = BddDriverConfig::Builder()
            .withWirePort(3902)
            .withVerboseOutput(false)
            .withColorOutput(true)
            .build();

        // Create and initialize the BDD driver
        auto bddDriver = std::make_unique<BddDriver>(config);

        // Execute BDD tests (combines setup and run)
        // This will start the wire server and block until cucumber finishes
        runStatus = bddDriver->execute(argc, argv);

        // Cleanup (ignore return value - we already have the test result)
        static_cast<void>(bddDriver->teardown());

        LOG4CXX_INFO(logger, "BDD execution completed with status: " << runStatus);

        // Cleanup BDD environment (global teardown)
        bddEnv.TearDown();
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
    LOG4CXX_INFO(logger, "  " << programName << " BDD Suite " <<
                 (runStatus == 0 ? "COMPLETED" : "FAILED"));
    LOG4CXX_INFO(logger, "========================================");

    return (runStatus == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}