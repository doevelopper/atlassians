/**
 * @file PluginTypes.hpp
 * @brief Common types and enumerations for the Plugin Architecture Framework
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINTYPES_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINTYPES_HPP

#include <cstdint>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <any>

namespace com::github::doevelopper::atlassians::plugin
{
    // Forward declarations
    class IPlugin;
    class IPluginContext;
    class IPluginMetadata;

    /**
     * @brief Enumeration representing the lifecycle state of a plugin.
     *
     * Plugins transition through these states during their lifecycle:
     * Unloaded -> Loaded -> Initialized -> Started <-> Stopped -> Unloaded
     *                                        ^            |
     *                                        +------------+
     */
    enum class PluginState : std::uint8_t
    {
        Unloaded = 0,       ///< Plugin is not loaded
        Discovered,         ///< Plugin has been discovered but not yet loaded
        Loaded,             ///< Plugin library is loaded but not initialized
        Initialized,        ///< Plugin has been initialized and ready to start
        Starting,           ///< Plugin is in the process of starting
        Started,            ///< Plugin is actively running
        Stopping,           ///< Plugin is in the process of stopping
        Stopped,            ///< Plugin has been stopped but can be restarted
        Failed,             ///< Plugin has encountered an unrecoverable error
        Disabled            ///< Plugin is explicitly disabled
    };

    /**
     * @brief Enumeration for plugin isolation levels.
     */
    enum class IsolationLevel : std::uint8_t
    {
        None = 0,           ///< No isolation (same process, same thread)
        Thread,             ///< Thread-based isolation
        Process             ///< Process-based isolation (maximum security)
    };

    /**
     * @brief Enumeration for log levels used by plugins.
     */
    enum class LogLevel : std::uint8_t
    {
        Trace = 0,
        Debug,
        Info,
        Warning,
        Error,
        Critical,
        Fatal
    };

    /**
     * @brief Plugin priority levels for load ordering.
     */
    enum class PluginPriority : std::uint8_t
    {
        Critical = 0,       ///< Must load first (core system plugins)
        High = 1,
        Normal = 2,
        Low = 3,
        Optional = 4        ///< Can be loaded last or skipped
    };

    /**
     * @brief Plugin capability flags.
     */
    enum class PluginCapability : std::uint32_t
    {
        None = 0,
        HotReloadable = 1 << 0,         ///< Supports hot-reloading
        ThreadSafe = 1 << 1,            ///< All APIs are thread-safe
        StatePersistent = 1 << 2,       ///< Can persist state across restarts
        NetworkEnabled = 1 << 3,        ///< Requires network access
        FileSystemAccess = 1 << 4,      ///< Requires filesystem access
        UIProvider = 1 << 5,            ///< Provides UI components
        ServiceProvider = 1 << 6,       ///< Provides services to other plugins
        EventPublisher = 1 << 7,        ///< Publishes events
        EventSubscriber = 1 << 8,       ///< Subscribes to events
        Configurable = 1 << 9           ///< Has configurable options
    };

    /**
     * @brief Bitwise OR operator for PluginCapability.
     */
    [[nodiscard]] constexpr auto operator|(PluginCapability lhs, PluginCapability rhs) noexcept -> PluginCapability
    {
        return static_cast<PluginCapability>(
            static_cast<std::uint32_t>(lhs) | static_cast<std::uint32_t>(rhs)
        );
    }

    /**
     * @brief Bitwise AND operator for PluginCapability.
     */
    [[nodiscard]] constexpr auto operator&(PluginCapability lhs, PluginCapability rhs) noexcept -> PluginCapability
    {
        return static_cast<PluginCapability>(
            static_cast<std::uint32_t>(lhs) & static_cast<std::uint32_t>(rhs)
        );
    }

    /**
     * @brief Check if a capability is set.
     */
    [[nodiscard]] constexpr auto hasCapability(PluginCapability caps, PluginCapability flag) noexcept -> bool
    {
        return (caps & flag) == flag;
    }

    /**
     * @brief Structure representing a plugin dependency.
     */
    struct PluginDependency
    {
        std::string pluginName;              ///< Name of the required plugin
        std::string versionConstraint;       ///< Semantic version constraint (e.g., ">=1.2.0, <2.0.0")
        bool optional{false};                ///< Whether the dependency is optional

        /**
         * @brief Equality comparison operator (C++17 compatible).
         */
        [[nodiscard]] auto operator==(const PluginDependency& other) const noexcept -> bool
        {
            return pluginName == other.pluginName &&
                   versionConstraint == other.versionConstraint &&
                   optional == other.optional;
        }

        /**
         * @brief Inequality comparison operator (C++17 compatible).
         */
        [[nodiscard]] auto operator!=(const PluginDependency& other) const noexcept -> bool
        {
            return !(*this == other);
        }

        /**
         * @brief Less-than comparison operator for ordering (C++17 compatible).
         */
        [[nodiscard]] auto operator<(const PluginDependency& other) const noexcept -> bool
        {
            if (pluginName != other.pluginName)
                return pluginName < other.pluginName;
            if (versionConstraint != other.versionConstraint)
                return versionConstraint < other.versionConstraint;
            return optional < other.optional;
        }
    };

    /**
     * @brief Event payload type using std::variant for type safety.
     */
    using EventPayload = std::variant<
        std::monostate,
        bool,
        std::int64_t,
        double,
        std::string,
        std::vector<std::string>,
        std::any
    >;

    /**
     * @brief Structure representing a plugin event.
     */
    struct PluginEvent
    {
        std::string eventType;                                      ///< Event type identifier
        std::string sourcePlugin;                                   ///< Name of the plugin that published the event
        EventPayload payload;                                       ///< Event data
        std::chrono::system_clock::time_point timestamp;            ///< When the event was created
        std::uint64_t sequenceNumber{0};                            ///< Monotonically increasing sequence number
        bool propagate{true};                                       ///< Whether the event should propagate

        /**
         * @brief Default constructor.
         */
        PluginEvent() noexcept
            : timestamp(std::chrono::system_clock::now())
        {
        }

        /**
         * @brief Constructor with event type.
         */
        explicit PluginEvent(std::string type) noexcept
            : eventType(std::move(type))
            , timestamp(std::chrono::system_clock::now())
        {
        }

        /**
         * @brief Constructor with event type and payload.
         */
        PluginEvent(std::string type, EventPayload data) noexcept
            : eventType(std::move(type))
            , payload(std::move(data))
            , timestamp(std::chrono::system_clock::now())
        {
        }
    };

    /**
     * @brief Type alias for event handler callback.
     */
    using EventHandler = std::function<void(const PluginEvent&)>;

    /**
     * @brief Type alias for event listener (same as handler for compatibility).
     */
    using EventListener = EventHandler;

    /**
     * @brief Structure for plugin validation results.
     */
    struct ValidationResult
    {
        bool isValid{false};
        std::vector<std::string> errors;
        std::vector<std::string> warnings;

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return isValid;
        }
    };

    /**
     * @brief Structure for dependency resolution results.
     */
    struct ResolutionResult
    {
        bool success{false};
        std::vector<std::string> loadOrder;
        std::vector<std::string> missingDependencies;
        std::vector<std::string> circularDependencies;
        std::vector<std::string> versionConflicts;

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return success;
        }
    };

    /**
     * @brief Structure for plugin performance metrics.
     */
    struct PerformanceMetrics
    {
        std::chrono::nanoseconds initializationTime{0};
        std::chrono::nanoseconds averageEventProcessingTime{0};
        std::size_t memoryUsage{0};
        double cpuUsagePercent{0.0};
        std::size_t eventCount{0};
        std::chrono::system_clock::time_point lastActivityTime;
    };

    /**
     * @brief Convert PluginState to string representation.
     */
    [[nodiscard]] constexpr auto toString(PluginState state) noexcept -> std::string_view
    {
        switch (state)
        {
            case PluginState::Unloaded:     return "Unloaded";
            case PluginState::Discovered:   return "Discovered";
            case PluginState::Loaded:       return "Loaded";
            case PluginState::Initialized:  return "Initialized";
            case PluginState::Starting:     return "Starting";
            case PluginState::Started:      return "Started";
            case PluginState::Stopping:     return "Stopping";
            case PluginState::Stopped:      return "Stopped";
            case PluginState::Failed:       return "Failed";
            case PluginState::Disabled:     return "Disabled";
        }
        return "Unknown";
    }

    /**
     * @brief Convert LogLevel to string representation.
     */
    [[nodiscard]] constexpr auto toString(LogLevel level) noexcept -> std::string_view
    {
        switch (level)
        {
            case LogLevel::Trace:    return "TRACE";
            case LogLevel::Debug:    return "DEBUG";
            case LogLevel::Info:     return "INFO";
            case LogLevel::Warning:  return "WARNING";
            case LogLevel::Error:    return "ERROR";
            case LogLevel::Critical: return "CRITICAL";
            case LogLevel::Fatal:    return "FATAL";
        }
        return "UNKNOWN";
    }

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINTYPES_HPP
