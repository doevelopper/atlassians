/*******************************************************************
 * @file   EnvironmentBasedInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Environment variable based initialization strategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENVIRONMENTBASEDINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENVIRONMENTBASEDINITIALIZATIONSTRATEGY_HPP

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class EnvironmentBasedInitializationStrategy
     * @brief Initialization strategy that reads configuration from environment variables
     *
     * This strategy checks for environment variables containing the path to a
     * logging configuration file. It supports multiple environment variable names
     * with priority ordering and can fall back to default configuration if no
     * valid environment variable is found.
     *
     * Environment Variables (checked in order):
     * 1. LOG4CXX_CONFIGURATION - Primary configuration file path
     * 2. LOG4CXX_CONFIG - Alternative configuration file path
     * 3. LOG4CXX_CONFIG_FILE - Another alternative
     *
     * The strategy automatically detects whether the configuration file is
     * XML or properties format based on the file extension.
     *
     * Example usage:
     * @code
     * // Set environment variable: export LOG4CXX_CONFIGURATION=/etc/myapp/log4cxx.xml
     * auto strategy = std::make_unique<EnvironmentBasedInitializationStrategy>();
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     * @endcode
     */
    class EnvironmentBasedInitializationStrategy final : public ILoggingInitializationStrategy
    {
    public:
        /**
         * @brief Primary environment variable name
         */
        static constexpr const char* ENV_VAR_PRIMARY = "LOG4CXX_CONFIGURATION";

        /**
         * @brief Secondary environment variable name
         */
        static constexpr const char* ENV_VAR_SECONDARY = "LOG4CXX_CONFIG";

        /**
         * @brief Tertiary environment variable name
         */
        static constexpr const char* ENV_VAR_TERTIARY = "LOG4CXX_CONFIG_FILE";

        /**
         * @brief Default constructor with fallback enabled
         */
        EnvironmentBasedInitializationStrategy() noexcept;

        /**
         * @brief Constructor with custom primary environment variable
         * @param envVarName Name of the primary environment variable to check
         * @param fallbackToBasic If true, use BasicConfigurator when env var not set
         */
        explicit EnvironmentBasedInitializationStrategy(
            std::string_view envVarName,
            bool fallbackToBasic = true);

        /**
         * @brief Copy constructor (deleted)
         */
        EnvironmentBasedInitializationStrategy(const EnvironmentBasedInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        EnvironmentBasedInitializationStrategy(EnvironmentBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment (deleted)
         */
        EnvironmentBasedInitializationStrategy& operator=(const EnvironmentBasedInitializationStrategy&) = delete;

        /**
         * @brief Move assignment
         */
        EnvironmentBasedInitializationStrategy& operator=(EnvironmentBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Destructor
         */
        ~EnvironmentBasedInitializationStrategy() noexcept override;

        /**
         * @brief Initialize based on environment variable configuration
         * @throws LoggingInitializationException if configuration fails
         */
        void initialize() const override;

        /**
         * @brief Validate the environment configuration
         * @return true if a valid environment variable is set or fallback is enabled
         */
        [[nodiscard]] bool validate() const noexcept override;

        /**
         * @brief Get the configuration type
         * @return ConfigType::ENVIRONMENT
         */
        [[nodiscard]] ConfigType getConfigType() const noexcept override;

        /**
         * @brief Get the strategy name
         * @return "EnvironmentBasedInitializationStrategy"
         */
        [[nodiscard]] std::string_view getStrategyName() const noexcept override;

        /**
         * @brief Get the resolved configuration source
         * @return The resolved file path from environment, or description of fallback
         */
        [[nodiscard]] std::optional<std::string> getConfigSource() const noexcept override;

        /**
         * @brief Set the primary environment variable name
         * @param envVarName Name of the environment variable
         */
        void setPrimaryEnvVar(std::string_view envVarName);

        /**
         * @brief Enable or disable fallback to basic configuration
         * @param enable If true, use BasicConfigurator when no valid env var found
         */
        void setFallbackEnabled(bool enable) noexcept;

        /**
         * @brief Check if fallback is enabled
         * @return true if fallback to basic configuration is enabled
         */
        [[nodiscard]] bool isFallbackEnabled() const noexcept;

        /**
         * @brief Get the resolved configuration file path (if any)
         * @return Optional path to the configuration file from environment
         */
        [[nodiscard]] std::optional<std::filesystem::path> getResolvedPath() const noexcept;

    private:
        /**
         * @brief Find the first valid configuration path from environment
         * @return Optional path if found
         */
        [[nodiscard]] std::optional<std::filesystem::path> findConfigPath() const noexcept;

        /**
         * @brief Check if file is accessible
         * @param path Path to check
         * @return true if file exists and is readable
         */
        [[nodiscard]] static bool isFileAccessible(const std::filesystem::path& path) noexcept;

        /**
         * @brief Determine configuration type from file extension
         * @param path Path to analyze
         * @return true if XML, false if properties
         */
        [[nodiscard]] static bool isXmlConfig(const std::filesystem::path& path) noexcept;

        std::string m_primaryEnvVar;
        bool m_fallbackToBasic;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_ENVIRONMENTBASEDINITIALIZATIONSTRATEGY_HPP
