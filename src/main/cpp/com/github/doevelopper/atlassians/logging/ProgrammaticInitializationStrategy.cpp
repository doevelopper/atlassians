/*******************************************************************
 * @file   ProgrammaticInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of ProgrammaticInitializationStrategy
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/ProgrammaticInitializationStrategy.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/helpers/pool.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/rolling/rollingfileappender.h>
#include <log4cxx/rolling/sizebasedtriggeringpolicy.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>

namespace com::github::doevelopper::atlassians::logging
{
    namespace
    {
        /**
         * @brief Parse a file size string (e.g., "10MB", "100KB", "1GB") to bytes
         * @param sizeStr The size string
         * @return Size in bytes
         */
        std::size_t parseFileSize(const std::string& sizeStr)
        {
            if (sizeStr.empty())
            {
                return 10 * 1024 * 1024;  // Default 10MB
            }

            std::string str = sizeStr;
            std::transform(str.begin(), str.end(), str.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });

            std::size_t multiplier = 1;
            std::string numPart = str;

            if (str.size() >= 2)
            {
                if (str.back() == 'B')
                {
                    str.pop_back();
                }

                if (!str.empty() && (str.back() == 'K' || str.back() == 'M' || str.back() == 'G'))
                {
                    char unit = str.back();
                    str.pop_back();
                    numPart = str;

                    switch (unit)
                    {
                        case 'K': multiplier = 1024ULL; break;
                        case 'M': multiplier = 1024ULL * 1024; break;
                        case 'G': multiplier = 1024ULL * 1024 * 1024; break;
                        default: break;
                    }
                }
            }

            try
            {
                std::size_t value = std::stoull(numPart);
                return value * multiplier;
            }
            catch (...)
            {
                return 10 * 1024 * 1024;  // Default 10MB on parse error
            }
        }
    }  // anonymous namespace

    // ============================================
    // Builder Implementation
    // ============================================

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::withConsoleAppender(
        std::string_view name,
        std::string_view pattern)
    {
        AppenderConfig config;
        config.type = AppenderConfig::Type::CONSOLE;
        config.name = name;
        config.pattern = pattern;
        config.target = "System.out";
        m_appenders.push_back(std::move(config));
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::withStderrAppender(
        std::string_view name,
        std::string_view pattern)
    {
        AppenderConfig config;
        config.type = AppenderConfig::Type::CONSOLE;
        config.name = name;
        config.pattern = pattern;
        config.target = "System.err";
        m_appenders.push_back(std::move(config));
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::withFileAppender(
        std::string_view name,
        const std::filesystem::path& filePath,
        std::string_view pattern,
        bool append)
    {
        AppenderConfig config;
        config.type = AppenderConfig::Type::FILE;
        config.name = name;
        config.pattern = pattern;
        config.filePath = filePath;
        config.append = append;
        m_appenders.push_back(std::move(config));
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::withRollingFileAppender(
        std::string_view name,
        const std::filesystem::path& filePath,
        std::string_view pattern)
    {
        AppenderConfig config;
        config.type = AppenderConfig::Type::ROLLING_FILE;
        config.name = name;
        config.pattern = pattern;
        config.filePath = filePath;
        config.maxFileSize = "10MB";
        config.maxBackupIndex = 5;
        m_appenders.push_back(std::move(config));
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::maxFileSize(std::string_view size)
    {
        if (!m_appenders.empty() &&
            m_appenders.back().type == AppenderConfig::Type::ROLLING_FILE)
        {
            m_appenders.back().maxFileSize = size;
        }
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::maxBackupIndex(int index)
    {
        if (!m_appenders.empty() &&
            m_appenders.back().type == AppenderConfig::Type::ROLLING_FILE)
        {
            m_appenders.back().maxBackupIndex = index;
        }
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::setRootLevel(const log4cxx::LevelPtr& level)
    {
        m_rootLevel = level;
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::addLogger(
        std::string_view loggerName,
        const log4cxx::LevelPtr& level)
    {
        LoggerConfig config;
        config.name = loggerName;
        config.level = level;
        config.additivity = true;
        m_loggers.push_back(std::move(config));
        return *this;
    }

    ProgrammaticInitializationStrategy::Builder&
    ProgrammaticInitializationStrategy::Builder::addLogger(
        std::string_view loggerName,
        const log4cxx::LevelPtr& level,
        const std::vector<std::string>& appenderNames,
        bool additivity)
    {
        LoggerConfig config;
        config.name = loggerName;
        config.level = level;
        config.appenders = appenderNames;
        config.additivity = additivity;
        m_loggers.push_back(std::move(config));
        return *this;
    }

    std::unique_ptr<ProgrammaticInitializationStrategy>
    ProgrammaticInitializationStrategy::Builder::build()
    {
        // Default to INFO level if not set
        if (!m_rootLevel)
        {
            m_rootLevel = log4cxx::Level::getInfo();
        }

        // Add default console appender if none configured
        if (m_appenders.empty())
        {
            withConsoleAppender("console");
        }

        return std::make_unique<ProgrammaticInitializationStrategy>(
            std::move(m_appenders),
            std::move(m_loggers),
            m_rootLevel);
    }

    // ============================================
    // ProgrammaticInitializationStrategy Implementation
    // ============================================

    ProgrammaticInitializationStrategy::Builder
    ProgrammaticInitializationStrategy::builder()
    {
        return Builder{};
    }

    ProgrammaticInitializationStrategy::ProgrammaticInitializationStrategy() noexcept
        : m_appenders()
        , m_loggers()
        , m_rootLevel(log4cxx::Level::getInfo())
    {
        // Add default console appender
        AppenderConfig config;
        config.type = AppenderConfig::Type::CONSOLE;
        config.name = "console";
        config.pattern = DEFAULT_PATTERN;
        m_appenders.push_back(std::move(config));
    }

    ProgrammaticInitializationStrategy::ProgrammaticInitializationStrategy(
        std::vector<AppenderConfig> appenders,
        std::vector<LoggerConfig> loggers,
        log4cxx::LevelPtr rootLevel)
        : m_appenders(std::move(appenders))
        , m_loggers(std::move(loggers))
        , m_rootLevel(std::move(rootLevel))
    {
    }

    ProgrammaticInitializationStrategy::~ProgrammaticInitializationStrategy() noexcept = default;

    void ProgrammaticInitializationStrategy::initialize() const
    {
        try
        {
            log4cxx::helpers::Pool pool;
            auto rootLogger = log4cxx::Logger::getRootLogger();

            // Store created appenders by name
            std::map<std::string, log4cxx::AppenderPtr> appenderMap;

            // Create and configure appenders
            for (const auto& config : m_appenders)
            {
                auto appender = createAppender(config);
                if (appender)
                {
                    appenderMap[config.name] = appender;
                    rootLogger->addAppender(appender);
                }
            }

            // Set root logger level
            if (m_rootLevel)
            {
                rootLogger->setLevel(m_rootLevel);
            }

            // Configure additional loggers
            for (const auto& loggerConfig : m_loggers)
            {
                auto logger = log4cxx::Logger::getLogger(loggerConfig.name);
                if (loggerConfig.level)
                {
                    logger->setLevel(loggerConfig.level);
                }
                logger->setAdditivity(loggerConfig.additivity);

                // Attach specific appenders if configured
                if (!loggerConfig.appenders.empty())
                {
                    for (const auto& appenderName : loggerConfig.appenders)
                    {
                        auto it = appenderMap.find(appenderName);
                        if (it != appenderMap.end())
                        {
                            logger->addAppender(it->second);
                        }
                    }
                }
            }

            // Mark as configured
            log4cxx::LogManager::getLoggerRepository()->setConfigured(true);

            LOG4CXX_INFO(rootLogger, "Log4CXX initialized programmatically with "
                         << m_appenders.size() << " appender(s) and "
                         << m_loggers.size() << " logger(s)");
        }
        catch (const log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Failed to initialize Log4CXX programmatically: " +
                std::string(ex.what()));
        }
    }

    bool ProgrammaticInitializationStrategy::validate() const noexcept
    {
        // Check that we have at least one appender
        if (m_appenders.empty())
        {
            return false;
        }

        // Validate file paths for file-based appenders
        for (const auto& config : m_appenders)
        {
            if (config.type == AppenderConfig::Type::FILE ||
                config.type == AppenderConfig::Type::ROLLING_FILE ||
                config.type == AppenderConfig::Type::DAILY_ROLLING_FILE)
            {
                if (config.filePath.empty())
                {
                    return false;
                }

                // Check that parent directory exists or can be created
                auto parentDir = config.filePath.parent_path();
                if (!parentDir.empty())
                {
                    try
                    {
                        if (!std::filesystem::exists(parentDir) &&
                            !std::filesystem::create_directories(parentDir))
                        {
                            return false;
                        }
                    }
                    catch (...)
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    ILoggingInitializationStrategy::ConfigType
    ProgrammaticInitializationStrategy::getConfigType() const noexcept
    {
        return ConfigType::PROGRAMMATIC;
    }

    std::string_view ProgrammaticInitializationStrategy::getStrategyName() const noexcept
    {
        return "ProgrammaticInitializationStrategy";
    }

    std::optional<std::string>
    ProgrammaticInitializationStrategy::getConfigSource() const noexcept
    {
        std::ostringstream oss;
        oss << "Programmatic configuration with " << m_appenders.size()
            << " appender(s) and " << m_loggers.size() << " logger(s)";
        return oss.str();
    }

    log4cxx::AppenderPtr
    ProgrammaticInitializationStrategy::createAppender(const AppenderConfig& config) const
    {
        log4cxx::helpers::Pool pool;

        // Create layout
        auto layout = std::make_shared<log4cxx::PatternLayout>(
            LOG4CXX_STR(config.pattern.c_str()));

        log4cxx::AppenderPtr appender;

        switch (config.type)
        {
            case AppenderConfig::Type::CONSOLE:
            {
                auto consoleAppender = std::make_shared<log4cxx::ConsoleAppender>(
                    layout,
                    config.target == "System.err"
                        ? log4cxx::ConsoleAppender::getSystemErr()
                        : log4cxx::ConsoleAppender::getSystemOut());
                consoleAppender->setName(LOG4CXX_STR(config.name.c_str()));
                consoleAppender->activateOptions(pool);
                appender = consoleAppender;
                break;
            }

            case AppenderConfig::Type::FILE:
            {
                auto fileAppender = std::make_shared<log4cxx::FileAppender>(
                    layout,
                    LOG4CXX_STR(config.filePath.string().c_str()),
                    config.append);
                fileAppender->setName(LOG4CXX_STR(config.name.c_str()));
                fileAppender->activateOptions(pool);
                appender = fileAppender;
                break;
            }

            case AppenderConfig::Type::ROLLING_FILE:
            {
                auto rollingAppender = std::make_shared<log4cxx::rolling::RollingFileAppender>();
                rollingAppender->setName(LOG4CXX_STR(config.name.c_str()));
                rollingAppender->setLayout(layout);
                rollingAppender->setFile(LOG4CXX_STR(config.filePath.string().c_str()));
                rollingAppender->setAppend(config.append);

                // Set up triggering policy (size-based)
                auto triggerPolicy = std::make_shared<log4cxx::rolling::SizeBasedTriggeringPolicy>();
                triggerPolicy->setMaxFileSize(parseFileSize(config.maxFileSize));
                triggerPolicy->activateOptions(pool);
                rollingAppender->setTriggeringPolicy(triggerPolicy);

                // Set up rolling policy (fixed window)
                auto rollingPolicy = std::make_shared<log4cxx::rolling::FixedWindowRollingPolicy>();
                rollingPolicy->setFileNamePattern(
                    LOG4CXX_STR((config.filePath.string() + ".%i").c_str()));
                rollingPolicy->setMinIndex(1);
                rollingPolicy->setMaxIndex(config.maxBackupIndex);
                rollingPolicy->activateOptions(pool);
                rollingAppender->setRollingPolicy(rollingPolicy);

                rollingAppender->activateOptions(pool);
                appender = rollingAppender;
                break;
            }

            default:
                break;
        }

        return appender;
    }

}  // namespace com::github::doevelopper::atlassians::logging
