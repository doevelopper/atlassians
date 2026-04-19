/**
 * @file PluginRegistry.cpp
 * @brief Concrete plugin registry implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/plugin/registry/PluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

namespace com::github::doevelopper::atlassians::plugin::registry
{
    log4cxx::LoggerPtr PluginRegistry::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.registry.PluginRegistry"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    PluginRegistry::PluginRegistry()
    {
        LOG4CXX_TRACE(logger, "PluginRegistry created");
    }

    PluginRegistry::~PluginRegistry() noexcept
    {
        LOG4CXX_TRACE(logger, "PluginRegistry destroyed with " << plugins_.size() << " plugins");
    }

    PluginRegistry::PluginRegistry(PluginRegistry&& other) noexcept
    {
        std::unique_lock lock(other.mutex_);
        plugins_ = std::move(other.plugins_);
        capabilityIndex_ = std::move(other.capabilityIndex_);
        providedServiceIndex_ = std::move(other.providedServiceIndex_);
        requiredServiceIndex_ = std::move(other.requiredServiceIndex_);
        changeCallbacks_ = std::move(other.changeCallbacks_);
        nextCallbackId_.store(other.nextCallbackId_.load());
    }

    auto PluginRegistry::operator=(PluginRegistry&& other) noexcept -> PluginRegistry&
    {
        if (this != &other)
        {
            std::scoped_lock lock(mutex_, other.mutex_);
            plugins_ = std::move(other.plugins_);
            capabilityIndex_ = std::move(other.capabilityIndex_);
            providedServiceIndex_ = std::move(other.providedServiceIndex_);
            requiredServiceIndex_ = std::move(other.requiredServiceIndex_);
            changeCallbacks_ = std::move(other.changeCallbacks_);
            nextCallbackId_.store(other.nextCallbackId_.load());
        }
        return *this;
    }

    // ============================================
    // IPluginRegistry Implementation
    // ============================================

    auto PluginRegistry::registerPlugin(std::unique_ptr<IPluginMetadata> metadata,
                                        std::shared_ptr<IPlugin> plugin) -> void
    {
        if (!metadata)
        {
            throw PluginException("Cannot register plugin with null metadata");
        }

        const std::string pluginName{metadata->getName()};
        LOG4CXX_DEBUG(logger, "Registering plugin: " << pluginName
                     << " version " << metadata->getVersion());

        std::unique_lock lock(mutex_);

        if (plugins_.find(pluginName) != plugins_.end())
        {
            throw PluginException("Plugin already registered: " + pluginName);
        }

        PluginEntry entry;
        entry.metadata = std::move(metadata);
        entry.plugin = std::move(plugin);
        entry.state = PluginState::Unloaded;
        entry.registrationTime = std::chrono::system_clock::now();

        buildIndices(pluginName, *entry.metadata);
        plugins_.emplace(pluginName, std::move(entry));

        lock.unlock();

        LOG4CXX_INFO(logger, "Plugin registered: " << pluginName);
        notifyChange(pluginName, PluginState::Unloaded);
    }

    auto PluginRegistry::unregisterPlugin(std::string_view name) -> void
    {
        const std::string pluginName(name);
        LOG4CXX_DEBUG(logger, "Unregistering plugin: " << pluginName);

        std::unique_lock lock(mutex_);

        auto it = plugins_.find(pluginName);
        if (it == plugins_.end())
        {
            throw PluginNotFoundException(pluginName);
        }

        if (it->second.state == PluginState::Started)
        {
            throw PluginStateException(pluginName, std::string(toString(PluginState::Started)),
                "Cannot unregister started plugin");
        }

        removeFromIndices(pluginName);
        plugins_.erase(it);

        lock.unlock();

        LOG4CXX_INFO(logger, "Plugin unregistered: " << pluginName);
        notifyChange(pluginName, PluginState::Unloaded);
    }

    auto PluginRegistry::updatePlugin(std::string_view name, std::shared_ptr<IPlugin> plugin) -> void
    {
        const std::string pluginName(name);
        LOG4CXX_DEBUG(logger, "Updating plugin instance: " << pluginName);

        std::unique_lock lock(mutex_);

        auto it = plugins_.find(pluginName);
        if (it == plugins_.end())
        {
            throw PluginNotFoundException(pluginName);
        }

        it->second.plugin = std::move(plugin);

        LOG4CXX_INFO(logger, "Plugin instance updated: " << pluginName);
    }

    auto PluginRegistry::isRegistered(std::string_view name) const noexcept -> bool
    {
        std::shared_lock lock(mutex_);
        return plugins_.find(std::string(name)) != plugins_.end();
    }

    auto PluginRegistry::getPlugin(std::string_view name) const -> std::shared_ptr<IPlugin>
    {
        std::shared_lock lock(mutex_);

        auto it = plugins_.find(std::string(name));
        if (it == plugins_.end())
        {
            return nullptr;
        }

        return it->second.plugin;
    }

    auto PluginRegistry::getMetadata(std::string_view name) const -> const IPluginMetadata*
    {
        std::shared_lock lock(mutex_);

        auto it = plugins_.find(std::string(name));
        if (it == plugins_.end())
        {
            return nullptr;
        }

        return it->second.metadata.get();
    }

    auto PluginRegistry::getAllMetadata() const -> std::vector<const IPluginMetadata*>
    {
        std::shared_lock lock(mutex_);

        std::vector<const IPluginMetadata*> result;
        result.reserve(plugins_.size());

        for (const auto& [name, entry] : plugins_)
        {
            result.push_back(entry.metadata.get());
        }

        return result;
    }

    auto PluginRegistry::getPluginState(std::string_view name) const noexcept -> PluginState
    {
        std::shared_lock lock(mutex_);

        auto it = plugins_.find(std::string(name));
        if (it == plugins_.end())
        {
            return PluginState::Unloaded;
        }

        return it->second.state;
    }

    auto PluginRegistry::setPluginState(std::string_view name, PluginState state) -> void
    {
        const std::string pluginName(name);
        LOG4CXX_DEBUG(logger, "Setting plugin state: " << pluginName
                     << " -> " << toString(state));

        PluginState oldState;

        {
            std::unique_lock lock(mutex_);

            auto it = plugins_.find(pluginName);
            if (it == plugins_.end())
            {
                throw PluginNotFoundException(pluginName);
            }

            oldState = it->second.state;
            it->second.state = state;
        }

        if (oldState != state)
        {
            LOG4CXX_INFO(logger, "Plugin state changed: " << pluginName
                        << " from " << toString(oldState) << " to " << toString(state));
            notifyChange(pluginName, state);
        }
    }

    auto PluginRegistry::getPluginsByState(PluginState state) const -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);

        std::vector<std::string> result;
        for (const auto& [name, entry] : plugins_)
        {
            if (entry.state == state)
            {
                result.push_back(name);
            }
        }

        return result;
    }

    auto PluginRegistry::findByCapability(std::string_view capability) const
        -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);

        std::vector<std::string> result;
        const std::string cap(capability);

        auto range = capabilityIndex_.equal_range(cap);
        for (auto it = range.first; it != range.second; ++it)
        {
            result.push_back(it->second);
        }

        return result;
    }

    auto PluginRegistry::findByService(std::string_view service) const -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);

        std::vector<std::string> result;
        const std::string svc(service);

        auto range = providedServiceIndex_.equal_range(svc);
        for (auto it = range.first; it != range.second; ++it)
        {
            result.push_back(it->second);
        }

        return result;
    }

    auto PluginRegistry::findByRequiredService(std::string_view service) const
        -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);

        std::vector<std::string> result;
        const std::string svc(service);

        auto range = requiredServiceIndex_.equal_range(svc);
        for (auto it = range.first; it != range.second; ++it)
        {
            result.push_back(it->second);
        }

        return result;
    }

    auto PluginRegistry::getAllPluginNames() const -> std::vector<std::string>
    {
        std::shared_lock lock(mutex_);

        std::vector<std::string> result;
        result.reserve(plugins_.size());

        for (const auto& [name, entry] : plugins_)
        {
            result.push_back(name);
        }

        return result;
    }

    auto PluginRegistry::getPluginCount() const noexcept -> std::size_t
    {
        std::shared_lock lock(mutex_);
        return plugins_.size();
    }

    auto PluginRegistry::getLoadedPluginCount() const noexcept -> std::size_t
    {
        std::shared_lock lock(mutex_);

        std::size_t count = 0;
        for (const auto& [name, entry] : plugins_)
        {
            if (entry.state == PluginState::Loaded ||
                entry.state == PluginState::Started ||
                entry.state == PluginState::Initialized)
            {
                ++count;
            }
        }

        return count;
    }

    auto PluginRegistry::forEachPlugin(const PluginVisitor& visitor) const -> void
    {
        std::shared_lock lock(mutex_);

        for (const auto& [name, entry] : plugins_)
        {
            visitor(name, *entry.metadata, entry.plugin);
        }
    }

    auto PluginRegistry::onRegistryChange(RegistryChangeCallback callback) -> std::uint64_t
    {
        std::unique_lock lock(callbackMutex_);

        std::uint64_t handle = nextCallbackId_++;
        changeCallbacks_.emplace(handle, std::move(callback));

        LOG4CXX_DEBUG(logger, "Registry change callback registered: " << handle);
        return handle;
    }

    auto PluginRegistry::removeRegistryChangeCallback(std::uint64_t handle) -> void
    {
        std::unique_lock lock(callbackMutex_);

        if (changeCallbacks_.erase(handle) > 0)
        {
            LOG4CXX_DEBUG(logger, "Registry change callback removed: " << handle);
        }
    }

    // ============================================
    // Additional Methods
    // ============================================

    auto PluginRegistry::clear() -> void
    {
        LOG4CXX_INFO(logger, "Clearing all plugins from registry");

        std::unique_lock lock(mutex_);

        plugins_.clear();
        capabilityIndex_.clear();
        providedServiceIndex_.clear();
        requiredServiceIndex_.clear();
    }

    // ============================================
    // Index Management
    // ============================================

    auto PluginRegistry::buildIndices(const std::string& pluginName,
                                      const IPluginMetadata& metadata) -> void
    {
        // Index by capabilities
        for (const auto& cap : metadata.getCapabilities())
        {
            capabilityIndex_.emplace(cap, pluginName);
        }

        // Index by provided services
        for (const auto& svc : metadata.getProvidedServices())
        {
            providedServiceIndex_.emplace(svc, pluginName);
        }

        // Index by required services
        for (const auto& svc : metadata.getRequiredServices())
        {
            requiredServiceIndex_.emplace(svc, pluginName);
        }

        LOG4CXX_TRACE(logger, "Built indices for plugin: " << pluginName);
    }

    auto PluginRegistry::removeFromIndices(const std::string& pluginName) -> void
    {
        // Remove from capability index
        for (auto it = capabilityIndex_.begin(); it != capabilityIndex_.end();)
        {
            if (it->second == pluginName)
            {
                it = capabilityIndex_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Remove from provided service index
        for (auto it = providedServiceIndex_.begin(); it != providedServiceIndex_.end();)
        {
            if (it->second == pluginName)
            {
                it = providedServiceIndex_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Remove from required service index
        for (auto it = requiredServiceIndex_.begin(); it != requiredServiceIndex_.end();)
        {
            if (it->second == pluginName)
            {
                it = requiredServiceIndex_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        LOG4CXX_TRACE(logger, "Removed indices for plugin: " << pluginName);
    }

    auto PluginRegistry::notifyChange(std::string_view pluginName, PluginState state) -> void
    {
        std::shared_lock lock(callbackMutex_);

        for (const auto& [handle, callback] : changeCallbacks_)
        {
            try
            {
                callback(pluginName, state);
            }
            catch (const std::exception& e)
            {
                LOG4CXX_WARN(logger, "Registry change callback threw exception: " << e.what());
            }
        }
    }

} // namespace com::github::doevelopper::atlassians::plugin::registry
