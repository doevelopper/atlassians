/*******************************************************************
 * @file   ILoggingInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Strategy interface for Log4CXX initialization
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ILOGGINGINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ILOGGINGINITIALIZATIONSTRATEGY_HPP

#include <cstdint>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/API_Export.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class ILoggingInitializationStrategy
     * @brief Pure abstract interface for Log4CXX initialization strategies.
     *
     * This interface follows the Strategy pattern, allowing different initialization
     * approaches to be used interchangeably. Implementations include:
     * - DefaultInitializationStrategy: Basic console configuration
     * - PropertyFileBasedInitializationStrategy: .properties file configuration
     * - XMLFileBasedInitializationStrategy: XML file configuration
     * - EnvironmentBasedInitializationStrategy: Environment variable based configuration
     * - ProgrammaticInitializationStrategy: Runtime fluent API configuration
     *
     * Design principles:
     * - Single Responsibility: Only handles initialization strategy
     * - Interface Segregation: Minimal focused interface
     * - Liskov Substitution: All strategies are interchangeable
     */
    class SDLC_API ILoggingInitializationStrategy
    {
    public:
        /**
         * @enum ConfigType
         * @brief Specifies the type of configuration source used for initialization.
         */
        enum class ConfigType : std::uint8_t
        {
            DEFAULT     = 0,  ///< Basic default configuration (console appender)
            PROPERTY    = 1,  ///< Property file configuration (e.g., log4cxx.properties)
            XML         = 2,  ///< XML file configuration (e.g., log4cxx.xml)
            ENVIRONMENT = 3,  ///< Environment variable based configuration
            PROGRAMMATIC= 4,  ///< Programmatic runtime configuration
            CUSTOM      = 5   ///< Custom user-defined configuration
        };

        /**
         * @brief Default constructor
         */
        ILoggingInitializationStrategy() noexcept = default;

        /**
         * @brief Copy constructor (deleted - strategies should not be copied)
         */
        ILoggingInitializationStrategy(const ILoggingInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        ILoggingInitializationStrategy(ILoggingInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment operator (deleted)
         */
        ILoggingInitializationStrategy& operator=(const ILoggingInitializationStrategy&) = delete;

        /**
         * @brief Move assignment operator
         */
        ILoggingInitializationStrategy& operator=(ILoggingInitializationStrategy&&) noexcept = default;

        /**
         * @brief Virtual destructor for proper polymorphic destruction
         */
        virtual ~ILoggingInitializationStrategy() noexcept = default;

        /**
         * @brief Initialize the logging system with this strategy
         * @throws std::runtime_error if initialization fails
         * @throws std::invalid_argument if configuration is invalid
         */
        virtual void initialize() const = 0;

        /**
         * @brief Validate the configuration before initialization
         * @return true if configuration is valid and can be applied
         */
        [[nodiscard]] virtual bool validate() const noexcept
        {
            return true;
        }

        /**
         * @brief Get the configuration type this strategy represents
         * @return The ConfigType enum value for this strategy
         */
        [[nodiscard]] virtual ConfigType getConfigType() const noexcept = 0;

        /**
         * @brief Get a human-readable name for this strategy
         * @return String view containing the strategy name
         */
        [[nodiscard]] virtual std::string_view getStrategyName() const noexcept = 0;

        /**
         * @brief Get optional configuration source description (e.g., file path)
         * @return Optional string containing source description, or nullopt if not applicable
         */
        [[nodiscard]] virtual std::optional<std::string> getConfigSource() const noexcept
        {
            return std::nullopt;
        }

    protected:
        /**
         * @brief Helper method to check if logging is already configured
         * @return true if Log4CXX repository is already configured
         */
        [[nodiscard]] bool isAlreadyConfigured() const noexcept;

        /**
         * @brief Mark the repository as configured after successful initialization
         */
        void markAsConfigured() const;
    };

    /**
     * @brief Exception class for logging initialization errors
     */
    class LoggingInitializationException : public std::runtime_error
    {
    public:
        explicit LoggingInitializationException(const std::string& message)
            : std::runtime_error(message)
        {
        }

        explicit LoggingInitializationException(const char* message)
            : std::runtime_error(message)
        {
        }

        LoggingInitializationException(const std::string& message, const std::exception& cause)
            : std::runtime_error(message + ": " + cause.what())
        {
        }
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ILOGGINGINITIALIZATIONSTRATEGY_HPP
