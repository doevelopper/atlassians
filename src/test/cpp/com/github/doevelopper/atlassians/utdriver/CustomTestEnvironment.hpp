/*******************************************************************
 * @file    CustomTestEnvironment.hpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Custom Google Test Environment for global setup/teardown
 *
 * This class derives from ::testing::Environment to provide global
 * setup and teardown operations that run once per test execution,
 * before and after all test suites.
 *
 * Use cases:
 * - Database connection setup
 * - Service initialization
 * - Environment variable configuration
 * - Resource allocation for all tests
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <functional>
#include <vector>

#include <gtest/gtest.h>
#include <log4cxx/logger.h>

namespace com::github::doevelopper::premisses::utdriver
{
    /**
     * @class CustomTestEnvironment
     * @brief Global test environment with logging and resource management
     *
     * Inherits from ::testing::Environment to provide hooks that are called
     * once at the start and end of test program execution.
     *
     * Features:
     * - Environment variable management
     * - Logging configuration
     * - Temporary directory management
     * - Resource cleanup
     *
     * @note Google Test takes ownership of the environment object.
     *       Do not delete it manually.
     *
     * @example
     * // Registration happens automatically in TestDriver
     * ::testing::AddGlobalTestEnvironment(new CustomTestEnvironment());
     */
    class CustomTestEnvironment : public ::testing::Environment
    {
    public:
        /**
         * @brief Callback type for setup/teardown hooks
         */
        using EnvironmentCallback = std::function<void()>;

        /**
         * @brief Configuration for the test environment
         */
        struct Config
        {
            std::optional<std::string> tempDirectory;
            std::optional<std::string> logConfigPath;
            std::optional<std::uint16_t> testPort;
            bool enableColoredOutput{true};
            bool verboseLogging{false};
        };

        // ============================================
        // Constructors & Destructor
        // ============================================

        /**
         * @brief Default constructor
         */
        CustomTestEnvironment() noexcept;

        /**
         * @brief Constructor with configuration
         * @param config Environment configuration
         */
        explicit CustomTestEnvironment(const Config& config) noexcept;

        /**
         * @brief Destructor
         * @note Google Test owns this object - destructor called automatically
         */
        ~CustomTestEnvironment() noexcept override;

        // Non-copyable, non-movable (owned by Google Test)
        CustomTestEnvironment(const CustomTestEnvironment&) = delete;
        CustomTestEnvironment& operator=(const CustomTestEnvironment&) = delete;
        CustomTestEnvironment(CustomTestEnvironment&&) = delete;
        CustomTestEnvironment& operator=(CustomTestEnvironment&&) = delete;

        // ============================================
        // Google Test Environment Interface
        // ============================================

        /**
         * @brief Called before all tests run
         *
         * Override this to define global setup operations:
         * - Initialize logging
         * - Set environment variables
         * - Create temporary directories
         * - Initialize shared resources
         */
        void SetUp() override;

        /**
         * @brief Called after all tests complete
         *
         * Override this to define global teardown operations:
         * - Clean up temporary files
         * - Close connections
         * - Reset environment variables
         * - Release shared resources
         */
        void TearDown() override;

        // ============================================
        // Configuration & State
        // ============================================

        /**
         * @brief Get the temporary directory path
         * @return Path to temporary directory, or nullopt if not configured
         */
        [[nodiscard]] auto getTempDirectory() const noexcept -> std::optional<std::string>;

        /**
         * @brief Get the test port
         * @return Port number used for network tests
         */
        [[nodiscard]] auto getTestPort() const noexcept -> std::uint16_t;

        /**
         * @brief Check if environment is set up
         * @return true if SetUp() has been called
         */
        [[nodiscard]] auto isSetUp() const noexcept -> bool;

        // ============================================
        // Hook Registration
        // ============================================

        /**
         * @brief Register a callback to run during SetUp
         * @param callback Function to call during SetUp
         */
        void addSetUpCallback(EnvironmentCallback callback);

        /**
         * @brief Register a callback to run during TearDown
         * @param callback Function to call during TearDown
         */
        void addTearDownCallback(EnvironmentCallback callback);

        // ============================================
        // Static Utilities
        // ============================================

        /**
         * @brief Generate a random available port number
         * @return Random port in range [1024, 65535]
         */
        [[nodiscard]] static auto generateRandomPort() -> std::uint16_t;

        /**
         * @brief Create a unique temporary directory
         * @param prefix Prefix for directory name
         * @return Path to created directory
         */
        [[nodiscard]] static auto createTempDirectory(
            std::string_view prefix = "test_") -> std::string;

    private:
        /**
         * @brief Configure environment variables for testing
         */
        void configureEnvironment();

        /**
         * @brief Clean up environment variables
         */
        void cleanupEnvironment();

        /**
         * @brief Execute all registered callbacks
         * @param callbacks Vector of callbacks to execute
         */
        void executeCallbacks(const std::vector<EnvironmentCallback>& callbacks);

        // Member variables
        Config m_config;
        bool m_isSetUp{false};
        std::uint16_t m_testPort{0};
        std::string m_tempDirectory;
        std::vector<EnvironmentCallback> m_setupCallbacks;
        std::vector<EnvironmentCallback> m_teardownCallbacks;

        // Logger
        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::premisses::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_CUSTOMTESTENVIRONMENT_HPP
