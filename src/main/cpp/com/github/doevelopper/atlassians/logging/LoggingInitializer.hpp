/*******************************************************************
 * @file   LoggingInitializer.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Thread-safe singleton for Log4CXX initialization management
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZER_HPP

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

namespace com::github::doevelopper::atlassians::logging
{
    /**
     * @enum LogLevel
     * @brief Strongly typed log levels with extended categories
     */
    enum class LogLevel : std::uint32_t
    {
        /* Standard log levels */
        OFF      = 0,          ///< Logging disabled
        FATAL    = (1 << 0),   ///< Critical/fatal conditions
        ERROR    = (1 << 1),   ///< Error conditions
        WARN     = (1 << 2),   ///< Warning conditions
        INFO     = (1 << 3),   ///< Informational messages
        DEBUG    = (1 << 4),   ///< Debug-level messages
        TRACE    = (1 << 5),   ///< Trace-level messages (most verbose)
        ALL      = 0xFFFFFFFF  ///< All logging enabled
    };

    /**
     * @brief Combine log levels using bitwise OR
     */
    [[nodiscard]] constexpr LogLevel operator|(LogLevel lhs, LogLevel rhs) noexcept
    {
        return static_cast<LogLevel>(
            static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs));
    }

    /**
     * @brief Check if log level is enabled using bitwise AND
     */
    [[nodiscard]] constexpr bool operator&(LogLevel lhs, LogLevel rhs) noexcept
    {
        return (static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs)) != 0;
    }

    /**
     * @class LogFunctionScope
     * @brief RAII class to log function entry and exit
     *
     * Example usage:
     * @code
     * void myFunction() {
     *     LOG_FUNCTION_SCOPE("myFunction");
     *     // Function body
     * }
     * @endcode
     */
    class LogFunctionScope
    {
    public:
        explicit LogFunctionScope(std::string_view funcName) noexcept;
        ~LogFunctionScope() noexcept;

        LogFunctionScope(const LogFunctionScope&) = delete;
        LogFunctionScope& operator=(const LogFunctionScope&) = delete;
        LogFunctionScope(LogFunctionScope&&) = delete;
        LogFunctionScope& operator=(LogFunctionScope&&) = delete;

    private:
        std::string_view m_funcName;
    };

    // Note: LOG_FUNCTION_SCOPE macro is defined in LogMacros.hpp
    // Include LogMacros.hpp to use the comprehensive version with optional boost demangle support

    /**
     * @class LoggingInitializer
     * @brief Thread-safe singleton for managing Log4CXX initialization lifecycle
     *
     * This class follows the Singleton pattern with thread-safe lazy initialization.
     * It acts as the single point of initialization for the logging system and ensures
     * that initialization happens exactly once, regardless of how many times
     * initialize() is called.
     *
     * SOLID Principles:
     * - Single Responsibility: Manages only the logging system lifecycle
     * - Open/Closed: Extended via ILoggingInitializationStrategy implementations
     * - Dependency Inversion: Depends on ILoggingInitializationStrategy abstraction
     *
     * Thread Safety:
     * - Uses std::call_once for thread-safe initialization
     * - All public methods are thread-safe
     *
     * Example usage:
     * @code
     * // Using default strategy
     * LoggingInitializer::getInstance().initialize();
     *
     * // Using custom strategy
     * auto strategy = std::make_unique<PropertyFileBasedInitializationStrategy>("log4cxx.properties");
     * LoggingInitializer::getInstance().initialize(std::move(strategy));
     *
     * // Check initialization status
     * if (LoggingInitializer::getInstance().isInitialized()) {
     *     // Logging is ready
     * }
     *
     * // Shutdown on application exit
     * LoggingInitializer::getInstance().shutdown();
     * @endcode
     */
    class LoggingInitializer
    {
    public:
        /**
         * @brief Callback type for initialization events
         */
        using InitializationCallback = std::function<void(bool success, std::string_view message)>;

        /**
         * @brief Get the singleton instance
         * @return Reference to the singleton LoggingInitializer
         */
        [[nodiscard]] static LoggingInitializer& getInstance() noexcept;

        /**
         * @brief Delete copy constructor (Singleton)
         */
        LoggingInitializer(const LoggingInitializer&) = delete;

        /**
         * @brief Delete copy assignment (Singleton)
         */
        LoggingInitializer& operator=(const LoggingInitializer&) = delete;

        /**
         * @brief Delete move constructor (Singleton)
         */
        LoggingInitializer(LoggingInitializer&&) = delete;

        /**
         * @brief Delete move assignment (Singleton)
         */
        LoggingInitializer& operator=(LoggingInitializer&&) = delete;

        /**
         * @brief Initialize logging with a specific strategy
         * @param strategy The initialization strategy to use (takes ownership)
         * @return true if initialization succeeded, false if already initialized or failed
         * @throws LoggingInitializationException if initialization fails critically
         *
         * @note This method is thread-safe and idempotent - multiple calls will
         *       only initialize once with the first provided strategy.
         */
        bool initialize(std::unique_ptr<ILoggingInitializationStrategy> strategy);

        /**
         * @brief Initialize logging with default strategy
         * @return true if initialization succeeded
         *
         * Uses DefaultInitializationStrategy if no strategy is provided.
         */
        bool initialize();

        /**
         * @brief Check if logging system is initialized
         * @return true if Log4CXX is configured and ready
         */
        [[nodiscard]] bool isInitialized() const noexcept;

        /**
         * @brief Check LogManager configuration status
         * @return true if log4cxx::LogManager::getLoggerRepository()->isConfigured()
         *
         * This is a direct check of the underlying log4cxx status.
         */
        [[nodiscard]] bool checkLogManagerStatus() const noexcept;

        /**
         * @brief Shutdown the logging system
         *
         * Releases all resources, flushes pending logs, and resets configuration.
         * After shutdown, initialize() can be called again with a new strategy.
         */
        void shutdown() noexcept;

        /**
         * @brief Reset the logging configuration
         *
         * Clears current configuration but keeps the system initialized.
         * Useful for reconfiguration scenarios.
         */
        void reset() noexcept;

        /**
         * @brief Get the name of the current initialization strategy
         * @return Strategy name if initialized, nullopt otherwise
         */
        [[nodiscard]] std::optional<std::string_view> getCurrentStrategyName() const noexcept;

        /**
         * @brief Get the configuration source of the current strategy
         * @return Configuration source if available, nullopt otherwise
         */
        [[nodiscard]] std::optional<std::string> getConfigSource() const noexcept;

        /**
         * @brief Register a callback to be notified of initialization events
         * @param callback The callback function to register
         */
        void setInitializationCallback(InitializationCallback callback);

        /**
         * @brief Set the default log level for the root logger
         * @param level The log level to set
         */
        void setRootLogLevel(LogLevel level);

        /**
         * @brief Get the current root log level
         * @return Current log level of the root logger
         */
        [[nodiscard]] LogLevel getRootLogLevel() const noexcept;

    private:
        /**
         * @brief Private constructor (Singleton pattern)
         */
        LoggingInitializer() noexcept;

        /**
         * @brief Private destructor
         */
        ~LoggingInitializer() noexcept;

        /**
         * @brief Perform the actual initialization
         * @param strategy The strategy to use for initialization
         * @return true if successful
         */
        bool doInitialize(const ILoggingInitializationStrategy& strategy);

        /**
         * @brief Notify registered callback of initialization result
         */
        void notifyCallback(bool success, std::string_view message) const;

        /// Current initialization strategy (stored after successful initialization)
        std::unique_ptr<ILoggingInitializationStrategy> m_strategy;

        /// Flag for thread-safe one-time initialization
        mutable std::once_flag m_initFlag;

        /// Mutex for thread-safe operations
        mutable std::mutex m_mutex;

        /// Atomic flag indicating initialization status
        std::atomic<bool> m_initialized{false};

        /// Optional callback for initialization events
        InitializationCallback m_callback;
    };

}  // namespace com::github::doevelopper::atlassians::logging

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGGINGINITIALIZER_HPP
