/**
 * @file IPluginRegistry.hpp
 * @brief Interface for plugin registration and discovery
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINREGISTRY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINREGISTRY_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for plugin registration and discovery.
     *
     * The plugin registry maintains a catalog of all known plugins,
     * both loaded and available. It provides:
     * - Plugin registration and unregistration
     * - Plugin lookup by name, capability, or service
     * - Plugin state tracking
     * - Metadata caching
     */
    class IPluginRegistry
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginRegistry() noexcept = default;

        // ============================================
        // Plugin Registration
        // ============================================

        /**
         * @brief Register a plugin with its metadata.
         * @param metadata The plugin's metadata.
         * @param plugin The plugin instance (may be nullptr for discovered but unloaded plugins).
         * @throws PluginException if registration fails.
         */
        virtual auto registerPlugin(std::unique_ptr<IPluginMetadata> metadata,
                                     std::shared_ptr<IPlugin> plugin = nullptr) -> void = 0;

        /**
         * @brief Unregister a plugin.
         * @param name The name of the plugin to unregister.
         * @throws PluginNotFoundException if the plugin is not registered.
         */
        virtual auto unregisterPlugin(std::string_view name) -> void = 0;

        /**
         * @brief Update the plugin instance for a registered plugin.
         * @param name The name of the plugin.
         * @param plugin The new plugin instance.
         * @throws PluginNotFoundException if the plugin is not registered.
         */
        virtual auto updatePlugin(std::string_view name,
                                   std::shared_ptr<IPlugin> plugin) -> void = 0;

        // ============================================
        // Plugin Queries
        // ============================================

        /**
         * @brief Check if a plugin is registered.
         * @param name The plugin name.
         * @return True if the plugin is registered.
         */
        [[nodiscard]] virtual auto isRegistered(std::string_view name) const noexcept -> bool = 0;

        /**
         * @brief Get a plugin by name.
         * @param name The plugin name.
         * @return Shared pointer to the plugin, or nullptr if not found.
         */
        [[nodiscard]] virtual auto getPlugin(std::string_view name) const
            -> std::shared_ptr<IPlugin> = 0;

        /**
         * @brief Get plugin metadata by name.
         * @param name The plugin name.
         * @return Pointer to the metadata, or nullptr if not found.
         */
        [[nodiscard]] virtual auto getMetadata(std::string_view name) const
            -> const IPluginMetadata* = 0;

        /**
         * @brief Get all registered plugin metadata.
         * @return Vector of all plugin metadata.
         */
        [[nodiscard]] virtual auto getAllMetadata() const
            -> std::vector<const IPluginMetadata*> = 0;

        // ============================================
        // State Queries
        // ============================================

        /**
         * @brief Get the state of a plugin.
         * @param name The plugin name.
         * @return The plugin's state, or Unloaded if not found.
         */
        [[nodiscard]] virtual auto getPluginState(std::string_view name) const noexcept
            -> PluginState = 0;

        /**
         * @brief Set the state of a plugin.
         * @param name The plugin name.
         * @param state The new state.
         */
        virtual auto setPluginState(std::string_view name, PluginState state) -> void = 0;

        /**
         * @brief Get all plugins in a specific state.
         * @param state The state to filter by.
         * @return Names of plugins in the specified state.
         */
        [[nodiscard]] virtual auto getPluginsByState(PluginState state) const
            -> std::vector<std::string> = 0;

        // ============================================
        // Capability and Service Queries
        // ============================================

        /**
         * @brief Find plugins by capability.
         * @param capability The capability to search for.
         * @return Names of plugins with the specified capability.
         */
        [[nodiscard]] virtual auto findByCapability(std::string_view capability) const
            -> std::vector<std::string> = 0;

        /**
         * @brief Find plugins that provide a specific service.
         * @param service The service identifier.
         * @return Names of plugins providing the service.
         */
        [[nodiscard]] virtual auto findByService(std::string_view service) const
            -> std::vector<std::string> = 0;

        /**
         * @brief Find plugins that require a specific service.
         * @param service The service identifier.
         * @return Names of plugins requiring the service.
         */
        [[nodiscard]] virtual auto findByRequiredService(std::string_view service) const
            -> std::vector<std::string> = 0;

        // ============================================
        // Listing
        // ============================================

        /**
         * @brief Get all registered plugin names.
         * @return Vector of all plugin names.
         */
        [[nodiscard]] virtual auto getAllPluginNames() const -> std::vector<std::string> = 0;

        /**
         * @brief Get the count of registered plugins.
         * @return Number of registered plugins.
         */
        [[nodiscard]] virtual auto getPluginCount() const noexcept -> std::size_t = 0;

        /**
         * @brief Get the count of loaded plugins.
         * @return Number of plugins that are currently loaded.
         */
        [[nodiscard]] virtual auto getLoadedPluginCount() const noexcept -> std::size_t = 0;

        // ============================================
        // Iteration
        // ============================================

        /**
         * @brief Type alias for plugin visitor callback.
         */
        using PluginVisitor = std::function<void(const std::string&,
                                                  const IPluginMetadata&,
                                                  std::shared_ptr<IPlugin>)>;

        /**
         * @brief Iterate over all registered plugins.
         * @param visitor Callback function for each plugin.
         */
        virtual auto forEachPlugin(const PluginVisitor& visitor) const -> void = 0;

        // ============================================
        // Registry Events
        // ============================================

        /**
         * @brief Type alias for registry change callback.
         */
        using RegistryChangeCallback = std::function<void(std::string_view pluginName,
                                                           PluginState newState)>;

        /**
         * @brief Register a callback for registry changes.
         * @param callback The callback to invoke on changes.
         * @return A handle for unregistering the callback.
         */
        [[nodiscard]] virtual auto onRegistryChange(RegistryChangeCallback callback)
            -> std::uint64_t = 0;

        /**
         * @brief Unregister a change callback.
         * @param handle The handle returned from onRegistryChange.
         */
        virtual auto removeRegistryChangeCallback(std::uint64_t handle) -> void = 0;

    protected:
        IPluginRegistry() = default;
        IPluginRegistry(const IPluginRegistry&) = default;
        IPluginRegistry(IPluginRegistry&&) = default;
        IPluginRegistry& operator=(const IPluginRegistry&) = default;
        IPluginRegistry& operator=(IPluginRegistry&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINREGISTRY_HPP
