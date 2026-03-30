/*******************************************************************
 * @file   LoggingInitializer.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of LoggingInitializer singleton
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>

#include <iostream>

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/logger.h>
#include <log4cxx/logmanager.h>

namespace com::github::doevelopper::atlassians::logging
{
    // ============================================
    // LogFunctionScope Implementation
    // ============================================

    LogFunctionScope::LogFunctionScope(std::string_view funcName) noexcept
        : m_funcName(funcName)
    {
        try
        {
            if (LoggingInitializer::getInstance().isInitialized())
            {
                LOG4CXX_TRACE(::log4cxx::Logger::getRootLogger(),
                              ">>> Entering: " << std::string(m_funcName));
            }
        }
        catch (...)
        {
            // Silently ignore if logging not available
        }
    }

    LogFunctionScope::~LogFunctionScope() noexcept
    {
        try
        {
            if (LoggingInitializer::getInstance().isInitialized())
            {
                LOG4CXX_TRACE(::log4cxx::Logger::getRootLogger(),
                              "<<< Exiting: " << std::string(m_funcName));
            }
        }
        catch (...)
        {
            // Silently ignore if logging not available
        }
    }

    // ============================================
    // LoggingInitializer Implementation
    // ============================================

    LoggingInitializer& LoggingInitializer::getInstance() noexcept
    {
        static LoggingInitializer instance;
        return instance;
    }

    LoggingInitializer::LoggingInitializer() noexcept
        : m_strategy(nullptr)
        , m_initialized(false)
        , m_callback(nullptr)
    {
    }

    LoggingInitializer::~LoggingInitializer() noexcept
    {
        try
        {
            shutdown();
        }
        catch (...)
        {
            // Destructor must not throw
        }
    }

    bool LoggingInitializer::initialize(std::unique_ptr<ILoggingInitializationStrategy> strategy)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        // Check if already initialized
        if (m_initialized.load(std::memory_order_acquire))
        {
            notifyCallback(true, "Logging already initialized");
            return true;
        }

        if (!strategy)
        {
            notifyCallback(false, "Null strategy provided");
            return false;
        }

        // Validate strategy before applying
        if (!strategy->validate())
        {
            std::string errorMsg = "Strategy validation failed: " +
                                   std::string(strategy->getStrategyName());
            notifyCallback(false, errorMsg);
            throw LoggingInitializationException(errorMsg);
        }

        try
        {
            // Perform initialization
            if (doInitialize(*strategy))
            {
                m_strategy = std::move(strategy);
                m_initialized.store(true, std::memory_order_release);

                std::string successMsg = "Logging initialized with strategy: " +
                                         std::string(m_strategy->getStrategyName());
                notifyCallback(true, successMsg);

                LOG4CXX_INFO(::log4cxx::Logger::getRootLogger(), successMsg);
                return true;
            }

            notifyCallback(false, "Initialization returned false");
            return false;
        }
        catch (const std::exception& ex)
        {
            std::string errorMsg = "Initialization failed: " + std::string(ex.what());
            notifyCallback(false, errorMsg);

            // Log to stderr if logging not available
            std::cerr << "[LoggingInitializer] " << errorMsg << std::endl;
            throw LoggingInitializationException(errorMsg);
        }
    }

    bool LoggingInitializer::initialize()
    {
        return initialize(std::make_unique<DefaultInitializationStrategy>());
    }

    bool LoggingInitializer::doInitialize(const ILoggingInitializationStrategy& strategy)
    {
        try
        {
            // Log startup message
            auto rootLogger = ::log4cxx::Logger::getRootLogger();

            strategy.initialize();

            // Log startup message and appender count
            if (rootLogger)
            {
                LOG4CXX_INFO(rootLogger, "----START LOGGING-----");
                LOG4CXX_TRACE(rootLogger,
                    "Logging service initialized with "
                    << rootLogger->getAllAppenders().size()
                    << " appenders");
            }

            return true;
        }
        catch (const ::log4cxx::helpers::Exception& ex)
        {
            throw LoggingInitializationException(
                "Log4CXX error during initialization", ex);
        }
    }

    bool LoggingInitializer::checkLogManagerStatus() const noexcept
    {
        try
        {
            auto repository = ::log4cxx::LogManager::getLoggerRepository();
            return repository && repository->isConfigured();
        }
        catch (...)
        {
            return false;
        }
    }

    bool LoggingInitializer::isInitialized() const noexcept
    {
        if (!m_initialized.load(std::memory_order_acquire))
        {
            return false;
        }

        return checkLogManagerStatus();
    }

    void LoggingInitializer::shutdown() noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_initialized.load(std::memory_order_acquire))
        {
            return;
        }

        try
        {
            auto rootLogger = ::log4cxx::Logger::getRootLogger();
            if (rootLogger && ::log4cxx::LogManager::getLoggerRepository()->isConfigured())
            {
                LOG4CXX_INFO(rootLogger, "---- END LOGGING -----");
            }

            // Shutdown Log4CXX
            ::log4cxx::LogManager::shutdown();

            m_strategy.reset();
            m_initialized.store(false, std::memory_order_release);
        }
        catch (...)
        {
            // Ensure we mark as not initialized even on error
            m_initialized.store(false, std::memory_order_release);
        }
    }

    void LoggingInitializer::reset() noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        try
        {
            ::log4cxx::BasicConfigurator::resetConfiguration();
            ::log4cxx::LogManager::resetConfiguration();
        }
        catch (...)
        {
            // Silently ignore reset errors
        }
    }

    std::optional<std::string_view> LoggingInitializer::getCurrentStrategyName() const noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_strategy)
        {
            return m_strategy->getStrategyName();
        }
        return std::nullopt;
    }

    std::optional<std::string> LoggingInitializer::getConfigSource() const noexcept
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_strategy)
        {
            return m_strategy->getConfigSource();
        }
        return std::nullopt;
    }

    void LoggingInitializer::setInitializationCallback(InitializationCallback callback)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_callback = std::move(callback);
    }

    void LoggingInitializer::notifyCallback(bool success, std::string_view message) const
    {
        if (m_callback)
        {
            try
            {
                m_callback(success, message);
            }
            catch (...)
            {
                // Callback must not propagate exceptions
            }
        }
    }

    void LoggingInitializer::setRootLogLevel(LogLevel level)
    {
        if (!isInitialized())
        {
            return;
        }

        auto rootLogger = ::log4cxx::Logger::getRootLogger();
        if (!rootLogger)
        {
            return;
        }

        switch (level)
        {
            case LogLevel::OFF:
                rootLogger->setLevel(::log4cxx::Level::getOff());
                break;
            case LogLevel::FATAL:
                rootLogger->setLevel(::log4cxx::Level::getFatal());
                break;
            case LogLevel::ERROR:
                rootLogger->setLevel(::log4cxx::Level::getError());
                break;
            case LogLevel::WARN:
                rootLogger->setLevel(::log4cxx::Level::getWarn());
                break;
            case LogLevel::INFO:
                rootLogger->setLevel(::log4cxx::Level::getInfo());
                break;
            case LogLevel::DEBUG:
                rootLogger->setLevel(::log4cxx::Level::getDebug());
                break;
            case LogLevel::TRACE:
            case LogLevel::ALL:
                rootLogger->setLevel(::log4cxx::Level::getTrace());
                break;
        }
    }

    LogLevel LoggingInitializer::getRootLogLevel() const noexcept
    {
        if (!isInitialized())
        {
            return LogLevel::OFF;
        }

        try
        {
            auto rootLogger = ::log4cxx::Logger::getRootLogger();
            if (!rootLogger)
            {
                return LogLevel::OFF;
            }

            auto level = rootLogger->getLevel();
            if (!level)
            {
                return LogLevel::INFO;  // Default
            }

            if (level == ::log4cxx::Level::getOff())
                return LogLevel::OFF;
            if (level == ::log4cxx::Level::getFatal())
                return LogLevel::FATAL;
            if (level == ::log4cxx::Level::getError())
                return LogLevel::ERROR;
            if (level == ::log4cxx::Level::getWarn())
                return LogLevel::WARN;
            if (level == ::log4cxx::Level::getInfo())
                return LogLevel::INFO;
            if (level == ::log4cxx::Level::getDebug())
                return LogLevel::DEBUG;
            if (level == ::log4cxx::Level::getTrace())
                return LogLevel::TRACE;
            if (level == ::log4cxx::Level::getAll())
                return LogLevel::ALL;

            return LogLevel::INFO;
        }
        catch (...)
        {
            return LogLevel::OFF;
        }
    }

}  // namespace com::github::doevelopper::atlassians::logging

