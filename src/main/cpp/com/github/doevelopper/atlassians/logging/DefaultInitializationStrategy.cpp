/*******************************************************************
 * @file   DefaultInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of DefaultInitializationStrategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/patternlayout.h>

namespace com::github::doevelopper::atlassians::logging
{
    DefaultInitializationStrategy::DefaultInitializationStrategy() noexcept
        : m_pattern(DEFAULT_PATTERN)
        , m_useTraceLevel(false)
    {
    }

    DefaultInitializationStrategy::DefaultInitializationStrategy(std::string_view pattern) noexcept
        : m_pattern(pattern)
        , m_useTraceLevel(false)
    {
    }

    DefaultInitializationStrategy::~DefaultInitializationStrategy() noexcept = default;

    void DefaultInitializationStrategy::initialize() const
    {
        try
        {
            // Create pattern layout with configured pattern
            auto layout = std::make_shared<::log4cxx::PatternLayout>(
                LOG4CXX_STR(m_pattern.c_str()));

            // Create console appender with the layout
            auto consoleAppender = std::make_shared<::log4cxx::ConsoleAppender>(layout);

            // Activate appender options
            ::log4cxx::helpers::Pool pool;
            consoleAppender->activateOptions(pool);

            // Configure with the console appender
            ::log4cxx::BasicConfigurator::configure(consoleAppender);

            // Set root logger level
            auto rootLogger = ::log4cxx::Logger::getRootLogger();
            if (m_useTraceLevel)
            {
                rootLogger->setLevel(::log4cxx::Level::getTrace());
            }
            else
            {
                rootLogger->setLevel(::log4cxx::Level::getInfo());
            }

            // Mark repository as configured
            ::log4cxx::LogManager::getLoggerRepository()->setConfigured(true);

            LOG4CXX_DEBUG(rootLogger,
                          "Log4CXX initialized with default configuration (pattern: "
                          << m_pattern << ")");
        }
        catch (const ::log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Failed to initialize Log4CXX with default configuration: " +
                std::string(ex.what()));
        }
    }

    bool DefaultInitializationStrategy::validate() const noexcept
    {
        // Default strategy is always valid
        return true;
    }

    ILoggingInitializationStrategy::ConfigType
    DefaultInitializationStrategy::getConfigType() const noexcept
    {
        return ConfigType::DEFAULT;
    }

    std::string_view DefaultInitializationStrategy::getStrategyName() const noexcept
    {
        return "DefaultInitializationStrategy";
    }

    void DefaultInitializationStrategy::setUseTraceLevel(bool useTrace) noexcept
    {
        m_useTraceLevel = useTrace;
    }

    void DefaultInitializationStrategy::setPattern(std::string_view pattern)
    {
        m_pattern = pattern;
    }

}  // namespace com::github::doevelopper::atlassians::logging
