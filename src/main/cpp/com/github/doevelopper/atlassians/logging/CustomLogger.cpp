/*******************************************************************
 * @file   CustomLogger.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Modern C++ logging facade with PIMPL idiom - Implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/CustomLogger.hpp>
#include <com/github/doevelopper/atlassians/logging/CustomLoggerPrivate.hpp>

#include <log4cxx/mdc.h>
#include <log4cxx/ndc.h>

namespace com::github::doevelopper::atlassians::logging
{
    // ============================================
    // MDCScope Implementation
    // ============================================

    CustomLogger::MDCScope::MDCScope(std::string key, std::string value)
        : m_key(std::move(key))
        , m_active(true)
    {
        log4cxx::MDC::put(m_key, value);
    }

    CustomLogger::MDCScope::~MDCScope() noexcept
    {
        if (m_active)
        {
            try
            {
                log4cxx::MDC::remove(m_key);
            }
            catch (...)
            {
                // Swallow exceptions in destructor
            }
        }
    }

    CustomLogger::MDCScope::MDCScope(MDCScope&& other) noexcept
        : m_key(std::move(other.m_key))
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    // ============================================
    // NDCScope Implementation
    // ============================================

    CustomLogger::NDCScope::NDCScope(std::string message)
        : m_active(true)
    {
        log4cxx::NDC::push(message);
    }

    CustomLogger::NDCScope::~NDCScope() noexcept
    {
        if (m_active)
        {
            try
            {
                log4cxx::NDC::pop();
            }
            catch (...)
            {
                // Swallow exceptions in destructor
            }
        }
    }

    CustomLogger::NDCScope::NDCScope(NDCScope&& other) noexcept
        : m_active(other.m_active)
    {
        other.m_active = false;
    }

    // ============================================
    // LogStream Implementation
    // ============================================

    namespace
    {
        constexpr int LEVEL_TRACE = 0;
        constexpr int LEVEL_DEBUG = 1;
        constexpr int LEVEL_INFO = 2;
        constexpr int LEVEL_WARN = 3;
        constexpr int LEVEL_ERROR = 4;
        constexpr int LEVEL_FATAL = 5;
    }

    CustomLogger::LogStream::LogStream(const CustomLogger& logger, int level)
        : m_logger(logger)
        , m_level(level)
        , m_enabled(false)
    {
        switch (level)
        {
            case LEVEL_TRACE: m_enabled = logger.isTraceEnabled(); break;
            case LEVEL_DEBUG: m_enabled = logger.isDebugEnabled(); break;
            case LEVEL_INFO: m_enabled = logger.isInfoEnabled(); break;
            case LEVEL_WARN: m_enabled = logger.isWarnEnabled(); break;
            case LEVEL_ERROR: m_enabled = logger.isErrorEnabled(); break;
            case LEVEL_FATAL: m_enabled = logger.isFatalEnabled(); break;
            default: break;
        }
    }

    CustomLogger::LogStream::~LogStream()
    {
        if (m_enabled && !m_stream.str().empty())
        {
            std::string message = m_stream.str();
            switch (m_level)
            {
                case LEVEL_TRACE: m_logger.trace(message); break;
                case LEVEL_DEBUG: m_logger.debug(message); break;
                case LEVEL_INFO: m_logger.info(message); break;
                case LEVEL_WARN: m_logger.warn(message); break;
                case LEVEL_ERROR: m_logger.error(message); break;
                case LEVEL_FATAL: m_logger.fatal(message); break;
                default: break;
            }
        }
    }

    CustomLogger::LogStream::LogStream(LogStream&& other) noexcept
        : m_logger(other.m_logger)
        , m_stream(std::move(other.m_stream))
        , m_level(other.m_level)
        , m_enabled(other.m_enabled)
    {
        other.m_enabled = false;
    }

    // ============================================
    // Static Factory Methods
    // ============================================

    CustomLogger CustomLogger::getLogger(std::string_view name)
    {
        auto impl = std::make_unique<CustomLoggerPrivate>(std::string(name));
        return CustomLogger(std::move(impl));
    }

    CustomLogger CustomLogger::getRootLogger()
    {
        auto impl = std::make_unique<CustomLoggerPrivate>();
        return CustomLogger(std::move(impl));
    }

    // ============================================
    // Rule of Five
    // ============================================

    CustomLogger::CustomLogger() noexcept
        : m_impl(std::make_unique<CustomLoggerPrivate>())
    {
    }

    CustomLogger::CustomLogger(const CustomLogger& other)
        : m_impl(other.m_impl ? std::make_unique<CustomLoggerPrivate>(*other.m_impl) : nullptr)
    {
    }

    CustomLogger::CustomLogger(CustomLogger&& other) noexcept
        : m_impl(std::move(other.m_impl))
    {
    }

    CustomLogger& CustomLogger::operator=(const CustomLogger& other)
    {
        if (this != &other)
        {
            m_impl = other.m_impl ? std::make_unique<CustomLoggerPrivate>(*other.m_impl) : nullptr;
        }
        return *this;
    }

    CustomLogger& CustomLogger::operator=(CustomLogger&& other) noexcept
    {
        if (this != &other)
        {
            m_impl = std::move(other.m_impl);
        }
        return *this;
    }

    CustomLogger::~CustomLogger() noexcept = default;

    CustomLogger::CustomLogger(std::unique_ptr<CustomLoggerPrivate> impl)
        : m_impl(std::move(impl))
    {
    }

    // ============================================
    // Basic Logging Methods
    // ============================================

    void CustomLogger::trace(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->trace(std::string(message));
        }
    }

    void CustomLogger::debug(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->debug(std::string(message));
        }
    }

    void CustomLogger::info(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->info(std::string(message));
        }
    }

    void CustomLogger::warn(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->warn(std::string(message));
        }
    }

    void CustomLogger::error(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->error(std::string(message));
        }
    }

    void CustomLogger::fatal(std::string_view message) const
    {
        if (m_impl)
        {
            m_impl->fatal(std::string(message));
        }
    }

    // ============================================
    // Stream-Style Logging
    // ============================================

    CustomLogger::LogStream CustomLogger::trace() const
    {
        return LogStream(*this, LEVEL_TRACE);
    }

    CustomLogger::LogStream CustomLogger::debug() const
    {
        return LogStream(*this, LEVEL_DEBUG);
    }

    CustomLogger::LogStream CustomLogger::info() const
    {
        return LogStream(*this, LEVEL_INFO);
    }

    CustomLogger::LogStream CustomLogger::warn() const
    {
        return LogStream(*this, LEVEL_WARN);
    }

    CustomLogger::LogStream CustomLogger::error() const
    {
        return LogStream(*this, LEVEL_ERROR);
    }

    CustomLogger::LogStream CustomLogger::fatal() const
    {
        return LogStream(*this, LEVEL_FATAL);
    }

    // ============================================
    // Level Checking
    // ============================================

    bool CustomLogger::isTraceEnabled() const noexcept
    {
        return m_impl && m_impl->isTraceEnabled();
    }

    bool CustomLogger::isDebugEnabled() const noexcept
    {
        return m_impl && m_impl->isDebugEnabled();
    }

    bool CustomLogger::isInfoEnabled() const noexcept
    {
        return m_impl && m_impl->isInfoEnabled();
    }

    bool CustomLogger::isWarnEnabled() const noexcept
    {
        return m_impl && m_impl->isWarnEnabled();
    }

    bool CustomLogger::isErrorEnabled() const noexcept
    {
        return m_impl && m_impl->isErrorEnabled();
    }

    bool CustomLogger::isFatalEnabled() const noexcept
    {
        return m_impl && m_impl->isFatalEnabled();
    }

    // ============================================
    // Logger Properties
    // ============================================

    std::string CustomLogger::getName() const
    {
        return m_impl ? m_impl->getName() : std::string{};
    }

    bool CustomLogger::isValid() const noexcept
    {
        return m_impl != nullptr && m_impl->isValid();
    }

    // ============================================
    // Static MDC/NDC Methods
    // ============================================

    void CustomLogger::putMDC(const std::string& key, const std::string& value)
    {
        CustomLoggerPrivate::putMDC(key, value);
    }

    std::string CustomLogger::getMDC(const std::string& key)
    {
        return CustomLoggerPrivate::getMDC(key);
    }

    void CustomLogger::removeMDC(const std::string& key)
    {
        CustomLoggerPrivate::removeMDC(key);
    }

    void CustomLogger::clearMDC()
    {
        CustomLoggerPrivate::clearMDC();
    }

    void CustomLogger::pushNDC(const std::string& message)
    {
        CustomLoggerPrivate::pushNDC(message);
    }

    std::string CustomLogger::popNDC()
    {
        return CustomLoggerPrivate::popNDC();
    }

    void CustomLogger::clearNDC()
    {
        CustomLoggerPrivate::clearNDC();
    }

    // ============================================
    // Format Message Implementation (base case)
    // ============================================

    void CustomLogger::formatMessageImpl(std::ostringstream& oss, std::string_view format)
    {
        oss << format;
    }

}  // namespace com::github::doevelopper::atlassians::logging
