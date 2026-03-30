/*******************************************************************
 * @file   CustomLoggerPrivate.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of CustomLoggerPrivate
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/CustomLoggerPrivate.hpp>

#include <log4cxx/logmanager.h>
#include <log4cxx/spi/location/locationinfo.h>

namespace com::github::doevelopper::atlassians::logging
{
    CustomLoggerPrivate::CustomLoggerPrivate() noexcept
        : m_logger(::log4cxx::Logger::getRootLogger())
    {
    }

    CustomLoggerPrivate::CustomLoggerPrivate(std::string_view loggerName)
        : m_logger(::log4cxx::Logger::getLogger(std::string(loggerName)))
    {
    }

    CustomLoggerPrivate::CustomLoggerPrivate(const ::log4cxx::LoggerPtr& logger) noexcept
        : m_logger(logger)
    {
    }

    CustomLoggerPrivate::CustomLoggerPrivate(const CustomLoggerPrivate& other)
        : m_logger(other.m_logger)
    {
    }

    CustomLoggerPrivate::CustomLoggerPrivate(CustomLoggerPrivate&& other) noexcept
        : m_logger(std::move(other.m_logger))
    {
    }

    CustomLoggerPrivate& CustomLoggerPrivate::operator=(const CustomLoggerPrivate& other)
    {
        if (this != &other)
        {
            m_logger = other.m_logger;
        }
        return *this;
    }

    CustomLoggerPrivate& CustomLoggerPrivate::operator=(CustomLoggerPrivate&& other) noexcept
    {
        if (this != &other)
        {
            m_logger = std::move(other.m_logger);
        }
        return *this;
    }

    CustomLoggerPrivate::~CustomLoggerPrivate() noexcept = default;

    // ============================================
    // Logging Methods
    // ============================================

    void CustomLoggerPrivate::trace(const std::string& message) const
    {
        log(::log4cxx::Level::getTrace(), message);
    }

    void CustomLoggerPrivate::debug(const std::string& message) const
    {
        log(::log4cxx::Level::getDebug(), message);
    }

    void CustomLoggerPrivate::info(const std::string& message) const
    {
        log(::log4cxx::Level::getInfo(), message);
    }

    void CustomLoggerPrivate::warn(const std::string& message) const
    {
        log(::log4cxx::Level::getWarn(), message);
    }

    void CustomLoggerPrivate::error(const std::string& message) const
    {
        log(::log4cxx::Level::getError(), message);
    }

    void CustomLoggerPrivate::fatal(const std::string& message) const
    {
        log(::log4cxx::Level::getFatal(), message);
    }

    void CustomLoggerPrivate::logWithLocation(
        const ::log4cxx::LevelPtr& level,
        const std::string& message,
        const char* file,
        int line,
        const char* func) const
    {
        if (m_logger && m_logger->isEnabledFor(level))
        {
            // LocationInfo takes: fileName, shortFileName, methodName, lineNumber
            ::log4cxx::spi::LocationInfo location(file, file, func, line);
            m_logger->forcedLog(level, message, location);
        }
    }

    void CustomLoggerPrivate::log(
        const ::log4cxx::LevelPtr& level,
        const std::string& message) const
    {
        if (m_logger && m_logger->isEnabledFor(level))
        {
            m_logger->log(level, message);
        }
    }

    // ============================================
    // Level Checking
    // ============================================

    bool CustomLoggerPrivate::isTraceEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getTrace());
    }

    bool CustomLoggerPrivate::isDebugEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getDebug());
    }

    bool CustomLoggerPrivate::isInfoEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getInfo());
    }

    bool CustomLoggerPrivate::isWarnEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getWarn());
    }

    bool CustomLoggerPrivate::isErrorEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getError());
    }

    bool CustomLoggerPrivate::isFatalEnabled() const noexcept
    {
        return isEnabledFor(::log4cxx::Level::getFatal());
    }

    bool CustomLoggerPrivate::isEnabledFor(const ::log4cxx::LevelPtr& level) const noexcept
    {
        try
        {
            return m_logger && m_logger->isEnabledFor(level);
        }
        catch (...)
        {
            return false;
        }
    }

    // ============================================
    // Logger Properties
    // ============================================

    std::string CustomLoggerPrivate::getName() const
    {
        if (m_logger)
        {
            return m_logger->getName();
        }
        return {};
    }

    ::log4cxx::LevelPtr CustomLoggerPrivate::getLevel() const
    {
        if (m_logger)
        {
            return m_logger->getLevel();
        }
        return nullptr;
    }

    ::log4cxx::LevelPtr CustomLoggerPrivate::getEffectiveLevel() const
    {
        if (m_logger)
        {
            return m_logger->getEffectiveLevel();
        }
        return ::log4cxx::Level::getOff();
    }

    void CustomLoggerPrivate::setLevel(const ::log4cxx::LevelPtr& level)
    {
        if (m_logger)
        {
            m_logger->setLevel(level);
        }
    }

    ::log4cxx::LoggerPtr CustomLoggerPrivate::getLogger() const noexcept
    {
        return m_logger;
    }

    bool CustomLoggerPrivate::isValid() const noexcept
    {
        return m_logger != nullptr;
    }

    // ============================================
    // MDC Support
    // ============================================

    void CustomLoggerPrivate::putMDC(const std::string& key, const std::string& value)
    {
        ::log4cxx::MDC::put(key, value);
    }

    std::string CustomLoggerPrivate::getMDC(const std::string& key)
    {
        return ::log4cxx::MDC::get(key);
    }

    void CustomLoggerPrivate::removeMDC(const std::string& key)
    {
        ::log4cxx::MDC::remove(key);
    }

    void CustomLoggerPrivate::clearMDC()
    {
        ::log4cxx::MDC::clear();
    }

    // ============================================
    // NDC Support
    // ============================================

    void CustomLoggerPrivate::pushNDC(const std::string& message)
    {
        ::log4cxx::NDC::push(message);
    }

    std::string CustomLoggerPrivate::popNDC()
    {
        return ::log4cxx::NDC::pop();
    }

    std::size_t CustomLoggerPrivate::getNDCDepth()
    {
        return ::log4cxx::NDC::getDepth();
    }

    void CustomLoggerPrivate::clearNDC()
    {
        ::log4cxx::NDC::clear();
    }

}  // namespace com::github::doevelopper::atlassians::logging
