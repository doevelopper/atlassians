/*******************************************************************
 * @file    TestDriver.hpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Google Test Driver with comprehensive lifecycle management
 *
 * This class provides a modern C++ wrapper around Google Test that manages
 * the complete test lifecycle including initialization, execution, and cleanup.
 * It follows SOLID principles and integrates with the logging framework.
 *
 * Design Patterns:
 * - Facade: Simplifies Google Test API interaction
 * - Template Method: Defines test lifecycle skeleton
 * - Strategy: Allows custom test environment and listener configuration
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTDRIVER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTDRIVER_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <functional>

#include <gtest/gtest.h>
#include <log4cxx/logger.h>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/CustomLogger.hpp>

// ============================================
// Custom Test Macros for Enhanced Functionality
// ============================================

/**
 * @brief Parameterized test helper macro
 * Generates parameterized test suite from a fixture class
 *
 * @param testsuite The test fixture class name
 * @param testname The test case name
 * @param ... Parameter values for the test
 *
 * @example
 * TEST_PV(CalculatorTest, AdditionTest, 1, 2, 3, 4, 5)
 */
#define TEST_PV(testsuite, testname, ...)                                      \
    class testsuite##_##testname##_Parameterized                               \
        : public testsuite,                                                    \
          public ::testing::WithParamInterface<                                \
              decltype(std::make_tuple(__VA_ARGS__))>                          \
    {};                                                                        \
    INSTANTIATE_TEST_SUITE_P(                                                  \
        testname##_Values,                                                     \
        testsuite##_##testname##_Parameterized,                                \
        ::testing::Values(__VA_ARGS__));                                       \
    TEST_P(testsuite##_##testname##_Parameterized, testname)

/**
 * @brief Assert arrays are approximately equal within tolerance
 */
#define ASSERT_ARRAY_NEAR(expected, actual, size, epsilon)                     \
    do {                                                                       \
        for (std::size_t i = 0; i < static_cast<std::size_t>(size); ++i) {     \
            ASSERT_NEAR((expected)[i], (actual)[i], (epsilon))                 \
                << "Arrays differ at index " << i;                             \
        }                                                                      \
    } while (false)

/**
 * @brief Assert arrays are exactly equal
 */
#define ASSERT_ARRAY_EQ(expected, actual, size)                                \
    do {                                                                       \
        for (std::size_t i = 0; i < static_cast<std::size_t>(size); ++i) {     \
            ASSERT_EQ((expected)[i], (actual)[i])                              \
                << "Arrays differ at index " << i;                             \
        }                                                                      \
    } while (false)

/**
 * @brief Mark test as pending/not yet implemented with informative message
 */
#define GTEST_PENDING()                                                        \
    GTEST_SKIP() << "Test pending implementation"

/**
 * @brief Skip test with reason
 */
#define GTEST_SKIP_REASON(reason)                                              \
    GTEST_SKIP() << (reason)

/**
 * @brief Expect exception of specific type with message validation
 */
#define EXPECT_THROW_WITH_MESSAGE(statement, exception_type, expected_msg)     \
    do {                                                                       \
        try {                                                                  \
            statement;                                                         \
            FAIL() << "Expected exception " #exception_type;                   \
        } catch (const exception_type& e) {                                    \
            EXPECT_STREQ(e.what(), expected_msg);                              \
        }                                                                      \
    } while (false)

namespace com::github::doevelopper::atlassians::utdriver
{
    /**
     * @class TestDriverConfig
     * @brief Configuration options for TestDriver
     *
     * Uses the Builder pattern to construct configuration in a fluent manner.
     */
    struct TestDriverConfig
    {
        bool shuffleTests{false};
        bool breakOnFailure{false};
        bool colorOutput{true};
        bool printTimePerTest{true};
        std::uint32_t repeatCount{1};
        std::optional<std::string> filter;
        std::optional<std::string> outputXmlPath;
        bool throwOnFailure{false};
        bool installDeathTestHandler{true};

        /**
         * @brief Builder pattern for TestDriverConfig
         */
        class Builder
        {
        public:
            Builder() = default;

            [[nodiscard]] auto withShuffleTests(bool shuffle) noexcept -> Builder&
            {
                m_shuffleTests = shuffle;
                return *this;
            }

            [[nodiscard]] auto withBreakOnFailure(bool breakOnFail) noexcept -> Builder&
            {
                m_breakOnFailure = breakOnFail;
                return *this;
            }

            [[nodiscard]] auto withColorOutput(bool color) noexcept -> Builder&
            {
                m_colorOutput = color;
                return *this;
            }

            [[nodiscard]] auto withPrintTimePerTest(bool printTime) noexcept -> Builder&
            {
                m_printTimePerTest = printTime;
                return *this;
            }

            [[nodiscard]] auto withRepeatCount(std::uint32_t count) noexcept -> Builder&
            {
                m_repeatCount = count;
                return *this;
            }

            [[nodiscard]] auto withFilter(std::string_view filterExpr) -> Builder&
            {
                m_filter = std::string(filterExpr);
                return *this;
            }

            [[nodiscard]] auto withOutputXml(std::string_view path) -> Builder&
            {
                m_outputXmlPath = std::string(path);
                return *this;
            }

            [[nodiscard]] auto withThrowOnFailure(bool throwOnFail) noexcept -> Builder&
            {
                m_throwOnFailure = throwOnFail;
                return *this;
            }

            [[nodiscard]] auto build() const noexcept -> TestDriverConfig
            {
                TestDriverConfig config;
                config.shuffleTests = m_shuffleTests;
                config.breakOnFailure = m_breakOnFailure;
                config.colorOutput = m_colorOutput;
                config.printTimePerTest = m_printTimePerTest;
                config.repeatCount = m_repeatCount;
                config.filter = m_filter;
                config.outputXmlPath = m_outputXmlPath;
                config.throwOnFailure = m_throwOnFailure;
                return config;
            }

        private:
            bool m_shuffleTests{false};
            bool m_breakOnFailure{false};
            bool m_colorOutput{true};
            bool m_printTimePerTest{true};
            std::uint32_t m_repeatCount{1};
            std::optional<std::string> m_filter;
            std::optional<std::string> m_outputXmlPath;
            bool m_throwOnFailure{false};
        };
    };

    /**
     * @class TestDriver
     * @brief Facade for Google Test framework with enhanced lifecycle management
     *
     * Provides a clean API for:
     * - Test initialization and configuration
     * - Custom environment and listener registration
     * - Test execution with proper error handling
     * - Logging integration throughout the test lifecycle
     *
     * @example
     * int main(int argc, char* argv[]) {
     *     auto config = TestDriverConfig::Builder()
     *         .withColorOutput(true)
     *         .withPrintTimePerTest(true)
     *         .build();
     *
     *     TestDriver driver(config);
     *     return driver.run(argc, argv);
     * }
     */
    class TestDriver
    {
    public:
        /**
         * @brief Result type for test execution
         */
        enum class ExecutionResult : std::int32_t
        {
            Success = 0,
            TestFailure = 1,
            InitializationError = 2,
            ConfigurationError = 3,
            UnknownError = -1
        };

        // ============================================
        // Constructors & Destructor
        // ============================================

        /**
         * @brief Default constructor with default configuration
         */
        TestDriver() noexcept;

        /**
         * @brief Constructor with custom configuration
         * @param config Test driver configuration
         */
        explicit TestDriver(const TestDriverConfig& config) noexcept;

        /**
         * @brief Constructor with suite name and iteration count
         * @param suiteName Name of the test suite
         * @param iterationCount Number of test iterations (default: 1)
         */
        explicit TestDriver(std::string_view suiteName,
                           std::uint32_t iterationCount = 1) noexcept;

        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~TestDriver() noexcept;

        // Disable copy operations (not copyable)
        TestDriver(const TestDriver&) = delete;
        TestDriver& operator=(const TestDriver&) = delete;

        // Enable move operations
        TestDriver(TestDriver&&) noexcept;
        TestDriver& operator=(TestDriver&&) noexcept;

        // ============================================
        // Lifecycle Management
        // ============================================

        /**
         * @brief Initialize the test framework
         * @param argc Command line argument count
         * @param argv Command line arguments
         * @return true if initialization succeeded
         */
        [[nodiscard]] auto setup(int argc, char* argv[]) -> bool;

        /**
         * @brief Run all tests
         * @param argc Command line argument count (default: 0)
         * @param argv Command line arguments (default: nullptr)
         * @return Test execution result code
         */
        [[nodiscard]] auto run(int argc = 0, char* argv[] = nullptr) -> int;

        /**
         * @brief Convenience method combining setup and run
         * @param argc Command line argument count
         * @param argv Command line arguments
         * @return Test execution result code
         */
        [[nodiscard]] auto execute(int argc, char* argv[]) -> int;

        /**
         * @brief Cleanup test framework resources
         * @return true if cleanup succeeded
         */
        [[nodiscard]] auto teardown() noexcept -> bool;

        // ============================================
        // Configuration & State
        // ============================================

        /**
         * @brief Get the module/suite under test name
         * @return String view of the module name
         */
        [[nodiscard]] auto getModuleUnderTest() const noexcept -> std::string_view;

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
        [[nodiscard]] auto getConfig() const noexcept -> const TestDriverConfig&;

        // ============================================
        // Static Utility Methods
        // ============================================

        /**
         * @brief Mark a test as not yet implemented
         * Creates a non-fatal failure indicating pending implementation
         */
        static void notYetImplemented();

        /**
         * @brief Get test statistics summary
         * @return Formatted string with test counts
         */
        [[nodiscard]] static auto getTestSummary() -> std::string;

    protected:
        /**
         * @brief Hook for subclasses to customize environment setup
         * Called before test execution begins
         */
        virtual void onEnvironmentSetup();

        /**
         * @brief Hook for subclasses to customize environment teardown
         * Called after test execution completes
         */
        virtual void onEnvironmentTeardown();

    private:
        /**
         * @brief Extract module name from command line arguments
         * @param argv Command line arguments
         */
        void extractModuleName(char* argv[]);

        /**
         * @brief Apply configuration to Google Test
         */
        void applyConfiguration();

        /**
         * @brief Initialize logging system
         * @return true if logging initialized successfully
         */
        [[nodiscard]] auto initializeLogging() -> bool;

        /**
         * @brief Register custom test environment
         */
        void registerTestEnvironment();

        /**
         * @brief Register custom test event listeners
         */
        void registerEventListeners();

        // Member variables
        TestDriverConfig m_config;
        std::string m_moduleUnderTest;
        bool m_isInitialized{false};
        ExecutionResult m_lastResult{ExecutionResult::Success};

        // Logger instance
        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::atlassians::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTDRIVER_HPP
