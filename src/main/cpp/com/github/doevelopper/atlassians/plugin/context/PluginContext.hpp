/**
 * @file PluginContext.hpp
 * @brief Concrete plugin context implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CONTEXT_PLUGINCONTEXT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CONTEXT_PLUGINCONTEXT_HPP

#include <any>
#include <filesystem>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginContext.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginCommunicator.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::context
{
    /**
     * @brief Concrete plugin context implementation.
     *
     * Provides runtime context to plugins including:
     * - Service locator for dependencies
     * - Configuration access
     * - Event bus access
     * - Logging facilities
     * - Data storage
     */
    class PluginContext : public IPluginContext
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Construct a context for a specific plugin.
         * @param pluginName Plugin name
         * @param registry Plugin registry reference
         * @param eventBus Event bus reference
         */
        PluginContext(std::string_view pluginName,
                      IPluginRegistry& registry,
                      IPluginCommunicator& eventBus);

        /**
         * @brief Destructor.
         */
        ~PluginContext() noexcept override;

        // Delete copy operations
        PluginContext(const PluginContext&) = delete;
        auto operator=(const PluginContext&) -> PluginContext& = delete;

        // Allow move operations
        PluginContext(PluginContext&& other) noexcept;
        auto operator=(PluginContext&& other) noexcept -> PluginContext&;

        // ============================================
        // IPluginContext - Service Access
        // ============================================

        [[nodiscard]] auto getService(std::string_view serviceName)
            -> std::shared_ptr<void> override;

        auto registerService(std::string_view serviceName,
                             std::shared_ptr<void> service) -> void override;

        auto unregisterService(std::string_view serviceName) -> void override;

        [[nodiscard]] auto hasService(std::string_view serviceName) const noexcept
            -> bool override;

        // ============================================
        // IPluginContext - Configuration Access
        // ============================================

        [[nodiscard]] auto getConfigValue(std::string_view key) const
            -> std::optional<std::string> override;

        [[nodiscard]] auto getConfigValue(std::string_view key,
                                          std::string_view defaultValue) const
            -> std::string override;

        auto setConfigValue(std::string_view key, std::string_view value) -> void override;

        // ============================================
        // IPluginContext - Path Access
        // ============================================

        [[nodiscard]] auto getPluginDirectory() const noexcept
            -> std::filesystem::path override;

        [[nodiscard]] auto getDataDirectory() const noexcept
            -> std::filesystem::path override;

        [[nodiscard]] auto getConfigDirectory() const noexcept
            -> std::filesystem::path override;

        [[nodiscard]] auto getTempDirectory() const noexcept
            -> std::filesystem::path override;

        [[nodiscard]] auto getApplicationDirectory() const noexcept
            -> std::filesystem::path override;

        // ============================================
        // IPluginContext - Logging
        // ============================================

        auto log(LogLevel level, std::string_view message) -> void override;

        // ============================================
        // IPluginContext - Event Handling
        // ============================================

        auto publishEvent(const PluginEvent& event) -> void override;

        auto publishEvent(std::string_view eventType, EventPayload payload = {}) -> void override;

        [[nodiscard]] auto subscribeToEvent(std::string_view eventType,
                                            EventHandler handler) -> std::uint64_t override;

        auto unsubscribeFromEvent(std::uint64_t subscriptionId) -> void override;

        // ============================================
        // IPluginContext - Plugin Interaction
        // ============================================

        [[nodiscard]] auto getPluginName() const noexcept -> std::string_view override;

        [[nodiscard]] auto getPlugin(std::string_view pluginName) const
            -> std::weak_ptr<IPlugin> override;

        [[nodiscard]] auto getLoadedPlugins() const -> std::vector<std::string> override;

        // ============================================
        // IPluginContext - Host Information
        // ============================================

        [[nodiscard]] auto getHostName() const noexcept -> std::string_view override;

        [[nodiscard]] auto getHostVersion() const noexcept -> std::string_view override;

        [[nodiscard]] auto getPluginApiVersion() const noexcept -> std::string_view override;

        // ============================================
        // Additional Methods
        // ============================================

        /**
         * @brief Set the plugin directory.
         * @param path Plugin directory path
         */
        auto setPluginDirectory(const std::filesystem::path& path) -> void;

        /**
         * @brief Set the data directory.
         * @param path Data directory path
         */
        auto setDataDirectory(const std::filesystem::path& path) -> void;

        /**
         * @brief Set the config directory.
         * @param path Config directory path
         */
        auto setConfigDirectory(const std::filesystem::path& path) -> void;

        /**
         * @brief Set the temp directory.
         * @param path Temp directory path
         */
        auto setTempDirectory(const std::filesystem::path& path) -> void;

        /**
         * @brief Set the application directory.
         * @param path Application directory path
         */
        auto setApplicationDirectory(const std::filesystem::path& path) -> void;

        /**
         * @brief Load configuration from file.
         * @param path Configuration file path
         * @return True if loaded successfully
         */
        auto loadConfiguration(const std::filesystem::path& path) -> bool;

    private:
        // ============================================
        // Member Variables
        // ============================================

        std::string pluginName_;

        std::filesystem::path pluginDirectory_;
        std::filesystem::path dataDirectory_;
        std::filesystem::path configDirectory_;
        std::filesystem::path tempDirectory_;
        std::filesystem::path applicationDirectory_;

        IPluginRegistry& registry_;
        IPluginCommunicator& eventBus_;

        std::unordered_map<std::string, std::shared_ptr<void>> services_;
        std::map<std::string, std::string> configuration_;
        std::vector<std::uint64_t> subscriptions_;

        mutable std::shared_mutex servicesMutex_;
        mutable std::shared_mutex configMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::context

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CONTEXT_PLUGINCONTEXT_HPP
