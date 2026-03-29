/*******************************************************************
 * @file   PropertyFileBasedInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Property file based initialization strategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROPERTYFILEBASEDINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROPERTYFILEBASEDINITIALIZATIONSTRATEGY_HPP

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class PropertyFileBasedInitializationStrategy
     * @brief Initialization strategy using log4cxx .properties configuration files
     *
     * This strategy loads logging configuration from a standard log4cxx properties file.
     * It supports file watching for automatic reconfiguration when the file changes.
     *
     * Features:
     * - Validates file existence and readability before initialization
     * - Optional file watching with configurable interval
     * - Thread-safe configuration updates
     *
     * Example usage:
     * @code
     * auto strategy = std::make_unique<PropertyFileBasedInitializationStrategy>(
     *     "/etc/myapp/log4cxx.properties");
     * strategy->setWatchInterval(std::chrono::seconds(30));
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     * @endcode
     */
    class PropertyFileBasedInitializationStrategy final : public ILoggingInitializationStrategy
    {
    public:
        /**
         * @brief Default watch interval (60 seconds)
         */
        static constexpr std::chrono::milliseconds DEFAULT_WATCH_INTERVAL{60000};

        /**
         * @brief Default constructor (requires setConfigFilePath before use)
         */
        PropertyFileBasedInitializationStrategy() noexcept;

        /**
         * @brief Constructor with file path
         * @param filePath Path to the .properties configuration file
         */
        explicit PropertyFileBasedInitializationStrategy(const std::filesystem::path& filePath);

        /**
         * @brief Constructor with file path as string
         * @param filePath Path to the .properties configuration file
         */
        explicit PropertyFileBasedInitializationStrategy(std::string_view filePath);

        /**
         * @brief Copy constructor (deleted)
         */
        PropertyFileBasedInitializationStrategy(const PropertyFileBasedInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        PropertyFileBasedInitializationStrategy(PropertyFileBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment (deleted)
         */
        PropertyFileBasedInitializationStrategy& operator=(const PropertyFileBasedInitializationStrategy&) = delete;

        /**
         * @brief Move assignment
         */
        PropertyFileBasedInitializationStrategy& operator=(PropertyFileBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Destructor
         */
        ~PropertyFileBasedInitializationStrategy() noexcept override;

        /**
         * @brief Initialize from a properties file
         * @throws LoggingInitializationException if file not found or configuration fails
         */
        void initialize() const override;

        /**
         * @brief Validate the configuration file
         * @return true if file exists, is readable, and has .properties extension
         */
        [[nodiscard]] bool validate() const noexcept override;

        /**
         * @brief Get the configuration type
         * @return ConfigType::PROPERTY
         */
        [[nodiscard]] ConfigType getConfigType() const noexcept override;

        /**
         * @brief Get the strategy name
         * @return "PropertyFileBasedInitializationStrategy"
         */
        [[nodiscard]] std::string_view getStrategyName() const noexcept override;

        /**
         * @brief Get the configuration source (file path)
         * @return The configured file path
         */
        [[nodiscard]] std::optional<std::string> getConfigSource() const noexcept override;

        /**
         * @brief Set the configuration file path
         * @param filePath Path to the .properties file
         */
        void setConfigFilePath(const std::filesystem::path& filePath);

        /**
         * @brief Get the configured file path
         * @return The file path
         */
        [[nodiscard]] const std::filesystem::path& getConfigFilePath() const noexcept;

        /**
         * @brief Enable or disable file watching
         * @param enable true to enable file watching
         */
        void setWatchEnabled(bool enable) noexcept;

        /**
         * @brief Check if file watching is enabled
         * @return true if file watching is enabled
         */
        [[nodiscard]] bool isWatchEnabled() const noexcept;

        /**
         * @brief Set the file watch interval
         * @param interval Time between file checks
         */
        void setWatchInterval(std::chrono::milliseconds interval) noexcept;

        /**
         * @brief Get the current watch interval
         * @return Current watch interval
         */
        [[nodiscard]] std::chrono::milliseconds getWatchInterval() const noexcept;

    private:
        /**
         * @brief Check if file exists and is readable
         * @return true if file is accessible
         */
        [[nodiscard]] bool isFileAccessible() const noexcept;

        std::filesystem::path m_configFilePath;
        bool m_watchEnabled;
        std::chrono::milliseconds m_watchInterval;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_PROPERTYFILEBASEDINITIALIZATIONSTRATEGY_HPP
