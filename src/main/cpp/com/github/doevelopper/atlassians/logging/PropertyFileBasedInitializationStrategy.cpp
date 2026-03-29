/*******************************************************************
 * @file   PropertyFileBasedInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of PropertyFileBasedInitializationStrategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/PropertyFileBasedInitializationStrategy.hpp>

#include <fstream>

#include <apr-1/apr.h>

#include <log4cxx/helpers/exception.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/propertyconfigurator.h>

namespace com::github::doevelopper::atlassians::logging
{
    PropertyFileBasedInitializationStrategy::PropertyFileBasedInitializationStrategy() noexcept
        : m_configFilePath()
        , m_watchEnabled(false)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    PropertyFileBasedInitializationStrategy::PropertyFileBasedInitializationStrategy(
        const std::filesystem::path& filePath)
        : m_configFilePath(filePath)
        , m_watchEnabled(true)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    PropertyFileBasedInitializationStrategy::PropertyFileBasedInitializationStrategy(
        std::string_view filePath)
        : m_configFilePath(filePath)
        , m_watchEnabled(true)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    PropertyFileBasedInitializationStrategy::~PropertyFileBasedInitializationStrategy() noexcept = default;

    void PropertyFileBasedInitializationStrategy::initialize() const
    {
        // Validate before initialization
        if (!validate())
        {
            throw LoggingInitializationException(
                "Configuration file validation failed: " + m_configFilePath.string());
        }

        try
        {
            const std::string pathStr = m_configFilePath.string();

#if APR_HAS_THREADS
            if (m_watchEnabled)
            {
                // Configure with file watching (requires APR threads)
                log4cxx::PropertyConfigurator::configureAndWatch(
                    pathStr,
                    static_cast<long>(m_watchInterval.count()));
            }
            else
            {
                // Simple one-time configuration
                log4cxx::PropertyConfigurator::configure(pathStr);
            }
#else
            // No thread support - always use simple configuration
            log4cxx::PropertyConfigurator::configure(pathStr);
#endif

            // Mark as configured
            log4cxx::LogManager::getLoggerRepository()->setConfigured(true);

            LOG4CXX_INFO(log4cxx::Logger::getRootLogger(),
                         "Log4CXX initialized with properties file: " << pathStr
#if APR_HAS_THREADS
                         << (m_watchEnabled ? " (watching enabled)" : ""));
#else
                         );
#endif
        }
        catch (const log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Failed to initialize Log4CXX with properties file '" +
                m_configFilePath.string() + "': " + ex.what());
        }
    }

    bool PropertyFileBasedInitializationStrategy::validate() const noexcept
    {
        if (m_configFilePath.empty())
        {
            return false;
        }

        if (!isFileAccessible())
        {
            return false;
        }

        // Check for valid extension
        const auto extension = m_configFilePath.extension();
        if (extension != ".properties" && extension != ".props")
        {
            return false;
        }

        return true;
    }

    ILoggingInitializationStrategy::ConfigType
    PropertyFileBasedInitializationStrategy::getConfigType() const noexcept
    {
        return ConfigType::PROPERTY;
    }

    std::string_view PropertyFileBasedInitializationStrategy::getStrategyName() const noexcept
    {
        return "PropertyFileBasedInitializationStrategy";
    }

    std::optional<std::string>
    PropertyFileBasedInitializationStrategy::getConfigSource() const noexcept
    {
        if (m_configFilePath.empty())
        {
            return std::nullopt;
        }
        return m_configFilePath.string();
    }

    void PropertyFileBasedInitializationStrategy::setConfigFilePath(
        const std::filesystem::path& filePath)
    {
        m_configFilePath = filePath;
    }

    const std::filesystem::path&
    PropertyFileBasedInitializationStrategy::getConfigFilePath() const noexcept
    {
        return m_configFilePath;
    }

    void PropertyFileBasedInitializationStrategy::setWatchEnabled(bool enable) noexcept
    {
        m_watchEnabled = enable;
    }

    bool PropertyFileBasedInitializationStrategy::isWatchEnabled() const noexcept
    {
        return m_watchEnabled;
    }

    void PropertyFileBasedInitializationStrategy::setWatchInterval(
        std::chrono::milliseconds interval) noexcept
    {
        m_watchInterval = interval;
    }

    std::chrono::milliseconds
    PropertyFileBasedInitializationStrategy::getWatchInterval() const noexcept
    {
        return m_watchInterval;
    }

    bool PropertyFileBasedInitializationStrategy::isFileAccessible() const noexcept
    {
        try
        {
            if (!std::filesystem::exists(m_configFilePath))
            {
                return false;
            }

            if (!std::filesystem::is_regular_file(m_configFilePath))
            {
                return false;
            }

            // Check if file is readable
            std::ifstream file(m_configFilePath);
            return file.good();
        }
        catch (const std::filesystem::filesystem_error&)
        {
            return false;
        }
    }

}  // namespace com::github::doevelopper::atlassians::logging
