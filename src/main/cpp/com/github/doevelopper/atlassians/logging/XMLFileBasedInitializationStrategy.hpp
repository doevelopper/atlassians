/*******************************************************************
 * @file   XMLFileBasedInitializationStrategy.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    XML file based initialization strategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_XMLFILEBASEDINITIALIZATIONSTRATEGY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_XMLFILEBASEDINITIALIZATIONSTRATEGY_HPP

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @class XMLFileBasedInitializationStrategy
     * @brief Initialization strategy using log4cxx XML configuration files
     *
     * This strategy loads logging configuration from a log4cxx XML configuration file.
     * It uses DOMConfigurator for parsing and supports file watching for automatic
     * reconfiguration when the file changes.
     *
     * Features:
     * - Validates file existence, readability, and XML extension
     * - Optional file watching with configurable interval
     * - Supports standard log4cxx XML schema
     *
     * Example usage:
     * @code
     * auto strategy = std::make_unique<XMLFileBasedInitializationStrategy>(
     *     "/etc/myapp/log4cxx.xml");
     * strategy->setWatchInterval(std::chrono::seconds(30));
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     * @endcode
     */
    class XMLFileBasedInitializationStrategy final : public ILoggingInitializationStrategy
    {
    public:
        /**
         * @brief Default watch interval (60 seconds)
         */
        static constexpr std::chrono::milliseconds DEFAULT_WATCH_INTERVAL{60000};

        /**
         * @brief Default constructor (requires setConfigFilePath before use)
         */
        XMLFileBasedInitializationStrategy() noexcept;

        /**
         * @brief Constructor with file path
         * @param filePath Path to the XML configuration file
         */
        explicit XMLFileBasedInitializationStrategy(const std::filesystem::path& filePath);

        /**
         * @brief Constructor with file path as string
         * @param filePath Path to the XML configuration file
         */
        explicit XMLFileBasedInitializationStrategy(std::string_view filePath);

        /**
         * @brief Copy constructor (deleted)
         */
        XMLFileBasedInitializationStrategy(const XMLFileBasedInitializationStrategy&) = delete;

        /**
         * @brief Move constructor
         */
        XMLFileBasedInitializationStrategy(XMLFileBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Copy assignment (deleted)
         */
        XMLFileBasedInitializationStrategy& operator=(const XMLFileBasedInitializationStrategy&) = delete;

        /**
         * @brief Move assignment
         */
        XMLFileBasedInitializationStrategy& operator=(XMLFileBasedInitializationStrategy&&) noexcept = default;

        /**
         * @brief Destructor
         */
        ~XMLFileBasedInitializationStrategy() noexcept override;

        /**
         * @brief Initialize from an XML configuration file
         * @throws LoggingInitializationException if file not found or configuration fails
         */
        void initialize() const override;

        /**
         * @brief Validate the configuration file
         * @return true if file exists, is readable, and has XML extension
         */
        [[nodiscard]] bool validate() const noexcept override;

        /**
         * @brief Get the configuration type
         * @return ConfigType::XML
         */
        [[nodiscard]] ConfigType getConfigType() const noexcept override;

        /**
         * @brief Get the strategy name
         * @return "XMLFileBasedInitializationStrategy"
         */
        [[nodiscard]] std::string_view getStrategyName() const noexcept override;

        /**
         * @brief Get the configuration source (file path)
         * @return The configured file path
         */
        [[nodiscard]] std::optional<std::string> getConfigSource() const noexcept override;

        /**
         * @brief Set the configuration file path
         * @param filePath Path to the XML file
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

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_XMLFILEBASEDINITIALIZATIONSTRATEGY_HPP
