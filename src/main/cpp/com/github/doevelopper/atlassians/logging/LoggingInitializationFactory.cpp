/*******************************************************************
 * @file   LoggingInitializationFactory.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of LoggingInitializationFactory
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/LoggingInitializationFactory.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>
#include <com/github/doevelopper/atlassians/logging/EnvironmentBasedInitializationStrategy.hpp>
#include <com/github/doevelopper/atlassians/logging/PropertyFileBasedInitializationStrategy.hpp>
#include <com/github/doevelopper/atlassians/logging/ProgrammaticInitializationStrategy.hpp>
#include <com/github/doevelopper/atlassians/logging/XMLFileBasedInitializationStrategy.hpp>

#include <algorithm>
#include <cstdlib>
#include <fstream>

namespace com::github::doevelopper::atlassians::logging
{
    // Static registry storage
    std::map<std::string, LoggingInitializationFactory::StrategyCreator>&
    LoggingInitializationFactory::getCreatorRegistry()
    {
        static std::map<std::string, StrategyCreator> registry;
        return registry;
    }

    std::map<std::string, LoggingInitializationFactory::StrategyCreatorWithPath>&
    LoggingInitializationFactory::getCreatorWithPathRegistry()
    {
        static std::map<std::string, StrategyCreatorWithPath> registry;
        return registry;
    }

    std::mutex& LoggingInitializationFactory::getRegistryMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    std::unique_ptr<ILoggingInitializationStrategy> LoggingInitializationFactory::create(
        InitializationStrategyType type,
        const std::filesystem::path& configPath)
    {
        switch (type)
        {
            case InitializationStrategyType::Default:
                return std::make_unique<DefaultInitializationStrategy>();

            case InitializationStrategyType::PropertyFile:
                if (configPath.empty())
                {
                    throw std::invalid_argument(
                        "PropertyFile strategy requires a configuration path");
                }
                return std::make_unique<PropertyFileBasedInitializationStrategy>(configPath);

            case InitializationStrategyType::XMLFile:
                if (configPath.empty())
                {
                    throw std::invalid_argument(
                        "XMLFile strategy requires a configuration path");
                }
                return std::make_unique<XMLFileBasedInitializationStrategy>(configPath);

            case InitializationStrategyType::Environment:
                return std::make_unique<EnvironmentBasedInitializationStrategy>();

            case InitializationStrategyType::Programmatic:
                return std::make_unique<ProgrammaticInitializationStrategy>();

            case InitializationStrategyType::Custom:
                throw std::invalid_argument(
                    "Custom strategy type requires using createCustom() method");
        }

        // Fallback to default
        return std::make_unique<DefaultInitializationStrategy>();
    }

    std::unique_ptr<ILoggingInitializationStrategy> LoggingInitializationFactory::create(
        std::string_view typeName,
        const std::filesystem::path& configPath)
    {
        auto type = parseStrategyType(typeName);
        if (type.has_value())
        {
            return create(type.value(), configPath);
        }

        // Try to find as a custom registered strategy
        if (isStrategyRegistered(typeName))
        {
            return configPath.empty()
                ? createCustom(typeName)
                : createCustom(typeName, configPath);
        }

        throw std::invalid_argument(
            "Unknown strategy type: " + std::string(typeName));
    }

    std::unique_ptr<ILoggingInitializationStrategy>
    LoggingInitializationFactory::createAutoDetected()
    {
        auto detectedPath = getDetectedConfigPath();

        if (detectedPath.has_value())
        {
            const auto& path = detectedPath.value();
            const auto extension = path.extension();

            if (extension == ".xml" || extension == ".XML")
            {
                return std::make_unique<XMLFileBasedInitializationStrategy>(path);
            }
            else if (extension == ".properties" || extension == ".props")
            {
                return std::make_unique<PropertyFileBasedInitializationStrategy>(path);
            }
        }

        // Check environment
        const char* envConfig = std::getenv(
            EnvironmentBasedInitializationStrategy::ENV_VAR_PRIMARY);
        if (envConfig != nullptr)
        {
            return std::make_unique<EnvironmentBasedInitializationStrategy>();
        }

        // Default fallback
        return std::make_unique<DefaultInitializationStrategy>();
    }

    InitializationStrategyType LoggingInitializationFactory::detectStrategyType()
    {
        auto detectedPath = getDetectedConfigPath();

        if (detectedPath.has_value())
        {
            const auto& path = detectedPath.value();
            const auto extension = path.extension();

            if (extension == ".xml" || extension == ".XML")
            {
                return InitializationStrategyType::XMLFile;
            }
            else if (extension == ".properties" || extension == ".props")
            {
                return InitializationStrategyType::PropertyFile;
            }
        }

        // Check environment
        const char* envConfig = std::getenv(
            EnvironmentBasedInitializationStrategy::ENV_VAR_PRIMARY);
        if (envConfig != nullptr)
        {
            return InitializationStrategyType::Environment;
        }

        return InitializationStrategyType::Default;
    }

    void LoggingInitializationFactory::registerStrategy(
        std::string_view name,
        StrategyCreator creator)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());
        getCreatorRegistry()[std::string(name)] = std::move(creator);
    }

    void LoggingInitializationFactory::registerStrategyWithPath(
        std::string_view name,
        StrategyCreatorWithPath creator)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());
        getCreatorWithPathRegistry()[std::string(name)] = std::move(creator);
    }

    bool LoggingInitializationFactory::unregisterStrategy(std::string_view name)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());

        const std::string nameStr(name);
        bool removed = false;

        auto it1 = getCreatorRegistry().find(nameStr);
        if (it1 != getCreatorRegistry().end())
        {
            getCreatorRegistry().erase(it1);
            removed = true;
        }

        auto it2 = getCreatorWithPathRegistry().find(nameStr);
        if (it2 != getCreatorWithPathRegistry().end())
        {
            getCreatorWithPathRegistry().erase(it2);
            removed = true;
        }

        return removed;
    }

    std::unique_ptr<ILoggingInitializationStrategy>
    LoggingInitializationFactory::createCustom(std::string_view name)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());

        const std::string nameStr(name);
        auto it = getCreatorRegistry().find(nameStr);
        if (it != getCreatorRegistry().end())
        {
            return it->second();
        }

        throw std::invalid_argument(
            "Custom strategy not registered: " + nameStr);
    }

    std::unique_ptr<ILoggingInitializationStrategy>
    LoggingInitializationFactory::createCustom(
        std::string_view name,
        const std::filesystem::path& configPath)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());

        const std::string nameStr(name);

        // First try path-based creator
        auto it = getCreatorWithPathRegistry().find(nameStr);
        if (it != getCreatorWithPathRegistry().end())
        {
            return it->second(configPath);
        }

        // Fall back to regular creator
        auto it2 = getCreatorRegistry().find(nameStr);
        if (it2 != getCreatorRegistry().end())
        {
            return it2->second();
        }

        throw std::invalid_argument(
            "Custom strategy not registered: " + nameStr);
    }

    bool LoggingInitializationFactory::isStrategyRegistered(std::string_view name)
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());

        const std::string nameStr(name);
        return getCreatorRegistry().count(nameStr) > 0 ||
               getCreatorWithPathRegistry().count(nameStr) > 0;
    }

    std::vector<std::string> LoggingInitializationFactory::getRegisteredStrategyNames()
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());

        std::vector<std::string> names;

        for (const auto& [name, _] : getCreatorRegistry())
        {
            names.push_back(name);
        }

        for (const auto& [name, _] : getCreatorWithPathRegistry())
        {
            if (std::find(names.begin(), names.end(), name) == names.end())
            {
                names.push_back(name);
            }
        }

        return names;
    }

    void LoggingInitializationFactory::clearRegisteredStrategies()
    {
        std::lock_guard<std::mutex> lock(getRegistryMutex());
        getCreatorRegistry().clear();
        getCreatorWithPathRegistry().clear();
    }

    std::optional<std::filesystem::path>
    LoggingInitializationFactory::getDetectedConfigPath()
    {
        // Common configuration file names to search for
        static const std::array<const char*, 4> configFiles = {
            "log4cxx.xml",
            "log4cxx.properties",
            "logging.xml",
            "logging.properties"
        };

        // Common directories to search
        static const std::array<const char*, 5> searchDirs = {
            ".",
            "./config",
            "./conf",
            "./etc",
            "../config"
        };

        // Check environment variable first
        for (const auto* envVar : {
            EnvironmentBasedInitializationStrategy::ENV_VAR_PRIMARY,
            EnvironmentBasedInitializationStrategy::ENV_VAR_SECONDARY,
            EnvironmentBasedInitializationStrategy::ENV_VAR_TERTIARY})
        {
            const char* value = std::getenv(envVar);
            if (value != nullptr && value[0] != '\0')
            {
                std::filesystem::path path(value);
                if (fileExists(path))
                {
                    return path;
                }
            }
        }

        // Search common locations
        for (const auto* dir : searchDirs)
        {
            for (const auto* file : configFiles)
            {
                std::filesystem::path path =
                    std::filesystem::path(dir) / file;
                if (fileExists(path))
                {
                    return std::filesystem::canonical(path);
                }
            }
        }

        return std::nullopt;
    }

    bool LoggingInitializationFactory::fileExists(const std::filesystem::path& path)
    {
        try
        {
            if (!std::filesystem::exists(path))
            {
                return false;
            }

            if (!std::filesystem::is_regular_file(path))
            {
                return false;
            }

            std::ifstream file(path);
            return file.good();
        }
        catch (...)
        {
            return false;
        }
    }

    std::optional<InitializationStrategyType>
    LoggingInitializationFactory::parseStrategyType(std::string_view typeName)
    {
        // Case-insensitive comparison
        std::string lower(typeName);
        std::transform(lower.begin(), lower.end(), lower.begin(),
                       [](unsigned char c) { return std::tolower(c); });

        if (lower == "default" || lower == "basic")
        {
            return InitializationStrategyType::Default;
        }
        if (lower == "propertyfile" || lower == "properties" || lower == "props")
        {
            return InitializationStrategyType::PropertyFile;
        }
        if (lower == "xmlfile" || lower == "xml")
        {
            return InitializationStrategyType::XMLFile;
        }
        if (lower == "environment" || lower == "env")
        {
            return InitializationStrategyType::Environment;
        }
        if (lower == "programmatic" || lower == "runtime" || lower == "code")
        {
            return InitializationStrategyType::Programmatic;
        }
        if (lower == "custom")
        {
            return InitializationStrategyType::Custom;
        }

        return std::nullopt;
    }

}  // namespace com::github::doevelopper::atlassians::logging
