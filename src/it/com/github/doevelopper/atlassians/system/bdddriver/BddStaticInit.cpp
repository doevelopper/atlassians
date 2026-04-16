/*******************************************************************
 * @file    BddStaticInit.cpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   Static initialization for BDD environment
 *
 * This file provides static initialization that runs before main()
 * to set up the logging framework for BDD tests. By using static
 * initialization, we ensure logging is configured before cucumber-cpp's
 * built-in main() starts the wire server.
 *
 * SPDX-LICENSE-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <iostream>
#include <memory>

#include <com/github/doevelopper/atlassians/bdddriver/BddEnvironment.hpp>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>

using namespace com::github::doevelopper::atlassians::bdddriver;
using namespace com::github::doevelopper::atlassians::logging;

namespace
{
    /**
     * @brief Static initializer that runs before main()
     *
     * This class uses the "static initialization order fiasco" solution
     * to ensure logging is initialized before cucumber-cpp starts.
     */
    class BddStaticInitializer
    {
    public:
        BddStaticInitializer()
        {
            try
            {
                // Initialize logging framework
                auto& initializer = LoggingInitializer::getInstance();
                if (!initializer.isInitialized())
                {
                    auto strategy = std::make_unique<DefaultInitializationStrategy>();
                    if (initializer.initialize(std::move(strategy)))
                    {
                        auto logger = log4cxx::Logger::getLogger(
                            "com.github.doevelopper.atlassians.bdd.init");
                        LOG4CXX_INFO(logger, "========================================");
                        LOG4CXX_INFO(logger, "  BDD Static Initialization Complete");
                        LOG4CXX_INFO(logger, "========================================");
                        LOG4CXX_DEBUG(logger, "Logging framework initialized before main()");
                    }
                    else
                    {
                        std::cerr << "Warning: BDD logging initialization failed" << std::endl;
                    }
                }

                // Initialize BDD environment
                auto& env = BddEnvironment::getInstance();
                env.SetUp();
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error during BDD static initialization: " << e.what() << std::endl;
            }
        }

        ~BddStaticInitializer()
        {
            try
            {
                // Cleanup BDD environment
                auto& env = BddEnvironment::getInstance();
                env.TearDown();

                auto logger = log4cxx::Logger::getLogger(
                    "com.github.doevelopper.atlassians.bdd.init");
                LOG4CXX_INFO(logger, "========================================");
                LOG4CXX_INFO(logger, "  BDD Static Cleanup Complete");
                LOG4CXX_INFO(logger, "========================================");
            }
            catch (...)
            {
                // Suppress exceptions in destructor
            }
        }
    };

    // Static instance - constructor runs before main(), destructor after main()
    [[maybe_unused]] static BddStaticInitializer bddInitializer;

}  // anonymous namespace
