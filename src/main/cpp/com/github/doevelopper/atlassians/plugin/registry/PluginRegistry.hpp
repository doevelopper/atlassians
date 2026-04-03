/**
 * @file PluginRegistry.hpp
 * @brief Concrete plugin registry implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_REGISTRY_PLUGINREGISTRY_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_REGISTRY_PLUGINREGISTRY_HPP

#include <atomic>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::registry
{
    /**
     * @brief Thread-safe plugin registry implementation.
     *
     * Maintains a catalog of all plugins with indexing by:
     * - Plugin name
     * - Capability
     * - Provided service
     * - Required service
     */
    class PluginRegistry : public IPluginRegistry
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Internal entry for registered plugins.
         */
        struct PluginEntry
        {
            std::unique_ptr<IPluginMetadata> metadata;
            std::shared_ptr<IPlugin> plugin;
            PluginState state{PluginState::Unloaded};
            std::chrono::system_clock::time_point registrationTime;
        };

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Default constructor.
         */
        PluginRegistry();

        /**
         * @brief Destructor.
         */
        ~PluginRegistry() noexcept override;

        // Delete copy operations
        PluginRegistry(const PluginRegistry&) = delete;
        auto operator=(const PluginRegistry&) -> PluginRegistry& = delete;

        // Allow move operations
        PluginRegistry(PluginRegistry&& other) noexcept;
        auto operator=(PluginRegistry&& other) noexcept -> PluginRegistry&;

        // ============================================
        // IPluginRegistry Implementation
        // ============================================

        auto registerPlugin(std::unique_ptr<IPluginMetadata> metadata,
                           std::shared_ptr<IPlugin> plugin = nullptr) -> void override;

        auto unregisterPlugin(std::string_view name) -> void override;

        auto updatePlugin(std::string_view name, std::shared_ptr<IPlugin> plugin) -> void override;

        [[nodiscard]] auto isRegistered(std::string_view name) const noexcept -> bool override;

        [[nodiscard]] auto getPlugin(std::string_view name) const
            -> std::shared_ptr<IPlugin> override;

        [[nodiscard]] auto getMetadata(std::string_view name) const
            -> const IPluginMetadata* override;

        [[nodiscard]] auto getAllMetadata() const
            -> std::vector<const IPluginMetadata*> override;

        [[nodiscard]] auto getPluginState(std::string_view name) const noexcept
            -> PluginState override;

        auto setPluginState(std::string_view name, PluginState state) -> void override;

        [[nodiscard]] auto getPluginsByState(PluginState state) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto findByCapability(std::string_view capability) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto findByService(std::string_view service) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto findByRequiredService(std::string_view service) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto getAllPluginNames() const -> std::vector<std::string> override;

        [[nodiscard]] auto getPluginCount() const noexcept -> std::size_t override;

        [[nodiscard]] auto getLoadedPluginCount() const noexcept -> std::size_t override;

        auto forEachPlugin(const PluginVisitor& visitor) const -> void override;

        [[nodiscard]] auto onRegistryChange(RegistryChangeCallback callback)
            -> std::uint64_t override;

        auto removeRegistryChangeCallback(std::uint64_t handle) -> void override;

        // ============================================
        // Additional Methods
        // ============================================

        /**
         * @brief Clear all registered plugins.
         */
        auto clear() -> void;

    private:
        // ============================================
        // Index Management
        // ============================================

        auto buildIndices(const std::string& pluginName, const IPluginMetadata& metadata) -> void;
        auto removeFromIndices(const std::string& pluginName) -> void;
        auto notifyChange(std::string_view pluginName, PluginState state) -> void;

        // ============================================
        // Member Variables
        // ============================================

        std::map<std::string, PluginEntry> plugins_;
        std::multimap<std::string, std::string> capabilityIndex_;
        std::multimap<std::string, std::string> providedServiceIndex_;
        std::multimap<std::string, std::string> requiredServiceIndex_;

        std::map<std::uint64_t, RegistryChangeCallback> changeCallbacks_;
        std::atomic<std::uint64_t> nextCallbackId_{1};

        mutable std::shared_mutex mutex_;
        mutable std::shared_mutex callbackMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::registry

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_REGISTRY_PLUGINREGISTRY_HPP
