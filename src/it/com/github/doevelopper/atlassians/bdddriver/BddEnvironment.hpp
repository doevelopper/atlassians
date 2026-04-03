/*******************************************************************
 * @file    BddEnvironment.hpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   Custom BDD test environment for setup/teardown
 *
 * This class provides a custom environment for BDD tests that integrates
 * with cucumber-cpp. It handles global setup and teardown operations
 * that should occur before and after all BDD scenarios run.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_BDDDRIVER_BDDENVIRONMENT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_BDDDRIVER_BDDENVIRONMENT_HPP

#include <memory>
#include <string>

#include <log4cxx/logger.h>

namespace com::github::doevelopper::premisses::bdddriver
{
    /**
     * @class BddEnvironment
     * @brief Custom environment for BDD test lifecycle management
     *
     * This class is automatically registered with cucumber-cpp and provides
     * hooks for global setup and teardown operations. It ensures that:
     * - Logging is properly initialized before any scenarios run
     * - Resources are properly cleaned up after all scenarios complete
     * - Environment state is properly managed across scenario runs
     *
     * The environment is registered via static initialization, so it will
     * be available when the step definitions executable starts.
     */
    class BddEnvironment
    {
    public:
        /**
         * @brief Constructor
         */
        BddEnvironment() noexcept;

        /**
         * @brief Destructor
         */
        virtual ~BddEnvironment() noexcept;

        // Disable copy and move operations
        BddEnvironment(const BddEnvironment&) = delete;
        BddEnvironment& operator=(const BddEnvironment&) = delete;
        BddEnvironment(BddEnvironment&&) = delete;
        BddEnvironment& operator=(BddEnvironment&&) = delete;

        /**
         * @brief Setup operations performed before any scenarios run
         *
         * This is called once when the BDD test suite starts, before any
         * step definitions are executed. Use this for:
         * - Logging initialization
         * - Global resource allocation
         * - Environment configuration
         */
        virtual void SetUp();

        /**
         * @brief Teardown operations performed after all scenarios complete
         *
         * This is called once when the BDD test suite completes, after all
         * step definitions have finished executing. Use this for:
         * - Logging cleanup
         * - Global resource deallocation
         * - Final environment cleanup
         */
        virtual void TearDown();

        /**
         * @brief Get the singleton instance
         * @return Reference to the BDD environment instance
         */
        [[nodiscard]] static auto getInstance() -> BddEnvironment&;

    protected:
        /**
         * @brief Hook for subclasses to add custom setup logic
         */
        virtual void onEnvironmentSetup();

        /**
         * @brief Hook for subclasses to add custom teardown logic
         */
        virtual void onEnvironmentTeardown();

    private:
        /**
         * @brief Initialize the logging system
         * @return true if logging initialized successfully
         */
        [[nodiscard]] auto initializeLogging() -> bool;

        /**
         * @brief Cleanup the logging system
         */
        void cleanupLogging() noexcept;

        // State tracking
        bool m_isSetup{false};

        // Logger instance
        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::premisses::bdddriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_BDDDRIVER_BDDENVIRONMENT_HPP
