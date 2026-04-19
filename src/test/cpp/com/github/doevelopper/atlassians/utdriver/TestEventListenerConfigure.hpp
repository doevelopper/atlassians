/*******************************************************************
 * @file    TestEventListenerConfigure.hpp
 * @version v1.0.0
 * @date    2024/12/30
 * @brief   Configurable Google Test Event Listener with Builder pattern
 *
 * This class provides a flexible way to configure which test events
 * are passed through to a delegate listener (typically the default printer).
 * It implements the Decorator pattern combined with Builder for configuration.
 *
 * Design Patterns:
 * - Decorator: Wraps another listener to add filtering behavior
 * - Builder: Fluent API for constructing configured listeners
 * - Delegate: Forwards selected events to wrapped listener
 *
 * Benefits:
 * - Fine-grained control over test output verbosity
 * - No code changes needed to adjust output
 * - Composable with other listeners
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENERCONFIGURE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENERCONFIGURE_HPP

#include <cstdint>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include <log4cxx/logger.h>

namespace com::github::doevelopper::atlassians::utdriver
{
    /**
     * @class TestEventListenerConfigure
     * @brief Configurable decorator for Google Test event listeners
     *
     * Wraps a delegate listener (e.g., default_result_printer) and
     * selectively forwards events based on configuration flags.
     *
     * @example
     * auto* listener = TestEventListenerConfigure::Delegate(defaultPrinter)
     *     .showProgramStartEnd()
     *     .showTestSuite()
     *     .showTestNames()
     *     .showFailures()
     *     .build();
     * listeners.Append(listener);
     */
    class TestEventListenerConfigure final : public ::testing::TestEventListener
    {
    public:
        /**
         * @struct Options
         * @brief Configuration options for event filtering
         */
        struct Options
        {
            bool programStartEnd{false};
            bool iterationsStartEnd{false};
            bool testCases{false};
            bool testNames{false};
            bool testSuite{false};
            bool successes{false};
            bool failures{false};
            bool inlineFailures{false};
            bool environment{false};
            bool skip{false};

            /**
             * @brief Enable all output options
             * @return Options with all flags set to true
             */
            [[nodiscard]] static constexpr auto all() noexcept -> Options
            {
                return Options{true, true, true, true, true, true, true, true, true, true};
            }

            /**
             * @brief Minimal output (failures only)
             * @return Options for minimal output
             */
            [[nodiscard]] static constexpr auto minimal() noexcept -> Options
            {
                return Options{true, false, false, false, false, false, true, true, false, false};
            }

            /**
             * @brief Standard output (common settings)
             * @return Options for standard output
             */
            [[nodiscard]] static constexpr auto standard() noexcept -> Options
            {
                return Options{true, true, false, true, true, true, true, true, false, true};
            }

            /**
             * @brief Verbose output (everything except inlineFailures)
             * @return Options for verbose output
             */
            [[nodiscard]] static constexpr auto verbose() noexcept -> Options
            {
                return Options{true, true, true, true, true, true, true, false, true, true};
            }
        };

        /**
         * @class Delegate
         * @brief Builder class for constructing TestEventListenerConfigure
         *
         * Provides a fluent API for configuring which events to display.
         *
         * @example
         * auto* listener = TestEventListenerConfigure::Delegate(printer)
         *     .showTestNames()
         *     .showFailures()
         *     .build();
         */
        class Delegate
        {
        public:
            /**
             * @brief Delete default constructor
             */
            Delegate() = delete;

            /**
             * @brief Constructor with delegate listener
             * @param eventListener The listener to wrap (takes ownership)
             */
            explicit Delegate(::testing::TestEventListener* eventListener);

            /**
             * @brief Constructor with preset options
             * @param eventListener The listener to wrap
             * @param preset Preset configuration options
             */
            Delegate(::testing::TestEventListener* eventListener, const Options& preset);

            /**
             * @brief Destructor
             */
            virtual ~Delegate() noexcept;

            // Non-copyable
            Delegate(const Delegate&) = delete;
            Delegate& operator=(const Delegate&) = delete;

            // Movable
            Delegate(Delegate&&) noexcept = default;
            Delegate& operator=(Delegate&&) noexcept = default;

            // ============================================
            // Fluent Configuration Methods
            // ============================================

            /**
             * @brief Enable program start/end messages
             * @return Reference to this builder for chaining
             */
            [[nodiscard]] auto showProgramStartEnd() noexcept -> Delegate&;

            /**
             * @brief Enable iteration start/end messages
             */
            [[nodiscard]] auto showIterationsStartEnd() noexcept -> Delegate&;

            /**
             * @brief Enable test case messages
             */
            [[nodiscard]] auto showTestCases() noexcept -> Delegate&;

            /**
             * @brief Enable individual test name messages
             */
            [[nodiscard]] auto showTestNames() noexcept -> Delegate&;

            /**
             * @brief Enable test suite messages
             */
            [[nodiscard]] auto showTestSuite() noexcept -> Delegate&;

            /**
             * @brief Enable success messages
             */
            [[nodiscard]] auto showSuccesses() noexcept -> Delegate&;

            /**
             * @brief Enable failure messages
             */
            [[nodiscard]] auto showFailures() noexcept -> Delegate&;

            /**
             * @brief Enable inline failure details
             */
            [[nodiscard]] auto showInlineFailures() noexcept -> Delegate&;

            /**
             * @brief Enable environment messages
             */
            [[nodiscard]] auto showEnvironment() noexcept -> Delegate&;

            /**
             * @brief Enable skip messages
             */
            [[nodiscard]] auto showSkip() noexcept -> Delegate&;

            /**
             * @brief Apply a preset configuration
             * @param preset The preset to apply
             */
            [[nodiscard]] auto withPreset(const Options& preset) noexcept -> Delegate&;

            /**
             * @brief Enable all output options
             */
            [[nodiscard]] auto showAll() noexcept -> Delegate&;

            /**
             * @brief Build the configured listener
             * @return Pointer to the configured listener (caller owns)
             */
            [[nodiscard]] auto build() -> TestEventListenerConfigure*;

            /**
             * @brief Build the configured listener as unique_ptr
             * @return unique_ptr to the configured listener
             */
            [[nodiscard]] auto buildUnique() -> std::unique_ptr<TestEventListenerConfigure>;

        private:
            ::testing::TestEventListener* m_delegateEventListener;
            Options m_options;

            static log4cxx::LoggerPtr logger;
        };

        // ============================================
        // Constructors & Destructor
        // ============================================

        /**
         * @brief Delete default constructor
         */
        TestEventListenerConfigure() = delete;

        /**
         * @brief Virtual destructor
         */
        ~TestEventListenerConfigure() noexcept override;

        // Non-copyable
        TestEventListenerConfigure(const TestEventListenerConfigure&) = delete;
        TestEventListenerConfigure& operator=(const TestEventListenerConfigure&) = delete;

        // Non-movable (owned by Google Test)
        TestEventListenerConfigure(TestEventListenerConfigure&&) = delete;
        TestEventListenerConfigure& operator=(TestEventListenerConfigure&&) = delete;

        // ============================================
        // TestEventListener Interface
        // ============================================

        void OnTestProgramStart(const ::testing::UnitTest& unit_test) override;
        void OnTestIterationStart(const ::testing::UnitTest& unit_test, int iteration) override;
        void OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test) override;
        void OnEnvironmentsSetUpEnd(const ::testing::UnitTest& unit_test) override;
        void OnTestSuiteStart(const ::testing::TestSuite& test_suite) override;
        void OnTestCaseStart(const ::testing::TestCase& test_case) override;
        void OnTestStart(const ::testing::TestInfo& test_info) override;
        void OnTestDisabled(const ::testing::TestInfo& test_info) override;
        void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override;
        void OnTestEnd(const ::testing::TestInfo& test_info) override;
        void OnTestSuiteEnd(const ::testing::TestSuite& test_suite) override;
        void OnTestCaseEnd(const ::testing::TestCase& test_case) override;
        void OnEnvironmentsTearDownStart(const ::testing::UnitTest& unit_test) override;
        void OnEnvironmentsTearDownEnd(const ::testing::UnitTest& unit_test) override;
        void OnTestIterationEnd(const ::testing::UnitTest& unit_test, int iteration) override;
        void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override;

        // ============================================
        // Accessors
        // ============================================

        /**
         * @brief Get the current options
         * @return Reference to options
         */
        [[nodiscard]] auto getOptions() const noexcept -> const Options&;

    private:
        /**
         * @brief Private constructor used by Delegate builder
         */
        TestEventListenerConfigure(::testing::TestEventListener* eventListener,
                                   const Options& options);

        std::unique_ptr<::testing::TestEventListener> m_eventListener;
        Options m_options;

        static log4cxx::LoggerPtr logger;
    };

}  // namespace com::github::doevelopper::atlassians::utdriver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_UTDRIVER_TESTEVENTLISTENERCONFIGURE_HPP
