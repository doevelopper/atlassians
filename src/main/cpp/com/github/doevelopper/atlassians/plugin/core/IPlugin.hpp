/**
 * @file IPlugin.hpp
 * @brief Core plugin interface that all plugins must implement
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGIN_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGIN_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginContext.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Core interface that all plugins must implement.
     *
     * This interface defines the contract between the plugin framework
     * and individual plugins. Plugins are loaded, initialized, started,
     * stopped, and unloaded according to a well-defined lifecycle.
     *
     * Plugin Lifecycle:
     * 1. Plugin is discovered and loaded (constructor called)
     * 2. initialize() is called with a context
     * 3. start() is called to begin plugin operation
     * 4. stop() is called to pause plugin operation
     * 5. shutdown() is called before unloading
     * 6. Plugin is unloaded (destructor called)
     *
     * @note Plugins should be designed to handle multiple start/stop cycles
     *       without requiring reinitialization.
     */
    class IPlugin
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPlugin() noexcept = default;

        // ============================================
        // Lifecycle Methods
        // ============================================

        /**
         * @brief Initialize the plugin with the provided context.
         *
         * This method is called once after the plugin is loaded. The plugin
         * should perform any setup required, such as:
         * - Registering services it provides
         * - Subscribing to events
         * - Loading configuration
         * - Initializing internal state
         *
         * @param context The plugin context providing access to host services.
         * @throws PluginInitializationException if initialization fails.
         */
        virtual auto initialize(IPluginContext& context) -> void = 0;

        /**
         * @brief Start the plugin.
         *
         * This method is called after initialization to begin plugin operation.
         * The plugin should start any background tasks, open connections, etc.
         *
         * @throws PluginStateException if the plugin cannot be started.
         */
        virtual auto start() -> void = 0;

        /**
         * @brief Stop the plugin.
         *
         * This method is called to pause plugin operation. The plugin should:
         * - Stop background tasks
         * - Close temporary connections
         * - Maintain state for potential restart
         *
         * After stop(), the plugin may be started again or shutdown.
         *
         * @throws PluginStateException if the plugin cannot be stopped.
         */
        virtual auto stop() -> void = 0;

        /**
         * @brief Shutdown the plugin.
         *
         * This method is called before the plugin is unloaded. The plugin should:
         * - Release all resources
         * - Unregister services
         * - Cancel subscriptions
         * - Save persistent state
         *
         * After shutdown(), the plugin will not be restarted without being
         * unloaded and reloaded first.
         */
        virtual auto shutdown() -> void = 0;

        // ============================================
        // Metadata Access
        // ============================================

        /**
         * @brief Get the plugin's metadata.
         * @return Reference to the plugin's metadata.
         */
        [[nodiscard]] virtual auto getMetadata() const noexcept -> const IPluginMetadata& = 0;

        /**
         * @brief Get the current state of the plugin.
         * @return The plugin's current lifecycle state.
         */
        [[nodiscard]] virtual auto getState() const noexcept -> PluginState = 0;

        // ============================================
        // Capability Queries
        // ============================================

        /**
         * @brief Check if the plugin has a specific capability.
         * @param capability The capability to check for.
         * @return True if the plugin has the capability.
         */
        [[nodiscard]] virtual auto hasCapability(std::string_view capability) const -> bool = 0;

        /**
         * @brief Get the list of services this plugin provides.
         * @return Vector of service identifiers.
         */
        [[nodiscard]] virtual auto getProvidedServices() const -> std::vector<std::string> = 0;

        /**
         * @brief Get the list of services this plugin requires.
         * @return Vector of required service identifiers.
         */
        [[nodiscard]] virtual auto getRequiredServices() const -> std::vector<std::string> = 0;

        // ============================================
        // Optional Lifecycle Extensions
        // ============================================

        /**
         * @brief Called when the plugin should save its state.
         *
         * This may be called during normal operation (e.g., before hot-reload)
         * or as part of an orderly shutdown.
         *
         * @return True if state was saved successfully.
         */
        [[nodiscard]] virtual auto saveState() -> bool
        {
            return true; // Default: no state to save
        }

        /**
         * @brief Called to restore previously saved state.
         *
         * This is called during initialization if there is state to restore
         * (e.g., after hot-reload).
         *
         * @return True if state was restored successfully.
         */
        [[nodiscard]] virtual auto restoreState() -> bool
        {
            return true; // Default: no state to restore
        }

        /**
         * @brief Called when the plugin should prepare for hot-reload.
         *
         * The plugin should save any state needed to continue after reload.
         */
        virtual auto prepareForHotReload() -> void
        {
            saveState();
        }

        /**
         * @brief Called after hot-reload is complete.
         *
         * The plugin should restore its state and resume operation.
         */
        virtual auto completeHotReload() -> void
        {
            restoreState();
        }

        // ============================================
        // Health Check
        // ============================================

        /**
         * @brief Check if the plugin is healthy.
         *
         * This method is called periodically to verify plugin health.
         *
         * @return True if the plugin is functioning normally.
         */
        [[nodiscard]] virtual auto isHealthy() const noexcept -> bool
        {
            return getState() == PluginState::Started;
        }

        /**
         * @brief Get a diagnostic message about the plugin's status.
         * @return Human-readable status message.
         */
        [[nodiscard]] virtual auto getDiagnostics() const -> std::string
        {
            return std::string("State: ") + std::string(toString(getState()));
        }

    protected:
        IPlugin() = default;
        IPlugin(const IPlugin&) = default;
        IPlugin(IPlugin&&) = default;
        IPlugin& operator=(const IPlugin&) = default;
        IPlugin& operator=(IPlugin&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGIN_HPP
