/**
 * @file IPluginContext.hpp
 * @brief Interface providing runtime context to plugins
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCONTEXT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCONTEXT_HPP

#include <any>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface providing runtime context and services to plugins.
     *
     * The plugin context is the primary way for plugins to interact with
     * the host application and other plugins. It provides access to:
     * - Host services and APIs
     * - Configuration settings
     * - File system paths
     * - Logging facilities
     * - Event publishing and subscription
     */
    class IPluginContext
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginContext() noexcept = default;

        // ============================================
        // Service Access
        // ============================================

        /**
         * @brief Get a service by name (type-erased).
         * @param serviceName The name of the service to retrieve.
         * @return Shared pointer to the service, or nullptr if not found.
         */
        [[nodiscard]] virtual auto getService(std::string_view serviceName)
            -> std::shared_ptr<void> = 0;

        /**
         * @brief Get a service by name with type safety.
         * @tparam T The expected type of the service.
         * @param serviceName The name of the service to retrieve.
         * @return Shared pointer to the typed service, or nullptr if not found.
         */
        template<typename T>
        [[nodiscard]] auto getService(std::string_view serviceName) -> std::shared_ptr<T>
        {
            return std::static_pointer_cast<T>(getService(serviceName));
        }

        /**
         * @brief Register a service provided by this plugin.
         * @param serviceName The name to register the service under.
         * @param service The service implementation.
         */
        virtual auto registerService(std::string_view serviceName,
                                      std::shared_ptr<void> service) -> void = 0;

        /**
         * @brief Register a typed service provided by this plugin.
         * @tparam T The type of the service.
         * @param serviceName The name to register the service under.
         * @param service The service implementation.
         */
        template<typename T>
        auto registerService(std::string_view serviceName, std::shared_ptr<T> service) -> void
        {
            registerService(serviceName, std::static_pointer_cast<void>(service));
        }

        /**
         * @brief Unregister a service.
         * @param serviceName The name of the service to unregister.
         */
        virtual auto unregisterService(std::string_view serviceName) -> void = 0;

        /**
         * @brief Check if a service is available.
         * @param serviceName The name of the service to check.
         * @return True if the service is registered.
         */
        [[nodiscard]] virtual auto hasService(std::string_view serviceName) const noexcept -> bool = 0;

        // ============================================
        // Configuration Access
        // ============================================

        /**
         * @brief Get a configuration value as string.
         * @param key The configuration key.
         * @return The value if found.
         */
        [[nodiscard]] virtual auto getConfigValue(std::string_view key) const
            -> std::optional<std::string> = 0;

        /**
         * @brief Get a configuration value with a default.
         * @param key The configuration key.
         * @param defaultValue The default value if key is not found.
         * @return The configuration value or default.
         */
        [[nodiscard]] virtual auto getConfigValue(std::string_view key,
                                                   std::string_view defaultValue) const
            -> std::string = 0;

        /**
         * @brief Get a typed configuration value.
         * @tparam T The expected type of the configuration value.
         * @param key The configuration key.
         * @return The typed value if found and convertible.
         */
        template<typename T>
        [[nodiscard]] auto getConfig(std::string_view key) const -> std::optional<T>;

        /**
         * @brief Set a configuration value.
         * @param key The configuration key.
         * @param value The value to set.
         */
        virtual auto setConfigValue(std::string_view key, std::string_view value) -> void = 0;

        // ============================================
        // Path Access
        // ============================================

        /**
         * @brief Get the plugin's installation directory.
         * @return Path to the directory containing the plugin.
         */
        [[nodiscard]] virtual auto getPluginDirectory() const noexcept
            -> std::filesystem::path = 0;

        /**
         * @brief Get the plugin's data directory.
         * @return Path to the directory for plugin-specific data storage.
         */
        [[nodiscard]] virtual auto getDataDirectory() const noexcept
            -> std::filesystem::path = 0;

        /**
         * @brief Get the plugin's configuration directory.
         * @return Path to the directory for plugin configuration files.
         */
        [[nodiscard]] virtual auto getConfigDirectory() const noexcept
            -> std::filesystem::path = 0;

        /**
         * @brief Get the plugin's temporary directory.
         * @return Path to a temporary directory for the plugin.
         */
        [[nodiscard]] virtual auto getTempDirectory() const noexcept
            -> std::filesystem::path = 0;

        /**
         * @brief Get the host application's installation directory.
         * @return Path to the host application's root directory.
         */
        [[nodiscard]] virtual auto getApplicationDirectory() const noexcept
            -> std::filesystem::path = 0;

        // ============================================
        // Logging
        // ============================================

        /**
         * @brief Log a message at the specified level.
         * @param level The log level.
         * @param message The message to log.
         */
        virtual auto log(LogLevel level, std::string_view message) -> void = 0;

        /**
         * @brief Log a trace message.
         * @param message The message to log.
         */
        auto logTrace(std::string_view message) -> void
        {
            log(LogLevel::Trace, message);
        }

        /**
         * @brief Log a debug message.
         * @param message The message to log.
         */
        auto logDebug(std::string_view message) -> void
        {
            log(LogLevel::Debug, message);
        }

        /**
         * @brief Log an info message.
         * @param message The message to log.
         */
        auto logInfo(std::string_view message) -> void
        {
            log(LogLevel::Info, message);
        }

        /**
         * @brief Log a warning message.
         * @param message The message to log.
         */
        auto logWarning(std::string_view message) -> void
        {
            log(LogLevel::Warning, message);
        }

        /**
         * @brief Log an error message.
         * @param message The message to log.
         */
        auto logError(std::string_view message) -> void
        {
            log(LogLevel::Error, message);
        }

        // ============================================
        // Event Handling
        // ============================================

        /**
         * @brief Publish an event.
         * @param event The event to publish.
         */
        virtual auto publishEvent(const PluginEvent& event) -> void = 0;

        /**
         * @brief Publish an event with type and payload.
         * @param eventType The type of event.
         * @param payload The event payload.
         */
        virtual auto publishEvent(std::string_view eventType, EventPayload payload = {}) -> void = 0;

        /**
         * @brief Subscribe to an event type.
         * @param eventType The type of events to subscribe to.
         * @param handler The handler to call when events occur.
         * @return A subscription ID for unsubscribing.
         */
        [[nodiscard]] virtual auto subscribeToEvent(std::string_view eventType,
                                                     EventHandler handler) -> std::uint64_t = 0;

        /**
         * @brief Unsubscribe from events.
         * @param subscriptionId The subscription ID returned from subscribeToEvent.
         */
        virtual auto unsubscribeFromEvent(std::uint64_t subscriptionId) -> void = 0;

        // ============================================
        // Plugin Interaction
        // ============================================

        /**
         * @brief Get the name of this plugin.
         * @return The plugin's name.
         */
        [[nodiscard]] virtual auto getPluginName() const noexcept -> std::string_view = 0;

        /**
         * @brief Request a reference to another loaded plugin.
         * @param pluginName The name of the plugin to access.
         * @return Weak pointer to the plugin, or empty if not found.
         */
        [[nodiscard]] virtual auto getPlugin(std::string_view pluginName) const
            -> std::weak_ptr<IPlugin> = 0;

        /**
         * @brief Get the names of all loaded plugins.
         * @return Vector of plugin names.
         */
        [[nodiscard]] virtual auto getLoadedPlugins() const -> std::vector<std::string> = 0;

        // ============================================
        // Host Information
        // ============================================

        /**
         * @brief Get the host application name.
         * @return The host application's name.
         */
        [[nodiscard]] virtual auto getHostName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the host application version.
         * @return The host application's version string.
         */
        [[nodiscard]] virtual auto getHostVersion() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the plugin API version supported by the host.
         * @return The plugin API version string.
         */
        [[nodiscard]] virtual auto getPluginApiVersion() const noexcept -> std::string_view = 0;

    protected:
        IPluginContext() = default;
        IPluginContext(const IPluginContext&) = default;
        IPluginContext(IPluginContext&&) = default;
        IPluginContext& operator=(const IPluginContext&) = default;
        IPluginContext& operator=(IPluginContext&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCONTEXT_HPP
