/*******************************************************************
 * @file    BddDriver.hpp
 * @version v1.0.0
 * @date    2026/02/04
 * @brief   Cucumber-CPP BDD Driver with comprehensive lifecycle management
 *
 * This class provides a modern C++ wrapper around cucumber-cpp that manages
 * the complete BDD test lifecycle including initialization, execution, and cleanup.
 * It follows SOLID principles and integrates with the logging framework.
 *
 * Design Patterns:
 * - Facade: Simplifies cucumber-cpp wire protocol API interaction
 * - Template Method: Defines BDD test lifecycle skeleton
 * - Strategy: Allows custom test environment configuration
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_ATLASSIANS_BDDDRIVER_BDDDRIVER_HPP
#define COM_GITHUB_DOEVELOPPER_ATLASSIANS_BDDDRIVER_BDDDRIVER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <log4cxx/logger.h>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>

namespace com::github::doevelopper::atlassians::bdddriver
{
    /**
     * @class BddDriverConfig
     * @brief Configuration options for BddDriver
     *
     * Uses the Builder pattern to construct configuration in a fluent manner.
     */
    struct BddDriverConfig
    {
        std::uint16_t wirePort{3902};           ///< Cucumber wire protocol port
        bool verboseOutput{false};               ///< Enable verbose output
        bool colorOutput{true};                  ///< Enable colored output
        std::uint32_t startupTimeoutMs{10000};  ///< Startup timeout in milliseconds
        std::optional<std::string> stepsFilter; ///< Filter for step definitions

        /**
         * @brief Builder pattern for BddDriverConfig
         */
        class Builder
        {
        public:
            Builder() = default;

            [[nodiscard]] auto withWirePort(std::uint16_t port) noexcept -> Builder&
            {
                m_wirePort = port;
                return *this;
            }

            [[nodiscard]] auto withVerboseOutput(bool verbose) noexcept -> Builder&
            {
                m_verboseOutput = verbose;
                return *this;
            }

            [[nodiscard]] auto withColorOutput(bool color) noexcept -> Builder&
            {
                m_colorOutput = color;
                return *this;
            }

            [[nodiscard]] auto withStartupTimeout(std::uint32_t timeoutMs) noexcept -> Builder&
            {
                m_startupTimeoutMs = timeoutMs;
                return *this;
            }

            [[nodiscard]] auto withStepsFilter(std::string_view filter) -> Builder&
            {
                m_stepsFilter = std::string(filter);
                return *this;
            }

            [[nodiscard]] auto build() const noexcept -> BddDriverConfig
            {
                BddDriverConfig config;
                config.wirePort = m_wirePort;
                config.verboseOutput = m_verboseOutput;
                config.colorOutput = m_colorOutput;
                config.startupTimeoutMs = m_startupTimeoutMs;
                config.stepsFilter = m_stepsFilter;
                return config;
            }

        private:
            std::uint16_t m_wirePort{3902};
            bool m_verboseOutput{false};
            bool m_colorOutput{true};
            std::uint32_t m_startupTimeoutMs{10000};
            std::optional<std::string> m_stepsFilter;
        };
    };

    /**
     * @class BddDriver
     * @brief Facade for Cucumber-CPP framework with enhanced lifecycle management
     *
     * Provides a clean API for:
     * - BDD test initialization and configuration
     * - Custom environment registration
     * - Wire protocol server management
     * - Test execution with proper error handling
     * - Logging integration throughout the BDD test lifecycle
     *
     * @example
     * int main(int argc, char* argv[]) {
     *     auto config = BddDriverConfig::Builder()
     *         .withWirePort(3902)
     *         .withVerboseOutput(true)
     *         .build();
     *
     *     BddDriver driver(config);
     *     return driver.run(argc, argv);
     * }
     */
    class BddDriver
    {
    public:
        /**
         * @brief Result type for BDD execution
         */
        enum class ExecutionResult : std::int32_t
        {
            Success = 0,
            TestFailure = 1,
            InitializationError = 2,
            ConfigurationError = 3,
            ServerError = 4,
            UnknownError = -1
        };

        // ============================================
        // Constructors & Destructor
        // ============================================

        /**
         * @brief Default constructor with default configuration
         */
        BddDriver() noexcept;

        /**
         * @brief Constructor with custom configuration
         * @param config BDD driver configuration
         */
        explicit BddDriver(const BddDriverConfig& config) noexcept;

        /**
         * @brief Constructor with suite name
         * @param suiteName Name of the BDD test suite
         */
        explicit BddDriver(std::string_view suiteName) noexcept;

        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~BddDriver() noexcept;

        // Disable copy operations (not copyable)
        BddDriver(const BddDriver&) = delete;
        BddDriver& operator=(const BddDriver&) = delete;

        // Enable move operations
        BddDriver(BddDriver&&) noexcept;
        BddDriver& operator=(BddDriver&&) noexcept;

        // ============================================
        // Lifecycle Management
        // ============================================

        /**
         * @brief Initialize the BDD framework
         * @param argc Command line argument count
         * @param argv Command line arguments
         * @return true if initialization succeeded
         */
        [[nodiscard]] auto setup(int argc, char* argv[]) -> bool;

        /**
         * @brief Start cucumber-cpp wire protocol server and run scenarios
         * @return BDD execution result code
         */
        [[nodiscard]] auto run() -> int;

        /**
         * @brief Convenience method combining setup and run
         * @param argc Command line argument count
         * @param argv Command line arguments
         * @return BDD execution result code
         */
        [[nodiscard]] auto execute(int argc, char* argv[]) -> int;

        /**
         * @brief Cleanup BDD framework resources
         * @return true if cleanup succeeded
         */
        [[nodiscard]] auto teardown() noexcept -> bool;

        // ============================================
        // Configuration & State
        // ============================================

        /**
         * @brief Get the suite under test name
         * @return String view of the suite name
         */
        [[nodiscard]] auto getSuiteUnderTest() const noexcept -> std::string_view;

        /**
         * @brief Check if driver is initialized
         * @return true if setup has been called successfully
         */
        [[nodiscard]] auto isInitialized() const noexcept -> bool;

        /**
         * @brief Get the execution result
         * @return Last execution result
         */
        [[nodiscard]] auto getExecutionResult() const noexcept -> ExecutionResult;

        /**
         * @brief Get current configuration
         * @return Reference to configuration
         */
        [[nodiscard]] auto getConfig() const noexcept -> const BddDriverConfig&;

        /**
         * @brief Get wire protocol port
         * @return Configured wire protocol port
         */
        [[nodiscard]] auto getWirePort() const noexcept -> std::uint16_t;

        // ============================================
        // Static Utility Methods
        // ============================================

        /**
         * @brief Get step definitions count
         * @return Number of registered step definitions
         */
        [[nodiscard]] static auto getStepDefinitionsCount() -> std::size_t;

    protected:
        /**
         * @brief Hook for subclasses to customize environment setup
         * Called before BDD execution begins
         */
        virtual void onEnvironmentSetup();

        /**
         * @brief Hook for subclasses to customize environment teardown
         * Called after BDD execution completes
         */
        virtual void onEnvironmentTeardown();

    private:
        /**
         * @brief Extract suite name from command line arguments
         * @param argv Command line arguments
         */
        void extractSuiteName(char* argv[]);

        /**
         * @brief Apply configuration to cucumber-cpp
         */
        void applyConfiguration();

        /**
         * @brief Initialize logging system
         * @return true if logging initialized successfully
         */
        [[nodiscard]] auto initializeLogging() -> bool;

        /**
         * @brief Register custom BDD environment
         */
        void registerBddEnvironment();

        /**
         * @brief Parse command line arguments specific to BDD
         * @param argc Argument count
         * @param argv Arguments array
         */
        void parseCommandLineArgs(int argc, char* argv[]);

        // Member variables
        BddDriverConfig m_config;
        std::string m_suiteUnderTest;
        bool m_isInitialized{false};
        ExecutionResult m_lastResult{ExecutionResult::Success};

        // Logger instance
        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::atlassians::bdddriver

#endif  // COM_GITHUB_DOEVELOPPER_ATLASSIANS_BDDDRIVER_BDDDRIVER_HPP
