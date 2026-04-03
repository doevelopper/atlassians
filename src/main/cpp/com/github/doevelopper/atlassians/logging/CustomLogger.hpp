/*******************************************************************
 * @file   CustomLogger.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Modern C++ logging facade with PIMPL idiom
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGER_HPP

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include <com/github/doevelopper/atlassians/API_Export.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    // Forward declarations
    class CustomLoggerPrivate;

    /**
     * @class CustomLogger
     * @brief Modern C++ logging facade wrapping log4cxx
     *
     * This class provides a clean, type-safe, and thread-safe logging API
     * using the PIMPL idiom to hide log4cxx implementation details.
     *
     * Features:
     * - Type-safe logging methods (trace, debug, info, warn, error, fatal)
     * - Variadic template support for formatted logging
     * - Stream-style logging with operator<<
     * - MDC (Mapped Diagnostic Context) support via RAII scopes
     * - NDC (Nested Diagnostic Context) support
     * - Level checking to avoid unnecessary string construction
     * - Move semantics support
     * - Exception-safe operations
     *
     * Example usage:
     * @code
     * // Get a logger
     * auto logger = CustomLogger::getLogger("com.mycompany.module");
     *
     * // Simple logging
     * logger.info("Application started");
     *
     * // Formatted logging
     * logger.info("Processing {} items", itemCount);
     *
     * // Stream-style logging
     * logger.debug() << "Value: " << value << ", Status: " << status;
     *
     * // MDC scope (automatic cleanup)
     * {
     *     CustomLogger::MDCScope scope("requestId", "12345");
     *     logger.info("Processing request");  // MDC value included in output
     * }  // MDC value automatically removed
     *
     * // Level checking (avoid unnecessary work)
     * if (logger.isDebugEnabled()) {
     *     logger.debug("Expensive computation: {}", computeExpensiveValue());
     * }
     * @endcode
     */
    class SDLC_API CustomLogger
    {
    public:
        /**
         * @class MDCScope
         * @brief RAII class for MDC (Mapped Diagnostic Context) management
         *
         * Automatically removes the MDC value when the scope ends.
         */
        class MDCScope
        {
        public:
            /**
             * @brief Constructor - adds key/value to MDC
             * @param key The MDC key
             * @param value The MDC value
             */
            MDCScope(std::string key, std::string value);

            /**
             * @brief Destructor - removes key from MDC
             */
            ~MDCScope() noexcept;

            MDCScope(const MDCScope&) = delete;
            MDCScope& operator=(const MDCScope&) = delete;
            MDCScope(MDCScope&& other) noexcept;
            MDCScope& operator=(MDCScope&&) = delete;

        private:
            std::string m_key;
            bool m_active;
        };

        /**
         * @class NDCScope
         * @brief RAII class for NDC (Nested Diagnostic Context) management
         *
         * Automatically pops the NDC value when the scope ends.
         */
        class NDCScope
        {
        public:
            /**
             * @brief Constructor - pushes message onto NDC stack
             * @param message The message to push
             */
            explicit NDCScope(std::string message);

            /**
             * @brief Destructor - pops from NDC stack
             */
            ~NDCScope() noexcept;

            NDCScope(const NDCScope&) = delete;
            NDCScope& operator=(const NDCScope&) = delete;
            NDCScope(NDCScope&& other) noexcept;
            NDCScope& operator=(NDCScope&&) = delete;

        private:
            bool m_active;
        };

        /**
         * @class LogStream
         * @brief Stream-style logging helper
         *
         * Allows stream-style logging syntax. The message is logged
         * when the LogStream is destroyed.
         */
        class LogStream
        {
        public:
            LogStream(const CustomLogger& logger, int level);
            ~LogStream();

            LogStream(const LogStream&) = delete;
            LogStream& operator=(const LogStream&) = delete;
            LogStream(LogStream&& other) noexcept;
            LogStream& operator=(LogStream&&) = delete;

            template<typename T>
            LogStream& operator<<(const T& value)
            {
                if (m_enabled)
                {
                    m_stream << value;
                }
                return *this;
            }

        private:
            const CustomLogger& m_logger;
            std::ostringstream m_stream;
            int m_level;
            bool m_enabled;
        };

        // ============================================
        // Static Factory Methods
        // ============================================

        /**
         * @brief Get a logger with the specified name
         * @param name Hierarchical logger name (e.g., "com.mycompany.module")
         * @return CustomLogger instance
         */
        [[nodiscard]] static CustomLogger getLogger(std::string_view name);

        /**
         * @brief Get the root logger
         * @return CustomLogger instance for root logger
         */
        [[nodiscard]] static CustomLogger getRootLogger();

        /**
         * @brief Get a logger using the class type name
         * @tparam T The class type
         * @return CustomLogger instance named after the type
         */
        template<typename T>
        [[nodiscard]] static CustomLogger getLogger()
        {
            return getLogger(typeid(T).name());
        }

        // ============================================
        // Rule of Five
        // ============================================

        /**
         * @brief Default constructor (creates root logger)
         */
        CustomLogger() noexcept;

        /**
         * @brief Copy constructor
         */
        CustomLogger(const CustomLogger& other);

        /**
         * @brief Move constructor
         */
        CustomLogger(CustomLogger&& other) noexcept;

        /**
         * @brief Copy assignment
         */
        CustomLogger& operator=(const CustomLogger& other);

        /**
         * @brief Move assignment
         */
        CustomLogger& operator=(CustomLogger&& other) noexcept;

        /**
         * @brief Destructor
         */
        ~CustomLogger() noexcept;

        // ============================================
        // Basic Logging Methods
        // ============================================

        /**
         * @brief Log a trace message
         * @param message The message to log
         */
        void trace(std::string_view message) const;

        /**
         * @brief Log a debug message
         * @param message The message to log
         */
        void debug(std::string_view message) const;

        /**
         * @brief Log an info message
         * @param message The message to log
         */
        void info(std::string_view message) const;

        /**
         * @brief Log a warning message
         * @param message The message to log
         */
        void warn(std::string_view message) const;

        /**
         * @brief Log an error message
         * @param message The message to log
         */
        void error(std::string_view message) const;

        /**
         * @brief Log a fatal message
         * @param message The message to log
         */
        void fatal(std::string_view message) const;

        // ============================================
        // Variadic Template Logging
        // ============================================

        /**
         * @brief Log a formatted trace message
         * @tparam Args Argument types
         * @param format Format string with {} placeholders
         * @param args Arguments to format
         */
        template<typename... Args>
        void trace(std::string_view format, Args&&... args) const
        {
            if (isTraceEnabled())
            {
                trace(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        /**
         * @brief Log a formatted debug message
         */
        template<typename... Args>
        void debug(std::string_view format, Args&&... args) const
        {
            if (isDebugEnabled())
            {
                debug(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        /**
         * @brief Log a formatted info message
         */
        template<typename... Args>
        void info(std::string_view format, Args&&... args) const
        {
            if (isInfoEnabled())
            {
                info(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        /**
         * @brief Log a formatted warning message
         */
        template<typename... Args>
        void warn(std::string_view format, Args&&... args) const
        {
            if (isWarnEnabled())
            {
                warn(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        /**
         * @brief Log a formatted error message
         */
        template<typename... Args>
        void error(std::string_view format, Args&&... args) const
        {
            if (isErrorEnabled())
            {
                error(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        /**
         * @brief Log a formatted fatal message
         */
        template<typename... Args>
        void fatal(std::string_view format, Args&&... args) const
        {
            if (isFatalEnabled())
            {
                fatal(formatMessage(format, std::forward<Args>(args)...));
            }
        }

        // ============================================
        // Stream-Style Logging
        // ============================================

        /**
         * @brief Get a stream for trace-level logging
         * @return LogStream that logs on destruction
         */
        [[nodiscard]] LogStream trace() const;

        /**
         * @brief Get a stream for debug-level logging
         */
        [[nodiscard]] LogStream debug() const;

        /**
         * @brief Get a stream for info-level logging
         */
        [[nodiscard]] LogStream info() const;

        /**
         * @brief Get a stream for warn-level logging
         */
        [[nodiscard]] LogStream warn() const;

        /**
         * @brief Get a stream for error-level logging
         */
        [[nodiscard]] LogStream error() const;

        /**
         * @brief Get a stream for fatal-level logging
         */
        [[nodiscard]] LogStream fatal() const;

        // ============================================
        // Level Checking
        // ============================================

        [[nodiscard]] bool isTraceEnabled() const noexcept;
        [[nodiscard]] bool isDebugEnabled() const noexcept;
        [[nodiscard]] bool isInfoEnabled() const noexcept;
        [[nodiscard]] bool isWarnEnabled() const noexcept;
        [[nodiscard]] bool isErrorEnabled() const noexcept;
        [[nodiscard]] bool isFatalEnabled() const noexcept;

        // ============================================
        // Logger Properties
        // ============================================

        /**
         * @brief Get the logger name
         * @return Logger name string
         */
        [[nodiscard]] std::string getName() const;

        /**
         * @brief Check if the logger is valid
         * @return true if the logger is properly initialized
         */
        [[nodiscard]] bool isValid() const noexcept;

        // ============================================
        // Static MDC/NDC Methods
        // ============================================

        /**
         * @brief Put a value in MDC
         * @param key The key
         * @param value The value
         */
        static void putMDC(const std::string& key, const std::string& value);

        /**
         * @brief Get a value from MDC
         * @param key The key
         * @return The value, or empty string if not found
         */
        [[nodiscard]] static std::string getMDC(const std::string& key);

        /**
         * @brief Remove a value from MDC
         * @param key The key to remove
         */
        static void removeMDC(const std::string& key);

        /**
         * @brief Clear all MDC values
         */
        static void clearMDC();

        /**
         * @brief Push a message onto the NDC stack
         * @param message The message to push
         */
        static void pushNDC(const std::string& message);

        /**
         * @brief Pop a message from the NDC stack
         * @return The popped message
         */
        static std::string popNDC();

        /**
         * @brief Clear the NDC stack
         */
        static void clearNDC();

    private:
        /**
         * @brief Private constructor with implementation
         */
        explicit CustomLogger(std::unique_ptr<CustomLoggerPrivate> impl);

        /**
         * @brief Format a message with arguments
         */
        template<typename... Args>
        static std::string formatMessage(std::string_view format, Args&&... args)
        {
            std::ostringstream oss;
            formatMessageImpl(oss, format, std::forward<Args>(args)...);
            return oss.str();
        }

        /**
         * @brief Format message implementation (base case)
         */
        static void formatMessageImpl(std::ostringstream& oss, std::string_view format);

        /**
         * @brief Format message implementation (recursive)
         */
        template<typename T, typename... Rest>
        static void formatMessageImpl(std::ostringstream& oss, std::string_view format,
                                       T&& value, Rest&&... rest)
        {
            auto pos = format.find("{}");
            if (pos != std::string_view::npos)
            {
                oss << format.substr(0, pos) << std::forward<T>(value);
                formatMessageImpl(oss, format.substr(pos + 2), std::forward<Rest>(rest)...);
            }
            else
            {
                oss << format;
            }
        }

        std::unique_ptr<CustomLoggerPrivate> m_impl;
    };

    // ============================================
    // Logging Macros (optional, for source location)
    // ============================================

    /**
     * @def LOG_TRACE
     * @brief Log trace message with source location
     */
    #define LOG_TRACE(logger, msg) \
        do { if ((logger).isTraceEnabled()) (logger).trace(msg); } while(0)

    /**
     * @def LOG_DEBUG
     * @brief Log debug message with source location
     */
    #define LOG_DEBUG(logger, msg) \
        do { if ((logger).isDebugEnabled()) (logger).debug(msg); } while(0)

    /**
     * @def LOG_INFO
     * @brief Log info message with source location
     */
    #define LOG_INFO(logger, msg) \
        do { if ((logger).isInfoEnabled()) (logger).info(msg); } while(0)

    /**
     * @def LOG_WARN
     * @brief Log warning message with source location
     */
    #define LOG_WARN(logger, msg) \
        do { if ((logger).isWarnEnabled()) (logger).warn(msg); } while(0)

    /**
     * @def LOG_ERROR
     * @brief Log error message with source location
     */
    #define LOG_ERROR(logger, msg) \
        do { if ((logger).isErrorEnabled()) (logger).error(msg); } while(0)

    /**
     * @def LOG_FATAL
     * @brief Log fatal message with source location
     */
    #define LOG_FATAL(logger, msg) \
        do { if ((logger).isFatalEnabled()) (logger).fatal(msg); } while(0)

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGER_HPP
