/*******************************************************************
 * @file   LoggingInitializationFactory.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Factory for creating initialization strategies
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZATIONFACTORY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZATIONFACTORY_HPP

#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    // Forward declarations
    class DefaultInitializationStrategy;
    class PropertyFileBasedInitializationStrategy;
    class XMLFileBasedInitializationStrategy;
    class EnvironmentBasedInitializationStrategy;
    class ProgrammaticInitializationStrategy;

    /**
     * @enum InitializationStrategyType
     * @brief Enumeration of available initialization strategy types
     */
    enum class InitializationStrategyType
    {
        Default,      ///< BasicConfigurator with console output
        PropertyFile, ///< .properties file based configuration
        XMLFile,      ///< XML file based configuration
        Environment,  ///< Environment variable based configuration
        Programmatic, ///< Programmatic runtime configuration
        Custom        ///< User-registered custom strategy
    };

    /**
     * @brief Convert strategy type to string
     */
    [[nodiscard]] constexpr std::string_view toString(InitializationStrategyType type) noexcept
    {
        switch (type)
        {
            case InitializationStrategyType::Default:
                return "Default";
            case InitializationStrategyType::PropertyFile:
                return "PropertyFile";
            case InitializationStrategyType::XMLFile:
                return "XMLFile";
            case InitializationStrategyType::Environment:
                return "Environment";
            case InitializationStrategyType::Programmatic:
                return "Programmatic";
            case InitializationStrategyType::Custom:
                return "Custom";
        }
        return "Unknown";
    }

    /**
     * @class LoggingInitializationFactory
     * @brief Factory for creating and managing initialization strategies
     *
     * This factory provides:
     * - Creation of built-in strategy types
     * - Registration of custom strategies
     * - Auto-detection of appropriate strategy based on environment
     * - Caching and reuse of strategy configurations
     *
     * Example usage:
     * @code
     * // Create a specific strategy type
     * auto strategy = LoggingInitializationFactory::create(
     *     InitializationStrategyType::PropertyFile,
     *     "/etc/myapp/log4cxx.properties");
     *
     * // Auto-detect best strategy
     * auto detectedStrategy = LoggingInitializationFactory::createAutoDetected();
     *
     * // Register custom strategy
     * LoggingInitializationFactory::registerStrategy(
     *     "my-custom",
     *     []() { return std::make_unique<MyCustomStrategy>(); });
     *
     * auto customStrategy = LoggingInitializationFactory::createCustom("my-custom");
     * @endcode
     */
    class LoggingInitializationFactory
    {
    public:
        /**
         * @brief Strategy creator function type
         */
        using StrategyCreator = std::function<std::unique_ptr<ILoggingInitializationStrategy>()>;

        /**
         * @brief Strategy creator with path function type
         */
        using StrategyCreatorWithPath =
            std::function<std::unique_ptr<ILoggingInitializationStrategy>(const std::filesystem::path&)>;

        /**
         * @brief Delete constructor (static factory class)
         */
        LoggingInitializationFactory() = delete;

        /**
         * @brief Delete copy constructor
         */
        LoggingInitializationFactory(const LoggingInitializationFactory&) = delete;

        /**
         * @brief Delete copy assignment
         */
        LoggingInitializationFactory& operator=(const LoggingInitializationFactory&) = delete;

        /**
         * @brief Create a strategy of the specified type
         * @param type The strategy type to create
         * @param configPath Optional configuration file path (for file-based strategies)
         * @return Unique pointer to the created strategy
         * @throws std::invalid_argument if type is Custom without registration
         */
        [[nodiscard]] static std::unique_ptr<ILoggingInitializationStrategy> create(
            InitializationStrategyType type,
            const std::filesystem::path& configPath = {});

        /**
         * @brief Create a strategy with string-based type
         * @param typeName Strategy type name (e.g., "Default", "PropertyFile")
         * @param configPath Optional configuration file path
         * @return Unique pointer to the created strategy
         */
        [[nodiscard]] static std::unique_ptr<ILoggingInitializationStrategy> create(
            std::string_view typeName,
            const std::filesystem::path& configPath = {});

        /**
         * @brief Auto-detect and create the most appropriate strategy
         *
         * Detection order:
         * 1. Check environment variables for configuration path
         * 2. Look for log4cxx.xml in current directory
         * 3. Look for log4cxx.properties in current directory
         * 4. Fall back to default configuration
         *
         * @return Unique pointer to the detected strategy
         */
        [[nodiscard]] static std::unique_ptr<ILoggingInitializationStrategy> createAutoDetected();

        /**
         * @brief Detect the most appropriate strategy type
         * @return The detected strategy type
         */
        [[nodiscard]] static InitializationStrategyType detectStrategyType();

        /**
         * @brief Register a custom strategy creator
         * @param name Unique name for the custom strategy
         * @param creator Function that creates the strategy
         */
        static void registerStrategy(std::string_view name, StrategyCreator creator);

        /**
         * @brief Register a custom strategy creator with path parameter
         * @param name Unique name for the custom strategy
         * @param creator Function that creates the strategy with a path
         */
        static void registerStrategyWithPath(std::string_view name, StrategyCreatorWithPath creator);

        /**
         * @brief Unregister a custom strategy
         * @param name Name of the strategy to unregister
         * @return true if strategy was found and removed
         */
        static bool unregisterStrategy(std::string_view name);

        /**
         * @brief Create a custom registered strategy
         * @param name Name of the registered strategy
         * @return Unique pointer to the created strategy
         * @throws std::invalid_argument if strategy is not registered
         */
        [[nodiscard]] static std::unique_ptr<ILoggingInitializationStrategy> createCustom(
            std::string_view name);

        /**
         * @brief Create a custom registered strategy with path
         * @param name Name of the registered strategy
         * @param configPath Configuration path to pass to creator
         * @return Unique pointer to the created strategy
         * @throws std::invalid_argument if strategy is not registered
         */
        [[nodiscard]] static std::unique_ptr<ILoggingInitializationStrategy> createCustom(
            std::string_view name,
            const std::filesystem::path& configPath);

        /**
         * @brief Check if a custom strategy is registered
         * @param name Strategy name
         * @return true if strategy is registered
         */
        [[nodiscard]] static bool isStrategyRegistered(std::string_view name);

        /**
         * @brief Get list of registered custom strategy names
         * @return Vector of registered strategy names
         */
        [[nodiscard]] static std::vector<std::string> getRegisteredStrategyNames();

        /**
         * @brief Clear all registered custom strategies
         */
        static void clearRegisteredStrategies();

        /**
         * @brief Get the detected configuration file path (if any)
         * @return Optional path to detected configuration file
         */
        [[nodiscard]] static std::optional<std::filesystem::path> getDetectedConfigPath();

    private:
        /**
         * @brief Check if a file exists and is accessible
         */
        [[nodiscard]] static bool fileExists(const std::filesystem::path& path);

        /**
         * @brief Convert string to strategy type
         */
        [[nodiscard]] static std::optional<InitializationStrategyType> parseStrategyType(
            std::string_view typeName);

        /**
         * @brief Get the custom strategy registry
         */
        static std::map<std::string, StrategyCreator>& getCreatorRegistry();
        static std::map<std::string, StrategyCreatorWithPath>& getCreatorWithPathRegistry();
        static std::mutex& getRegistryMutex();
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZATIONFACTORY_HPP
