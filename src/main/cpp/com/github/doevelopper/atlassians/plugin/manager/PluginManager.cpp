/**
 * @file PluginManager.cpp
 * @brief Main plugin management facade implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <algorithm>
#include <future>

#include <com/github/doevelopper/atlassians/plugin/manager/PluginManager.hpp>
#include <com/github/doevelopper/atlassians/plugin/registry/PluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/plugin/loader/DynamicLibraryLoader.hpp>
#include <com/github/doevelopper/atlassians/plugin/communication/EventBus.hpp>
#include <com/github/doevelopper/atlassians/plugin/resolver/DependencyResolver.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

namespace com::github::doevelopper::atlassians::plugin::manager
{
    log4cxx::LoggerPtr PluginManager::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.manager.PluginManager"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    PluginManager::PluginManager()
        : PluginManager(PluginManagerConfig{})
    {
    }

    PluginManager::PluginManager(const PluginManagerConfig& config)
        : config_(config)
    {
        LOG4CXX_TRACE(logger, "PluginManager created");
    }

    PluginManager::~PluginManager() noexcept
    {
        if (initialized_)
        {
            shutdown();
        }
        LOG4CXX_TRACE(logger, "PluginManager destroyed");
    }

    PluginManager::PluginManager(PluginManager&& other) noexcept
        : config_(std::move(other.config_))
        , initialized_(other.initialized_)
        , loader_(std::move(other.loader_))
        , registry_(std::move(other.registry_))
        , eventBus_(std::move(other.eventBus_))
        , validator_(std::move(other.validator_))
        , dependencyResolver_(std::move(other.dependencyResolver_))
        , services_(std::move(other.services_))
        , contexts_(std::move(other.contexts_))
        , lifecycleCallbacks_(std::move(other.lifecycleCallbacks_))
    {
        other.initialized_ = false;
        nextCallbackId_.store(other.nextCallbackId_.load());
    }

    auto PluginManager::operator=(PluginManager&& other) noexcept -> PluginManager&
    {
        if (this != &other)
        {
            if (initialized_)
            {
                shutdown();
            }

            std::scoped_lock lock(mutex_, other.mutex_);
            config_ = std::move(other.config_);
            initialized_ = other.initialized_;
            other.initialized_ = false;
            loader_ = std::move(other.loader_);
            registry_ = std::move(other.registry_);
            eventBus_ = std::move(other.eventBus_);
            validator_ = std::move(other.validator_);
            dependencyResolver_ = std::move(other.dependencyResolver_);
            services_ = std::move(other.services_);
            contexts_ = std::move(other.contexts_);
            lifecycleCallbacks_ = std::move(other.lifecycleCallbacks_);
            nextCallbackId_.store(other.nextCallbackId_.load());
        }
        return *this;
    }

    // ============================================
    // Initialization and Shutdown
    // ============================================

    auto PluginManager::initialize() -> bool
    {
        std::unique_lock lock(mutex_);

        if (initialized_)
        {
            LOG4CXX_WARN(logger, "PluginManager already initialized");
            return true;
        }

        LOG4CXX_INFO(logger, "Initializing PluginManager");

        try
        {
            // Create default components if not set
            if (!registry_)
            {
                registry_ = std::make_unique<registry::PluginRegistry>();
            }

            if (!loader_)
            {
                loader_ = std::make_unique<loader::DynamicLibraryLoader>();
            }

            if (!eventBus_)
            {
                eventBus_ = std::make_unique<communication::EventBus>();
            }

            if (!dependencyResolver_)
            {
                dependencyResolver_ = std::make_unique<resolver::DependencyResolver>();
            }

            // Auto-discover plugins if configured
            if (config_.autoDiscoverPlugins)
            {
                lock.unlock();
                discoverPlugins();
                lock.lock();
            }

            initialized_ = true;
            LOG4CXX_INFO(logger, "PluginManager initialized successfully");
            return true;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to initialize PluginManager: " << e.what());
            return false;
        }
    }

    auto PluginManager::shutdown() -> void
    {
        std::unique_lock lock(mutex_);

        if (!initialized_)
        {
            return;
        }

        LOG4CXX_INFO(logger, "Shutting down PluginManager");

        lock.unlock();

        // Stop all running plugins
        stopAllPlugins();

        // Unload all plugins
        unloadAllPlugins();

        lock.lock();

        // Clear components
        contexts_.clear();
        services_.clear();

        initialized_ = false;
        LOG4CXX_INFO(logger, "PluginManager shut down");
    }

    auto PluginManager::isInitialized() const noexcept -> bool
    {
        return initialized_;
    }

    // ============================================
    // Plugin Discovery and Loading
    // ============================================

    auto PluginManager::discoverPlugins() -> std::size_t
    {
        LOG4CXX_DEBUG(logger, "Discovering plugins in configured directories");

        std::size_t total = 0;

        for (const auto& dir : config_.pluginDirectories)
        {
            total += discoverPlugins(dir, true);
        }

        LOG4CXX_INFO(logger, "Discovered " << total << " plugins in total");
        return total;
    }

    auto PluginManager::discoverPlugins(const std::filesystem::path& directory,
                                         bool recursive) -> std::size_t
    {
        LOG4CXX_DEBUG(logger, "Discovering plugins in: " << directory);

        if (!loader_)
        {
            LOG4CXX_ERROR(logger, "No loader configured");
            return 0;
        }

        try
        {
            // scanDirectory returns paths, not metadata
            auto discoveredPaths = loader_->scanDirectory(directory, recursive);
            std::size_t count = 0;

            for (const auto& pluginPath : discoveredPaths)
            {
                try
                {
                    // Read metadata from the plugin
                    auto metadata = loader_->readMetadata(pluginPath);
                    if (metadata)
                    {
                        std::string name{metadata->getName()};

                        if (!registry_->isRegistered(name))
                        {
                            // Store the path association (we need it for loading)
                            pluginPaths_[name] = pluginPath;
                            registry_->registerPlugin(std::move(metadata));
                            LOG4CXX_DEBUG(logger, "Registered discovered plugin: " << name);
                            ++count;
                        }
                    }
                }
                catch (const std::exception& e)
                {
                    LOG4CXX_WARN(logger, "Failed to read metadata from " << pluginPath << ": " << e.what());
                }
            }

            return count;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to discover plugins: " << e.what());
            return 0;
        }
    }

    auto PluginManager::loadPlugin(std::string_view name) -> LoadResult
    {
        LoadResult result;
        result.pluginName = std::string(name);

        LOG4CXX_DEBUG(logger, "Loading plugin: " << name);

        auto startTime = std::chrono::steady_clock::now();

        try
        {
            // Check if already loaded
            PluginState state = registry_->getPluginState(name);
            if (state == PluginState::Loaded || state == PluginState::Started)
            {
                result.success = true;
                return result;
            }

            // Get metadata
            const IPluginMetadata* metadata = registry_->getMetadata(name);
            if (!metadata)
            {
                result.error = "Plugin not found: " + result.pluginName;
                return result;
            }

            // Get the plugin path
            auto pathIt = pluginPaths_.find(result.pluginName);
            if (pathIt == pluginPaths_.end())
            {
                result.error = "Plugin path not found: " + result.pluginName;
                return result;
            }
            const std::filesystem::path& pluginPath = pathIt->second;

            // Collect all metadata for dependency resolution
            std::vector<const IPluginMetadata*> allMetadata = registry_->getAllMetadata();

            // Resolve dependencies
            auto depResult = dependencyResolver_->resolveFor(*metadata, allMetadata);
            if (!depResult.success)
            {
                result.error = "Dependency resolution failed";
                for (const auto& err : depResult.missingDependencies)
                {
                    result.error += ": missing " + err;
                }
                return result;
            }

            // Load dependencies first (they come before the plugin in loadOrder)
            for (const auto& dep : depResult.loadOrder)
            {
                if (dep != result.pluginName && !isPluginLoaded(dep))
                {
                    auto depLoadResult = loadPlugin(std::string_view{dep});
                    if (!depLoadResult.success)
                    {
                        result.error = "Failed to load dependency: " + dep;
                        return result;
                    }
                }
            }

            // Load the plugin using its path
            auto plugin = loader_->loadPlugin(pluginPath);
            if (!plugin)
            {
                result.error = "Failed to load plugin library";
                return result;
            }

            // Update registry (convert unique_ptr to shared_ptr)
            std::shared_ptr<IPlugin> sharedPlugin = std::move(plugin);
            registry_->updatePlugin(name, sharedPlugin);
            registry_->setPluginState(name, PluginState::Loaded);

            // Create context and initialize
            auto context = createContext(name);
            contexts_[result.pluginName] = context;

            if (!initializePlugin(result.pluginName))
            {
                result.error = "Failed to initialize plugin";
                registry_->setPluginState(name, PluginState::Failed);
                return result;
            }

            auto endTime = std::chrono::steady_clock::now();
            result.loadTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                endTime - startTime);
            result.success = true;

            LOG4CXX_INFO(logger, "Plugin loaded: " << name
                        << " in " << result.loadTime.count() << "ms");

            notifyLifecycleChange(result.pluginName, PluginState::Initialized);
        }
        catch (const std::exception& e)
        {
            result.error = e.what();
            LOG4CXX_ERROR(logger, "Failed to load plugin " << name << ": " << e.what());
        }

        return result;
    }

    auto PluginManager::loadPlugin(const std::filesystem::path& path) -> LoadResult
    {
        LoadResult result;

        LOG4CXX_DEBUG(logger, "Loading plugin from path: " << path);

        try
        {
            // Read metadata from the plugin
            auto metadata = loader_->readMetadata(path);
            if (!metadata)
            {
                result.error = "No valid plugin found at: " + path.string();
                return result;
            }

            std::string name{metadata->getName()};

            if (!registry_->isRegistered(name))
            {
                pluginPaths_[name] = path;
                registry_->registerPlugin(std::move(metadata));
            }

            return loadPlugin(std::string_view{name});
        }
        catch (const std::exception& e)
        {
            result.error = e.what();
            LOG4CXX_ERROR(logger, "Failed to load plugin from path: " << e.what());
        }

        return result;
    }

    auto PluginManager::loadAllPlugins() -> std::vector<LoadResult>
    {
        LOG4CXX_DEBUG(logger, "Loading all plugins");

        std::vector<LoadResult> results;

        // Collect all metadata
        std::vector<const IPluginMetadata*> allMetadata = registry_->getAllMetadata();

        // Get load order from dependency resolver
        auto resolutionResult = dependencyResolver_->resolve(allMetadata);
        auto loadOrder = resolutionResult.loadOrder;

        if (config_.parallelLoading)
        {
            // Parallel loading with respect to dependencies
            // Group plugins by depth in dependency graph
            std::map<std::string, int> depths;
            for (const auto& name : loadOrder)
            {
                auto depChain = dependencyResolver_->getDependencyChain(name, allMetadata);
                depths[name] = static_cast<int>(depChain.size()) - 1;
            }

            // Group by depth
            std::map<int, std::vector<std::string>> groups;
            for (const auto& [name, depth] : depths)
            {
                groups[depth].push_back(name);
            }

            // Load each group in parallel
            for (const auto& [depth, group] : groups)
            {
                std::vector<std::future<LoadResult>> futures;

                for (const auto& name : group)
                {
                    std::string nameCopy = name;
                    futures.push_back(std::async(std::launch::async, [this, nameCopy]() {
                        return loadPlugin(std::string_view{nameCopy});
                    }));
                }

                for (auto& future : futures)
                {
                    results.push_back(future.get());
                }
            }
        }
        else
        {
            // Sequential loading
            for (const auto& name : loadOrder)
            {
                results.push_back(loadPlugin(std::string_view{name}));
            }
        }

        LOG4CXX_INFO(logger, "Loaded " << results.size() << " plugins");
        return results;
    }

    auto PluginManager::loadPlugins(const PluginFilter& filter) -> std::vector<LoadResult>
    {
        std::vector<LoadResult> results;
        auto allMetadata = registry_->getAllMetadata();

        std::vector<const IPluginMetadata*> toLoadMetadata;
        for (const auto* metadata : allMetadata)
        {
            if (filter(*metadata))
            {
                toLoadMetadata.push_back(metadata);
            }
        }

        auto resolutionResult = dependencyResolver_->resolve(toLoadMetadata);
        auto loadOrder = resolutionResult.loadOrder;

        for (const auto& name : loadOrder)
        {
            results.push_back(loadPlugin(std::string_view{name}));
        }

        return results;
    }

    auto PluginManager::unloadPlugin(std::string_view name) -> bool
    {
        LOG4CXX_DEBUG(logger, "Unloading plugin: " << name);

        try
        {
            // Check dependencies - get dependents
            auto allMetadata = registry_->getAllMetadata();
            auto dependents = dependencyResolver_->getDependents(name, allMetadata);

            // Check if any dependent is still loaded
            for (const auto& dependent : dependents)
            {
                if (isPluginLoaded(dependent))
                {
                    LOG4CXX_WARN(logger, "Cannot unload plugin with active dependents: " << name);
                    return false;
                }
            }

            // Stop if running
            PluginState state = registry_->getPluginState(name);
            if (state == PluginState::Started)
            {
                if (!stopPlugin(name))
                {
                    return false;
                }
            }

            // Get plugin and shutdown
            auto plugin = registry_->getPlugin(name);
            if (plugin)
            {
                plugin->shutdown();
            }

            // Unload library
            loader_->unloadPlugin(std::string(name));

            // Update registry
            registry_->setPluginState(name, PluginState::Unloaded);
            contexts_.erase(std::string(name));

            LOG4CXX_INFO(logger, "Plugin unloaded: " << name);
            notifyLifecycleChange(std::string(name), PluginState::Unloaded);

            return true;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to unload plugin " << name << ": " << e.what());
            return false;
        }
    }

    auto PluginManager::unloadAllPlugins() -> void
    {
        LOG4CXX_DEBUG(logger, "Unloading all plugins");

        auto allMetadata = registry_->getAllMetadata();
        auto resolutionResult = dependencyResolver_->resolve(allMetadata);

        // Reverse the load order to get unload order
        auto unloadOrder = resolutionResult.loadOrder;
        std::reverse(unloadOrder.begin(), unloadOrder.end());

        for (const auto& name : unloadOrder)
        {
            unloadPlugin(name);
        }
    }

    auto PluginManager::reloadPlugin(std::string_view name) -> LoadResult
    {
        LOG4CXX_DEBUG(logger, "Reloading plugin: " << name);

        LoadResult result;
        result.pluginName = std::string(name);

        try
        {
            // Get current state to restore
            PluginState previousState = registry_->getPluginState(name);
            bool wasRunning = (previousState == PluginState::Started);

            // Unload
            if (!unloadPlugin(name))
            {
                result.error = "Failed to unload plugin for reload";
                return result;
            }

            // Load
            result = loadPlugin(name);

            // Restart if was running
            if (result.success && wasRunning)
            {
                startPlugin(name);
            }
        }
        catch (const std::exception& e)
        {
            result.error = e.what();
            LOG4CXX_ERROR(logger, "Failed to reload plugin " << name << ": " << e.what());
        }

        return result;
    }

    // ============================================
    // Lifecycle Management
    // ============================================

    auto PluginManager::startPlugin(std::string_view name) -> bool
    {
        LOG4CXX_DEBUG(logger, "Starting plugin: " << name);

        try
        {
            auto plugin = registry_->getPlugin(name);
            if (!plugin)
            {
                LOG4CXX_ERROR(logger, "Plugin not loaded: " << name);
                return false;
            }

            PluginState state = registry_->getPluginState(name);
            if (state == PluginState::Started)
            {
                return true;
            }

            if (state != PluginState::Initialized && state != PluginState::Stopped)
            {
                LOG4CXX_ERROR(logger, "Cannot start plugin in state: " << toString(state));
                return false;
            }

            plugin->start();

            registry_->setPluginState(name, PluginState::Started);
            LOG4CXX_INFO(logger, "Plugin started: " << name);

            notifyLifecycleChange(std::string(name), PluginState::Started);
            return true;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to start plugin " << name << ": " << e.what());
            return false;
        }
    }

    auto PluginManager::stopPlugin(std::string_view name) -> bool
    {
        LOG4CXX_DEBUG(logger, "Stopping plugin: " << name);

        try
        {
            auto plugin = registry_->getPlugin(name);
            if (!plugin)
            {
                return true;
            }

            PluginState state = registry_->getPluginState(name);
            if (state != PluginState::Started)
            {
                return true;
            }

            plugin->stop();

            registry_->setPluginState(name, PluginState::Stopped);
            LOG4CXX_INFO(logger, "Plugin stopped: " << name);

            notifyLifecycleChange(std::string(name), PluginState::Stopped);
            return true;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to stop plugin " << name << ": " << e.what());
            return false;
        }
    }

    auto PluginManager::startAllPlugins() -> std::size_t
    {
        LOG4CXX_DEBUG(logger, "Starting all plugins");

        std::size_t count = 0;
        auto loadedPlugins = registry_->getPluginsByState(PluginState::Initialized);
        auto stoppedPlugins = registry_->getPluginsByState(PluginState::Stopped);

        loadedPlugins.insert(loadedPlugins.end(),
                            stoppedPlugins.begin(), stoppedPlugins.end());

        // Build metadata list for resolution
        std::vector<const IPluginMetadata*> toStartMetadata;
        for (const auto& name : loadedPlugins)
        {
            const auto* meta = registry_->getMetadata(name);
            if (meta)
            {
                toStartMetadata.push_back(meta);
            }
        }

        auto resolutionResult = dependencyResolver_->resolve(toStartMetadata);
        auto startOrder = resolutionResult.loadOrder;

        for (const auto& name : startOrder)
        {
            if (startPlugin(name))
            {
                ++count;
            }
        }

        LOG4CXX_INFO(logger, "Started " << count << " plugins");
        return count;
    }

    auto PluginManager::stopAllPlugins() -> std::size_t
    {
        LOG4CXX_DEBUG(logger, "Stopping all plugins");

        std::size_t count = 0;
        auto runningPlugins = registry_->getPluginsByState(PluginState::Started);

        // Build metadata list for resolution
        std::vector<const IPluginMetadata*> toStopMetadata;
        for (const auto& name : runningPlugins)
        {
            const auto* meta = registry_->getMetadata(name);
            if (meta)
            {
                toStopMetadata.push_back(meta);
            }
        }

        auto resolutionResult = dependencyResolver_->resolve(toStopMetadata);
        auto stopOrder = resolutionResult.loadOrder;

        // Reverse to stop in correct order (dependents first)
        std::reverse(stopOrder.begin(), stopOrder.end());

        for (const auto& name : stopOrder)
        {
            if (stopPlugin(name))
            {
                ++count;
            }
        }

        LOG4CXX_INFO(logger, "Stopped " << count << " plugins");
        return count;
    }

    // ============================================
    // Plugin Access
    // ============================================

    auto PluginManager::getPlugin(std::string_view name) -> std::shared_ptr<IPlugin>
    {
        std::shared_lock lock(mutex_);
        return registry_->getPlugin(name);
    }

    auto PluginManager::getMetadata(std::string_view name) const -> const IPluginMetadata*
    {
        std::shared_lock lock(mutex_);
        return registry_->getMetadata(name);
    }

    auto PluginManager::getPluginsByCapability(std::string_view capability) const
        -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);
        return registry_->findByCapability(capability);
    }

    auto PluginManager::getPluginsByService(std::string_view service) const
        -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);
        return registry_->findByService(service);
    }

    auto PluginManager::getAllPluginNames() const -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);
        return registry_->getAllPluginNames();
    }

    auto PluginManager::getPluginCount() const noexcept -> std::size_t
    {
        std::shared_lock lock(mutex_);
        return registry_->getPluginCount();
    }

    auto PluginManager::isPluginLoaded(std::string_view name) const noexcept -> bool
    {
        PluginState state = registry_->getPluginState(name);
        return state == PluginState::Loaded ||
               state == PluginState::Initialized ||
               state == PluginState::Started;
    }

    auto PluginManager::getPluginState(std::string_view name) const noexcept -> PluginState
    {
        return registry_->getPluginState(name);
    }

    // ============================================
    // Event Bus Access
    // ============================================

    auto PluginManager::getEventBus() -> IPluginCommunicator&
    {
        return *eventBus_;
    }

    auto PluginManager::publishEvent(std::string_view topic, const PluginEvent& event) -> void
    {
        eventBus_->publish(event);
    }

    // ============================================
    // Lifecycle Callbacks
    // ============================================

    auto PluginManager::onLifecycleChange(LifecycleCallback callback) -> std::uint64_t
    {
        std::unique_lock lock(callbackMutex_);

        std::uint64_t id = nextCallbackId_++;
        lifecycleCallbacks_.emplace(id, std::move(callback));

        return id;
    }

    auto PluginManager::removeLifecycleCallback(std::uint64_t callbackId) -> void
    {
        std::unique_lock lock(callbackMutex_);
        lifecycleCallbacks_.erase(callbackId);
    }

    // ============================================
    // Configuration
    // ============================================

    auto PluginManager::getConfig() const noexcept -> const PluginManagerConfig&
    {
        return config_;
    }

    auto PluginManager::addPluginDirectory(const std::filesystem::path& directory) -> void
    {
        config_.pluginDirectories.push_back(directory);

        if (initialized_ && config_.autoDiscoverPlugins)
        {
            discoverPlugins(directory, true);
        }
    }

    auto PluginManager::setLoader(std::unique_ptr<IPluginLoader> loader) -> void
    {
        loader_ = std::move(loader);
    }

    auto PluginManager::setValidator(std::unique_ptr<IPluginValidator> validator) -> void
    {
        validator_ = std::move(validator);
    }

    // ============================================
    // Component Access
    // ============================================

    auto PluginManager::getRegistry() -> IPluginRegistry&
    {
        return *registry_;
    }

    auto PluginManager::getDependencyResolver() -> IPluginDependencyResolver&
    {
        return *dependencyResolver_;
    }

    // ============================================
    // Internal Methods
    // ============================================

    auto PluginManager::createContext(std::string_view pluginName)
        -> std::shared_ptr<IPluginContext>
    {
        // Simple context implementation would go here
        // For now, return nullptr as we haven't implemented PluginContext yet
        return nullptr;
    }

    auto PluginManager::initializePlugin(const std::string& name) -> bool
    {
        auto plugin = registry_->getPlugin(name);
        if (!plugin)
        {
            return false;
        }

        auto contextIt = contexts_.find(name);
        if (contextIt == contexts_.end() || !contextIt->second)
        {
            // If no context, we can still initialize with a null context
            // But the current interface requires a reference, so skip if no context
            LOG4CXX_WARN(logger, "No context available for plugin: " << name << ", skipping initialization");
            registry_->setPluginState(name, PluginState::Initialized);
            return true;
        }

        try
        {
            plugin->initialize(*contextIt->second);
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Plugin initialization failed: " << name << ": " << e.what());
            return false;
        }

        registry_->setPluginState(name, PluginState::Initialized);
        return true;
    }

    auto PluginManager::transitionState(const std::string& name, PluginState target) -> bool
    {
        PluginState current = registry_->getPluginState(name);

        // Validate transition
        switch (target)
        {
            case PluginState::Loaded:
                return current == PluginState::Unloaded;

            case PluginState::Initialized:
                return current == PluginState::Loaded;

            case PluginState::Started:
                return current == PluginState::Initialized ||
                       current == PluginState::Stopped;

            case PluginState::Stopped:
                return current == PluginState::Started;

            case PluginState::Unloaded:
                return current != PluginState::Started;

            default:
                return false;
        }
    }

    auto PluginManager::notifyLifecycleChange(const std::string& name, PluginState state) -> void
    {
        std::shared_lock lock(callbackMutex_);

        for (const auto& [id, callback] : lifecycleCallbacks_)
        {
            try
            {
                callback(name, state);
            }
            catch (const std::exception& e)
            {
                LOG4CXX_WARN(logger, "Lifecycle callback threw exception: " << e.what());
            }
        }

        // Also publish as event
        PluginEvent event;
        event.sourcePlugin = "PluginManager";
        event.eventType = "lifecycle_change";
        event.payload = name + ":" + std::string(toString(state));

        eventBus_->publish(event);
    }

} // namespace com::github::doevelopper::atlassians::plugin::manager
