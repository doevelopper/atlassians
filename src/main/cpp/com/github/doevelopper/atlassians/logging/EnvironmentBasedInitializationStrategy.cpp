/*******************************************************************
 * @file   EnvironmentBasedInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of EnvironmentBasedInitializationStrategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/EnvironmentBasedInitializationStrategy.hpp>

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <apr-1/apr.h>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/xml/domconfigurator.h>

namespace com::github::doevelopper::atlassians::logging
{
    namespace
    {
        // Environment variable for watch interval
        constexpr const char* ENV_VAR_WATCH_SECONDS = "LOG4CXX_CONFIGURATION_WATCH_SECONDS";

        // Default watch interval in milliseconds
        constexpr std::uint32_t DEFAULT_WATCH_INTERVAL_MS = 60000;

        /**
         * @brief Get watch interval from environment variable
         * @return Watch interval in milliseconds
         */
        std::uint32_t getWatchIntervalFromEnv() noexcept
        {
            const char* watchDelay = std::getenv(ENV_VAR_WATCH_SECONDS);
            if (watchDelay != nullptr && watchDelay[0] != '\0')
            {
                try
                {
                    int seconds = std::stoi(std::string(watchDelay));
                    if (seconds > 0)
                    {
                        return static_cast<std::uint32_t>(seconds * 1000);  // Convert to ms
                    }
                }
                catch (const std::invalid_argument& e)
                {
                    std::cerr << "Invalid watch seconds value: " << e.what() << std::endl;
                }
                catch (const std::out_of_range& e)
                {
                    std::cerr << "Watch seconds out of range: " << e.what() << std::endl;
                }
            }
            return DEFAULT_WATCH_INTERVAL_MS;
        }
    }  // anonymous namespace

    EnvironmentBasedInitializationStrategy::EnvironmentBasedInitializationStrategy() noexcept
        : m_primaryEnvVar(ENV_VAR_PRIMARY)
        , m_fallbackToBasic(true)
    {
    }

    EnvironmentBasedInitializationStrategy::EnvironmentBasedInitializationStrategy(
        std::string_view envVarName,
        bool fallbackToBasic)
        : m_primaryEnvVar(envVarName)
        , m_fallbackToBasic(fallbackToBasic)
    {
    }

    EnvironmentBasedInitializationStrategy::~EnvironmentBasedInitializationStrategy() noexcept = default;

    void EnvironmentBasedInitializationStrategy::initialize() const
    {
        try
        {
            auto configPath = findConfigPath();

            if (configPath.has_value())
            {
                const std::string pathStr = configPath->string();
                const std::uint32_t watchInterval = getWatchIntervalFromEnv();

                if (isXmlConfig(*configPath))
                {
#if APR_HAS_THREADS
                    ::log4cxx::xml::DOMConfigurator::configureAndWatch(
                        pathStr, static_cast<long>(watchInterval));
                    LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                                 "Log4CXX initialized with XML config (watching): " << pathStr);
#else
                    ::log4cxx::xml::DOMConfigurator::configure(pathStr);
                    LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                                 "Log4CXX initialized with XML config from environment: " << pathStr);
#endif
                }
                else
                {
#if APR_HAS_THREADS
                    ::log4cxx::PropertyConfigurator::configureAndWatch(
                        pathStr, static_cast<long>(watchInterval));
                    LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                                 "Log4CXX initialized with properties config (watching): " << pathStr);
#else
                    ::log4cxx::PropertyConfigurator::configure(pathStr);
                    LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                                 "Log4CXX initialized with properties config from environment: " << pathStr);
#endif
                }

                ::log4cxx::LogManager::getLoggerRepository()->setConfigured(true);
            }
            else if (m_fallbackToBasic)
            {
                // Fallback to basic configuration
                ::log4cxx::BasicConfigurator::configure();
                ::log4cxx::Logger::getRootLogger()->setLevel(::log4cxx::Level::getInfo());
                ::log4cxx::LogManager::getLoggerRepository()->setConfigured(true);

                LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                             "Log4CXX initialized with basic configuration (no environment variable set)");
            }
            else
            {
                throw LoggingInitializationException(
                    "No logging configuration found in environment variables and fallback disabled");
            }
        }
        catch (const ::log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Failed to initialize Log4CXX from environment: " + std::string(ex.what()));
        }
    }

    bool EnvironmentBasedInitializationStrategy::validate() const noexcept
    {
        // Valid if we find a config path or fallback is enabled
        return findConfigPath().has_value() || m_fallbackToBasic;
    }

    ILoggingInitializationStrategy::ConfigType
    EnvironmentBasedInitializationStrategy::getConfigType() const noexcept
    {
        return ConfigType::ENVIRONMENT;
    }

    std::string_view EnvironmentBasedInitializationStrategy::getStrategyName() const noexcept
    {
        return "EnvironmentBasedInitializationStrategy";
    }

    std::optional<std::string>
    EnvironmentBasedInitializationStrategy::getConfigSource() const noexcept
    {
        auto path = findConfigPath();
        if (path.has_value())
        {
            return path->string();
        }
        if (m_fallbackToBasic)
        {
            return "BasicConfigurator (fallback)";
        }
        return std::nullopt;
    }

    void EnvironmentBasedInitializationStrategy::setPrimaryEnvVar(std::string_view envVarName)
    {
        m_primaryEnvVar = envVarName;
    }

    void EnvironmentBasedInitializationStrategy::setFallbackEnabled(bool enable) noexcept
    {
        m_fallbackToBasic = enable;
    }

    bool EnvironmentBasedInitializationStrategy::isFallbackEnabled() const noexcept
    {
        return m_fallbackToBasic;
    }

    std::optional<std::filesystem::path>
    EnvironmentBasedInitializationStrategy::getResolvedPath() const noexcept
    {
        return findConfigPath();
    }

    std::optional<std::filesystem::path>
    EnvironmentBasedInitializationStrategy::findConfigPath() const noexcept
    {
        // Check environment variables in order of priority
        const std::array<const char*, 3> envVars = {
            m_primaryEnvVar.c_str(),
            ENV_VAR_SECONDARY,
            ENV_VAR_TERTIARY
        };

        for (const auto* envVar : envVars)
        {
            const char* value = std::getenv(envVar);
            if (value != nullptr && value[0] != '\0')
            {
                std::filesystem::path path(value);
                if (isFileAccessible(path))
                {
                    return path;
                }
            }
        }

        return std::nullopt;
    }

    bool EnvironmentBasedInitializationStrategy::isFileAccessible(
        const std::filesystem::path& path) noexcept
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

    bool EnvironmentBasedInitializationStrategy::isXmlConfig(
        const std::filesystem::path& path) noexcept
    {
        const auto extension = path.extension();
        return extension == ".xml" || extension == ".XML";
    }

}  // namespace com::github::doevelopper::atlassians::logging
