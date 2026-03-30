/*******************************************************************
 * @file   CustomLoggerPrivate.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Private implementation details for CustomLogger (PIMPL)
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGERPRIVATE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGERPRIVATE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include <log4cxx/logger.h>
#include <log4cxx/mdc.h>
#include <log4cxx/ndc.h>

namespace com::github::doevelopper::atlassians::logging
{
    // Forward declaration
    class CustomLogger;

    /**
     * @class CustomLoggerPrivate
     * @brief Private implementation class for CustomLogger
     *
     * This class encapsulates all log4cxx-specific details and provides
     * the actual logging functionality. It is hidden from the public API
     * using the PIMPL (Pointer to Implementation) idiom.
     *
     * Design principles:
     * - Encapsulation: All log4cxx details are hidden
     * - Binary compatibility: Changes don't affect ABI
     * - Clean separation: Public API vs implementation details
     */
    class CustomLoggerPrivate
    {
    public:
        /**
         * @brief Default constructor with root logger
         */
        CustomLoggerPrivate() noexcept;

        /**
         * @brief Constructor with named logger
         * @param loggerName Hierarchical logger name (e.g., "com.mycompany.module")
         */
        explicit CustomLoggerPrivate(std::string_view loggerName);

        /**
         * @brief Constructor with log4cxx LoggerPtr
         * @param logger The log4cxx logger pointer
         */
        explicit CustomLoggerPrivate(const ::log4cxx::LoggerPtr& logger) noexcept;

        /**
         * @brief Copy constructor
         */
        CustomLoggerPrivate(const CustomLoggerPrivate& other);

        /**
         * @brief Move constructor
         */
        CustomLoggerPrivate(CustomLoggerPrivate&& other) noexcept;

        /**
         * @brief Copy assignment
         */
        CustomLoggerPrivate& operator=(const CustomLoggerPrivate& other);

        /**
         * @brief Move assignment
         */
        CustomLoggerPrivate& operator=(CustomLoggerPrivate&& other) noexcept;

        /**
         * @brief Destructor
         */
        ~CustomLoggerPrivate() noexcept;

        // ============================================
        // Logging Methods
        // ============================================

        void trace(const std::string& message) const;
        void debug(const std::string& message) const;
        void info(const std::string& message) const;
        void warn(const std::string& message) const;
        void error(const std::string& message) const;
        void fatal(const std::string& message) const;

        /**
         * @brief Log with specific location information
         */
        void logWithLocation(
            const ::log4cxx::LevelPtr& level,
            const std::string& message,
            const char* file,
            int line,
            const char* func) const;

        // ============================================
        // Level Checking
        // ============================================

        [[nodiscard]] bool isTraceEnabled() const noexcept;
        [[nodiscard]] bool isDebugEnabled() const noexcept;
        [[nodiscard]] bool isInfoEnabled() const noexcept;
        [[nodiscard]] bool isWarnEnabled() const noexcept;
        [[nodiscard]] bool isErrorEnabled() const noexcept;
        [[nodiscard]] bool isFatalEnabled() const noexcept;

        /**
         * @brief Check if a specific level is enabled
         */
        [[nodiscard]] bool isEnabledFor(const ::log4cxx::LevelPtr& level) const noexcept;

        // ============================================
        // Logger Properties
        // ============================================

        /**
         * @brief Get the logger name
         */
        [[nodiscard]] std::string getName() const;

        /**
         * @brief Get the current effective level
         */
        [[nodiscard]] ::log4cxx::LevelPtr getLevel() const;

        /**
         * @brief Get the effective level (considering parent hierarchy)
         */
        [[nodiscard]] ::log4cxx::LevelPtr getEffectiveLevel() const;

        /**
         * @brief Set the logger level
         */
        void setLevel(const ::log4cxx::LevelPtr& level);

        /**
         * @brief Get the underlying log4cxx logger
         */
        [[nodiscard]] ::log4cxx::LoggerPtr getLogger() const noexcept;

        /**
         * @brief Check if logger is valid
         */
        [[nodiscard]] bool isValid() const noexcept;

        // ============================================
        // MDC (Mapped Diagnostic Context) Support
        // ============================================

        /**
         * @brief Put a value in MDC
         */
        static void putMDC(const std::string& key, const std::string& value);

        /**
         * @brief Get a value from MDC
         */
        [[nodiscard]] static std::string getMDC(const std::string& key);

        /**
         * @brief Remove a value from MDC
         */
        static void removeMDC(const std::string& key);

        /**
         * @brief Clear all MDC values
         */
        static void clearMDC();

        // ============================================
        // NDC (Nested Diagnostic Context) Support
        // ============================================

        /**
         * @brief Push a message onto the NDC stack
         */
        static void pushNDC(const std::string& message);

        /**
         * @brief Pop a message from the NDC stack
         */
        static std::string popNDC();

        /**
         * @brief Get the current NDC depth
         */
        [[nodiscard]] static std::size_t getNDCDepth();

        /**
         * @brief Clear the NDC stack
         */
        static void clearNDC();

    private:
        /**
         * @brief Log a message at the specified level
         */
        void log(const ::log4cxx::LevelPtr& level, const std::string& message) const;

        ::log4cxx::LoggerPtr m_logger;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_CUSTOMLOGGERPRIVATE_HPP
