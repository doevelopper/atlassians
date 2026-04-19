/**
 * @file PluginManager.hpp
 * @brief Main plugin management facade
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_MANAGER_PLUGINMANAGER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_MANAGER_PLUGINMANAGER_HPP

#include <filesystem>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginContext.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginLoader.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginCommunicator.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginValidator.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginDependencyResolver.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::manager
{
    using namespace com::github::doevelopper::atlassians::plugin;

    /**
     * @brief Plugin manager configuration.
     */
    struct PluginManagerConfig
    {
        std::vector<std::filesystem::path> pluginDirectories;
        bool autoDiscoverPlugins{true};
        bool enableHotReload{false};
        bool enableValidation{true};
        bool parallelLoading{true};
        std::chrono::seconds watchInterval{5};
        std::size_t maxConcurrentLoads{4};
    };

    /**
     * @brief Plugin load result.
     */
    struct LoadResult
    {
        bool success{false};
        std::string pluginName;
        std::string error;
        std::chrono::milliseconds loadTime{0};
    };

    /**
     * @brief Main facade for plugin management.
     *
     * Coordinates all plugin-related operations including:
     * - Plugin discovery and loading
     * - Lifecycle management
     * - Dependency resolution
     * - Inter-plugin communication
     * - Hot-reload support
     *
     * Implements the Facade pattern to provide a simple interface
     * to the complex plugin subsystem.
     */
    class PluginManager
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Lifecycle event callback.
         */
        using LifecycleCallback = std::function<void(const std::string&, PluginState)>;

        /**
         * @brief Plugin filter predicate.
         */
        using PluginFilter = std::function<bool(const IPluginMetadata&)>;

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Construct with default configuration.
         */
        PluginManager();

        /**
         * @brief Construct with custom configuration.
         * @param config Configuration settings
         */
        explicit PluginManager(const PluginManagerConfig& config);

        /**
         * @brief Destructor - shuts down all plugins.
         */
        ~PluginManager() noexcept;

        // Delete copy operations
        PluginManager(const PluginManager&) = delete;
        auto operator=(const PluginManager&) -> PluginManager& = delete;

        // Allow move operations
        PluginManager(PluginManager&& other) noexcept;
        auto operator=(PluginManager&& other) noexcept -> PluginManager&;

        // ============================================
        // Initialization and Shutdown
        // ============================================

        /**
         * @brief Initialize the plugin manager.
         * @return True if initialization succeeded
         */
        auto initialize() -> bool;

        /**
         * @brief Shut down the plugin manager and all plugins.
         */
        auto shutdown() -> void;

        /**
         * @brief Check if manager is initialized.
         * @return True if initialized
         */
        [[nodiscard]] auto isInitialized() const noexcept -> bool;

        // ============================================
        // Plugin Discovery and Loading
        // ============================================

        /**
         * @brief Discover plugins in configured directories.
         * @return Number of plugins discovered
         */
        auto discoverPlugins() -> std::size_t;

        /**
         * @brief Discover plugins in a specific directory.
         * @param directory Directory to scan
         * @param recursive Whether to scan recursively
         * @return Number of plugins discovered
         */
        auto discoverPlugins(const std::filesystem::path& directory,
                            bool recursive = true) -> std::size_t;

        /**
         * @brief Load a specific plugin by name.
         * @param name Plugin name
         * @return Load result
         */
        auto loadPlugin(std::string_view name) -> LoadResult;

        /**
         * @brief Load a plugin from a specific path.
         * @param path Path to plugin library
         * @return Load result
         */
        auto loadPlugin(const std::filesystem::path& path) -> LoadResult;

        /**
         * @brief Load all discovered plugins.
         * @return Vector of load results
         */
        auto loadAllPlugins() -> std::vector<LoadResult>;

        /**
         * @brief Load plugins matching a filter.
         * @param filter Filter predicate
         * @return Vector of load results
         */
        auto loadPlugins(const PluginFilter& filter) -> std::vector<LoadResult>;

        /**
         * @brief Unload a plugin by name.
         * @param name Plugin name
         * @return True if unloaded successfully
         */
        auto unloadPlugin(std::string_view name) -> bool;

        /**
         * @brief Unload all plugins.
         */
        auto unloadAllPlugins() -> void;

        /**
         * @brief Reload a plugin.
         * @param name Plugin name
         * @return Load result
         */
        auto reloadPlugin(std::string_view name) -> LoadResult;

        // ============================================
        // Lifecycle Management
        // ============================================

        /**
         * @brief Start a plugin.
         * @param name Plugin name
         * @return True if started successfully
         */
        auto startPlugin(std::string_view name) -> bool;

        /**
         * @brief Stop a plugin.
         * @param name Plugin name
         * @return True if stopped successfully
         */
        auto stopPlugin(std::string_view name) -> bool;

        /**
         * @brief Start all loaded plugins.
         * @return Number of plugins started
         */
        auto startAllPlugins() -> std::size_t;

        /**
         * @brief Stop all running plugins.
         * @return Number of plugins stopped
         */
        auto stopAllPlugins() -> std::size_t;

        // ============================================
        // Plugin Access
        // ============================================

        /**
         * @brief Get a plugin by name.
         * @param name Plugin name
         * @return Plugin pointer or nullptr
         */
        [[nodiscard]] auto getPlugin(std::string_view name) -> std::shared_ptr<IPlugin>;

        /**
         * @brief Get a plugin with specific interface.
         * @tparam T Plugin interface type
         * @param name Plugin name
         * @return Typed plugin pointer or nullptr
         */
        template<typename T>
        [[nodiscard]] auto getPlugin(std::string_view name) -> std::shared_ptr<T>
        {
            auto plugin = getPlugin(name);
            return std::dynamic_pointer_cast<T>(plugin);
        }

        /**
         * @brief Get metadata for a plugin.
         * @param name Plugin name
         * @return Metadata pointer or nullptr
         */
        [[nodiscard]] auto getMetadata(std::string_view name) const
            -> const IPluginMetadata*;

        /**
         * @brief Get plugins with a specific capability.
         * @param capability Capability to search for
         * @return Vector of plugin names
         */
        [[nodiscard]] auto getPluginsByCapability(std::string_view capability) const
            -> std::vector<std::string>;

        /**
         * @brief Get plugins providing a specific service.
         * @param service Service to search for
         * @return Vector of plugin names
         */
        [[nodiscard]] auto getPluginsByService(std::string_view service) const
            -> std::vector<std::string>;

        /**
         * @brief Get all plugin names.
         * @return Vector of plugin names
         */
        [[nodiscard]] auto getAllPluginNames() const -> std::vector<std::string>;

        /**
         * @brief Get plugin count.
         * @return Number of registered plugins
         */
        [[nodiscard]] auto getPluginCount() const noexcept -> std::size_t;

        /**
         * @brief Check if a plugin is loaded.
         * @param name Plugin name
         * @return True if loaded
         */
        [[nodiscard]] auto isPluginLoaded(std::string_view name) const noexcept -> bool;

        /**
         * @brief Get plugin state.
         * @param name Plugin name
         * @return Plugin state
         */
        [[nodiscard]] auto getPluginState(std::string_view name) const noexcept
            -> PluginState;

        // ============================================
        // Service Access
        // ============================================

        /**
         * @brief Get a service by type.
         * @tparam T Service type
         * @return Service pointer or nullptr
         */
        template<typename T>
        [[nodiscard]] auto getService() const -> std::shared_ptr<T>
        {
            auto it = services_.find(typeid(T).name());
            if (it != services_.end())
            {
                return std::any_cast<std::shared_ptr<T>>(it->second);
            }
            return nullptr;
        }

        /**
         * @brief Register a global service.
         * @tparam T Service type
         * @param service Service instance
         */
        template<typename T>
        auto registerService(std::shared_ptr<T> service) -> void
        {
            services_[typeid(T).name()] = std::move(service);
        }

        // ============================================
        // Event Bus Access
        // ============================================

        /**
         * @brief Get the event bus.
         * @return Event bus reference
         */
        [[nodiscard]] auto getEventBus() -> IPluginCommunicator&;

        /**
         * @brief Publish an event.
         * @param topic Event topic
         * @param event Event data
         */
        auto publishEvent(std::string_view topic, const PluginEvent& event) -> void;

        // ============================================
        // Lifecycle Callbacks
        // ============================================

        /**
         * @brief Register a lifecycle callback.
         * @param callback Callback function
         * @return Callback ID
         */
        [[nodiscard]] auto onLifecycleChange(LifecycleCallback callback) -> std::uint64_t;

        /**
         * @brief Remove a lifecycle callback.
         * @param callbackId Callback ID to remove
         */
        auto removeLifecycleCallback(std::uint64_t callbackId) -> void;

        // ============================================
        // Configuration
        // ============================================

        /**
         * @brief Get current configuration.
         * @return Configuration
         */
        [[nodiscard]] auto getConfig() const noexcept -> const PluginManagerConfig&;

        /**
         * @brief Add a plugin directory.
         * @param directory Directory path
         */
        auto addPluginDirectory(const std::filesystem::path& directory) -> void;

        /**
         * @brief Set custom loader.
         * @param loader Custom loader
         */
        auto setLoader(std::unique_ptr<IPluginLoader> loader) -> void;

        /**
         * @brief Set custom validator.
         * @param validator Custom validator
         */
        auto setValidator(std::unique_ptr<IPluginValidator> validator) -> void;

        // ============================================
        // Component Access
        // ============================================

        /**
         * @brief Get the registry.
         * @return Registry reference
         */
        [[nodiscard]] auto getRegistry() -> IPluginRegistry&;

        /**
         * @brief Get the dependency resolver.
         * @return Resolver reference
         */
        [[nodiscard]] auto getDependencyResolver() -> IPluginDependencyResolver&;

    private:
        // ============================================
        // Internal Methods
        // ============================================

        auto createContext(std::string_view pluginName) -> std::shared_ptr<IPluginContext>;
        auto initializePlugin(const std::string& name) -> bool;
        auto transitionState(const std::string& name, PluginState target) -> bool;
        auto notifyLifecycleChange(const std::string& name, PluginState state) -> void;

        // ============================================
        // Member Variables
        // ============================================

        PluginManagerConfig config_;
        bool initialized_{false};

        std::unique_ptr<IPluginLoader> loader_;
        std::unique_ptr<IPluginRegistry> registry_;
        std::unique_ptr<IPluginCommunicator> eventBus_;
        std::unique_ptr<IPluginValidator> validator_;
        std::unique_ptr<IPluginDependencyResolver> dependencyResolver_;

        // Plugin path mapping (name -> filesystem path)
        std::map<std::string, std::filesystem::path> pluginPaths_;

        // Service registry
        std::unordered_map<std::string, std::any> services_;

        // Plugin contexts
        std::map<std::string, std::shared_ptr<IPluginContext>> contexts_;

        // Lifecycle callbacks
        std::map<std::uint64_t, LifecycleCallback> lifecycleCallbacks_;
        std::atomic<std::uint64_t> nextCallbackId_{1};

        // Mutexes
        mutable std::shared_mutex mutex_;
        mutable std::shared_mutex serviceMutex_;
        mutable std::shared_mutex callbackMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::manager

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_MANAGER_PLUGINMANAGER_HPP
