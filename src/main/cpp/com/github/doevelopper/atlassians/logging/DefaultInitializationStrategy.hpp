/*******************************************************************
 * @file   DefaultInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Default/fallback initialization strategy using BasicConfigurator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEFAULTINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEFAULTINITIALIZATIONSTRATEGY_HPP

#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class DefaultInitializationStrategy
     * @brief Default initialization strategy using log4cxx BasicConfigurator
     *
     * This strategy provides a simple console-based logging configuration suitable
     * for development, testing, or as a fallback when no specific configuration is needed.
     *
     * Configuration:
     * - Appender: Console (stdout)
     * - Layout: PatternLayout with timestamp, level, thread, and message
     * - Default Level: INFO (configurable)
     *
     * Example usage:
     * @code
     * auto strategy = std::make_unique<DefaultInitializationStrategy>();
     * strategy->setDefaultLevel(LogLevel::DEBUG);
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     * @endcode
     */
    class DefaultInitializationStrategy final : public ILoggingInitializationStrategy
    {
    public:
        /**
         * @brief Default pattern for log output
         */
        static constexpr const char* DEFAULT_PATTERN =
                "%d{yyyy-MM-dd HH:mm:ss.SSS} %Y [%-6p] %y - [%15.15t] - %-35c{1.} -- %Y %m%y%n";
            // "%d{yyyy-MM-dd HH:mm:ss.SSS} [%-5p] [%15.15t] %-40c{1.} - %m%n";

        /**
         * @brief Default constructor with INFO log level
         */
        DefaultInitializationStrategy() noexcept;

        /**
         * @brief Constructor with custom log level
         * @param level The default log level to use
         */
        explicit DefaultInitializationStrategy(std::string_view pattern) noexcept;

        /**
         * @brief Copy constructor (deleted)
         */
        DefaultInitializationStrategy(const DefaultInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        DefaultInitializationStrategy(DefaultInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment (deleted)
         */
        DefaultInitializationStrategy& operator=(const DefaultInitializationStrategy&) = delete;

        /**
         * @brief Move assignment
         */
        DefaultInitializationStrategy& operator=(DefaultInitializationStrategy&&) noexcept = default;

        /**
         * @brief Destructor
         */
        ~DefaultInitializationStrategy() noexcept override;

        /**
         * @brief Initialize with BasicConfigurator (simple console output)
         * @throws LoggingInitializationException if configuration fails
         */
        void initialize() const override;

        /**
         * @brief Validate configuration (always valid for default strategy)
         * @return Always returns true
         */
        [[nodiscard]] bool validate() const noexcept override;

        /**
         * @brief Get the configuration type
         * @return ConfigType::DEFAULT
         */
        [[nodiscard]] ConfigType getConfigType() const noexcept override;

        /**
         * @brief Get the strategy name
         * @return "DefaultInitializationStrategy"
         */
        [[nodiscard]] std::string_view getStrategyName() const noexcept override;

        /**
         * @brief Set whether to use trace level (most verbose)
         * @param useTrace If true, sets root logger to TRACE level
         */
        void setUseTraceLevel(bool useTrace) noexcept;

        /**
         * @brief Set custom pattern layout
         * @param pattern The pattern string for log output formatting
         */
        void setPattern(std::string_view pattern);

    private:
        std::string m_pattern;
        bool m_useTraceLevel;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_DEFAULTINITIALIZATIONSTRATEGY_HPP
