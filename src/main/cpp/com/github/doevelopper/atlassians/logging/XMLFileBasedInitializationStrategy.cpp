/*******************************************************************
 * @file   XMLFileBasedInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of XMLFileBasedInitializationStrategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/XMLFileBasedInitializationStrategy.hpp>

#include <fstream>

#include <apr-1/apr.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/xml/domconfigurator.h>

namespace com::github::doevelopper::atlassians::logging
{
    XMLFileBasedInitializationStrategy::XMLFileBasedInitializationStrategy() noexcept
        : m_configFilePath()
        , m_watchEnabled(false)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    XMLFileBasedInitializationStrategy::XMLFileBasedInitializationStrategy(
        const std::filesystem::path& filePath)
        : m_configFilePath(filePath)
        , m_watchEnabled(true)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    XMLFileBasedInitializationStrategy::XMLFileBasedInitializationStrategy(
        std::string_view filePath)
        : m_configFilePath(filePath)
        , m_watchEnabled(true)
        , m_watchInterval(DEFAULT_WATCH_INTERVAL)
    {
    }

    XMLFileBasedInitializationStrategy::~XMLFileBasedInitializationStrategy() noexcept = default;

    void XMLFileBasedInitializationStrategy::initialize() const
    {
        // Validate before initialization
        if (!validate())
        {
            throw LoggingInitializationException(
                "XML configuration file validation failed: " + m_configFilePath.string());
        }

        try
        {
            const std::string pathStr = m_configFilePath.string();

#if APR_HAS_THREADS
            if (m_watchEnabled)
            {
                // Configure with file watching (only when APR has thread support)
                ::log4cxx::xml::DOMConfigurator::configureAndWatch(
                    pathStr,
                    static_cast<long>(m_watchInterval.count()));
            }
            else
            {
                // Simple one-time configuration
                ::log4cxx::xml::DOMConfigurator::configure(pathStr);
            }
#else
            // APR built without thread support, cannot use configureAndWatch
            ::log4cxx::xml::DOMConfigurator::configure(pathStr);
#endif

            // Mark as configured
            ::log4cxx::LogManager::getLoggerRepository()->setConfigured(true);

            LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(),
                         "Log4CXX initialized with XML file: " << pathStr
#if APR_HAS_THREADS
                         << (m_watchEnabled ? " (watching enabled)" : ""));
#else
                         << " (watching disabled - APR without thread support)");
#endif
        }
        catch (const ::log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Failed to initialize Log4CXX with XML file '" +
                m_configFilePath.string() + "': " + ex.what());
        }
    }

    bool XMLFileBasedInitializationStrategy::validate() const noexcept
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
        if (extension != ".xml" && extension != ".XML")
        {
            return false;
        }

        return true;
    }

    ILoggingInitializationStrategy::ConfigType
    XMLFileBasedInitializationStrategy::getConfigType() const noexcept
    {
        return ConfigType::XML;
    }

    std::string_view XMLFileBasedInitializationStrategy::getStrategyName() const noexcept
    {
        return "XMLFileBasedInitializationStrategy";
    }

    std::optional<std::string>
    XMLFileBasedInitializationStrategy::getConfigSource() const noexcept
    {
        if (m_configFilePath.empty())
        {
            return std::nullopt;
        }
        return m_configFilePath.string();
    }

    void XMLFileBasedInitializationStrategy::setConfigFilePath(
        const std::filesystem::path& filePath)
    {
        m_configFilePath = filePath;
    }

    const std::filesystem::path&
    XMLFileBasedInitializationStrategy::getConfigFilePath() const noexcept
    {
        return m_configFilePath;
    }

    void XMLFileBasedInitializationStrategy::setWatchEnabled(bool enable) noexcept
    {
        m_watchEnabled = enable;
    }

    bool XMLFileBasedInitializationStrategy::isWatchEnabled() const noexcept
    {
        return m_watchEnabled;
    }

    void XMLFileBasedInitializationStrategy::setWatchInterval(
        std::chrono::milliseconds interval) noexcept
    {
        m_watchInterval = interval;
    }

    std::chrono::milliseconds
    XMLFileBasedInitializationStrategy::getWatchInterval() const noexcept
    {
        return m_watchInterval;
    }

    bool XMLFileBasedInitializationStrategy::isFileAccessible() const noexcept
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
