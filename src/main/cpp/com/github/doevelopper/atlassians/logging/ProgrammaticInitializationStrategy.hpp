/*******************************************************************
 * @file   ProgrammaticInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Programmatic runtime initialization strategy with fluent API
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROGRAMMATICINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROGRAMMATICINITIALIZATIONSTRATEGY_HPP

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <log4cxx/appender.h>
#include <log4cxx/level.h>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @struct AppenderConfig
     * @brief Configuration for a single appender
     */
    struct AppenderConfig
    {
        enum class Type
        {
            CONSOLE,
            FILE,
            ROLLING_FILE,
            DAILY_ROLLING_FILE
        };

        Type type = Type::CONSOLE;
        std::string name;
        std::string pattern;
        std::filesystem::path filePath;      // For file-based appenders
        std::string maxFileSize;             // For rolling file
        int maxBackupIndex = 5;              // For rolling file
        bool append = true;                  // For file appenders
        std::string target = "System.out";   // For console (System.out or System.err)
    };

    /**
     * @struct LoggerConfig
     * @brief Configuration for a logger
     */
    struct LoggerConfig
    {
        std::string name;                    // Logger name (empty for root)
        log4cxx::LevelPtr level;             // Log level
        bool additivity = true;              // Whether to inherit parent appenders
        std::vector<std::string> appenders;  // Appender names to attach
    };

    /**
     * @class ProgrammaticInitializationStrategy
     * @brief Fluent API for programmatic logging configuration
     *
     * This strategy allows you to configure logging programmatically at runtime
     * using a type-safe, fluent builder interface. It's ideal for applications
     * that need dynamic configuration without external files.
     *
     * Example usage:
     * @code
     * auto strategy = ProgrammaticInitializationStrategy::builder()
     *     .withConsoleAppender("console", "%d{ISO8601} [%p] %c - %m%n")
     *     .withFileAppender("file", "/var/log/app.log", "%d [%t] %-5p %c - %m%n")
     *     .withRollingFileAppender("rolling", "/var/log/app-rolling.log")
     *         .maxFileSize("10MB")
     *         .maxBackupIndex(5)
     *     .setRootLevel(log4cxx::Level::getInfo())
     *     .addLogger("com.mycompany", log4cxx::Level::getDebug())
     *     .build();
     *
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     * @endcode
     */
    class ProgrammaticInitializationStrategy final : public ILoggingInitializationStrategy
    {
    public:
        /**
         * @brief Default console pattern
         */
        static constexpr const char* DEFAULT_PATTERN =
            "%d{yyyy-MM-dd HH:mm:ss.SSS} [%-5p] [%15.15t] %-40c{1.} - %m%n";

        /**
         * @brief Default file pattern
         */
        static constexpr const char* DEFAULT_FILE_PATTERN =
            "%d{yyyy-MM-dd HH:mm:ss.SSS} [%-5p] [%t] %c - %m%n";

        /**
         * @class Builder
         * @brief Fluent builder for ProgrammaticInitializationStrategy
         */
        class Builder
        {
        public:
            /**
             * @brief Add a console appender
             * @param name Appender name
             * @param pattern Optional pattern (uses default if not specified)
             * @return Reference to this builder for chaining
             */
            Builder& withConsoleAppender(
                std::string_view name,
                std::string_view pattern = DEFAULT_PATTERN);

            /**
             * @brief Add a console appender targeting stderr
             * @param name Appender name
             * @param pattern Optional pattern
             * @return Reference to this builder for chaining
             */
            Builder& withStderrAppender(
                std::string_view name,
                std::string_view pattern = DEFAULT_PATTERN);

            /**
             * @brief Add a file appender
             * @param name Appender name
             * @param filePath Path to log file
             * @param pattern Optional pattern
             * @param append Whether to append to existing file
             * @return Reference to this builder for chaining
             */
            Builder& withFileAppender(
                std::string_view name,
                const std::filesystem::path& filePath,
                std::string_view pattern = DEFAULT_FILE_PATTERN,
                bool append = true);

            /**
             * @brief Add a rolling file appender
             * @param name Appender name
             * @param filePath Path to log file
             * @param pattern Optional pattern
             * @return Reference to this builder for chaining
             */
            Builder& withRollingFileAppender(
                std::string_view name,
                const std::filesystem::path& filePath,
                std::string_view pattern = DEFAULT_FILE_PATTERN);

            /**
             * @brief Set max file size for last rolling appender
             * @param size Size string (e.g., "10MB", "100KB")
             * @return Reference to this builder for chaining
             */
            Builder& maxFileSize(std::string_view size);

            /**
             * @brief Set max backup index for last rolling appender
             * @param index Maximum number of backup files
             * @return Reference to this builder for chaining
             */
            Builder& maxBackupIndex(int index);

            /**
             * @brief Set the root logger level
             * @param level The log level
             * @return Reference to this builder for chaining
             */
            Builder& setRootLevel(const log4cxx::LevelPtr& level);

            /**
             * @brief Add a named logger with specific level
             * @param loggerName Logger name (hierarchical, e.g., "com.mycompany.module")
             * @param level Log level for this logger
             * @return Reference to this builder for chaining
             */
            Builder& addLogger(std::string_view loggerName, const log4cxx::LevelPtr& level);

            /**
             * @brief Add a named logger with level and specific appenders
             * @param loggerName Logger name
             * @param level Log level
             * @param appenderNames Appender names to attach
             * @param additivity Whether to inherit parent appenders
             * @return Reference to this builder for chaining
             */
            Builder& addLogger(
                std::string_view loggerName,
                const log4cxx::LevelPtr& level,
                const std::vector<std::string>& appenderNames,
                bool additivity = true);

            /**
             * @brief Build the strategy
             * @return Unique pointer to the configured strategy
             */
            [[nodiscard]] std::unique_ptr<ProgrammaticInitializationStrategy> build();

        private:
            std::vector<AppenderConfig> m_appenders;
            std::vector<LoggerConfig> m_loggers;
            log4cxx::LevelPtr m_rootLevel;
        };

        /**
         * @brief Create a new builder
         * @return Builder instance for fluent configuration
         */
        [[nodiscard]] static Builder builder();

        /**
         * @brief Default constructor (uses default console configuration)
         */
        ProgrammaticInitializationStrategy() noexcept;

        /**
         * @brief Constructor with configuration
         * @param appenders Appender configurations
         * @param loggers Logger configurations
         * @param rootLevel Root logger level
         */
        ProgrammaticInitializationStrategy(
            std::vector<AppenderConfig> appenders,
            std::vector<LoggerConfig> loggers,
            log4cxx::LevelPtr rootLevel);

        /**
         * @brief Copy constructor (deleted)
         */
        ProgrammaticInitializationStrategy(const ProgrammaticInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        ProgrammaticInitializationStrategy(ProgrammaticInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment (deleted)
         */
        ProgrammaticInitializationStrategy& operator=(const ProgrammaticInitializationStrategy&) = delete;

        /**
         * @brief Move assignment
         */
        ProgrammaticInitializationStrategy& operator=(ProgrammaticInitializationStrategy&&) noexcept = default;

        /**
         * @brief Destructor
         */
        ~ProgrammaticInitializationStrategy() noexcept override;

        /**
         * @brief Initialize programmatically with configured appenders and loggers
         * @throws LoggingInitializationException if configuration fails
         */
        void initialize() const override;

        /**
         * @brief Validate the configuration
         * @return true if configuration is valid
         */
        [[nodiscard]] bool validate() const noexcept override;

        /**
         * @brief Get the configuration type
         * @return ConfigType::PROGRAMMATIC
         */
        [[nodiscard]] ConfigType getConfigType() const noexcept override;

        /**
         * @brief Get the strategy name
         * @return "ProgrammaticInitializationStrategy"
         */
        [[nodiscard]] std::string_view getStrategyName() const noexcept override;

        /**
         * @brief Get the configuration source description
         * @return Description of the programmatic configuration
         */
        [[nodiscard]] std::optional<std::string> getConfigSource() const noexcept override;

    private:
        /**
         * @brief Create an appender from configuration
         */
        [[nodiscard]] log4cxx::AppenderPtr createAppender(const AppenderConfig& config) const;

        std::vector<AppenderConfig> m_appenders;
        std::vector<LoggerConfig> m_loggers;
        log4cxx::LevelPtr m_rootLevel;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROGRAMMATICINITIALIZATIONSTRATEGY_HPP
