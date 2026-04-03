/**
 * @file PluginContext.cpp
 * @brief Concrete plugin context implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <fstream>
#include <sstream>

#include <com/github/doevelopper/atlassians/plugin/context/PluginContext.hpp>

namespace com::github::doevelopper::atlassians::plugin::context
{
    log4cxx::LoggerPtr PluginContext::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.context.PluginContext"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    PluginContext::PluginContext(std::string_view pluginName,
                                  IPluginRegistry& registry,
                                  IPluginCommunicator& eventBus)
        : pluginName_(pluginName)
        , registry_(registry)
        , eventBus_(eventBus)
    {
        LOG4CXX_TRACE(logger, "PluginContext created for: " << pluginName);
    }

    PluginContext::~PluginContext() noexcept
    {
        // Unsubscribe from all events
        for (auto id : subscriptions_)
        {
            try
            {
                eventBus_.unsubscribe(id);
            }
            catch (...)
            {
                // Ignore errors during cleanup
            }
        }
        LOG4CXX_TRACE(logger, "PluginContext destroyed for: " << pluginName_);
    }

    PluginContext::PluginContext(PluginContext&& other) noexcept
        : pluginName_(std::move(other.pluginName_))
        , pluginDirectory_(std::move(other.pluginDirectory_))
        , dataDirectory_(std::move(other.dataDirectory_))
        , configDirectory_(std::move(other.configDirectory_))
        , tempDirectory_(std::move(other.tempDirectory_))
        , applicationDirectory_(std::move(other.applicationDirectory_))
        , registry_(other.registry_)
        , eventBus_(other.eventBus_)
        , services_(std::move(other.services_))
        , configuration_(std::move(other.configuration_))
        , subscriptions_(std::move(other.subscriptions_))
    {
    }

    auto PluginContext::operator=(PluginContext&& other) noexcept -> PluginContext&
    {
        if (this != &other)
        {
            pluginName_ = std::move(other.pluginName_);
            pluginDirectory_ = std::move(other.pluginDirectory_);
            dataDirectory_ = std::move(other.dataDirectory_);
            configDirectory_ = std::move(other.configDirectory_);
            tempDirectory_ = std::move(other.tempDirectory_);
            applicationDirectory_ = std::move(other.applicationDirectory_);
            // registry_ and eventBus_ are references, cannot reassign
            services_ = std::move(other.services_);
            configuration_ = std::move(other.configuration_);
            subscriptions_ = std::move(other.subscriptions_);
        }
        return *this;
    }

    // ============================================
    // Service Access
    // ============================================

    auto PluginContext::getService(std::string_view serviceName)
        -> std::shared_ptr<void>
    {
        std::shared_lock lock(servicesMutex_);
        auto it = services_.find(std::string(serviceName));
        return it != services_.end() ? it->second : nullptr;
    }

    auto PluginContext::registerService(std::string_view serviceName,
                                          std::shared_ptr<void> service) -> void
    {
        std::unique_lock lock(servicesMutex_);
        services_[std::string(serviceName)] = std::move(service);
        LOG4CXX_DEBUG(logger, "Service registered: " << serviceName << " by " << pluginName_);
    }

    auto PluginContext::unregisterService(std::string_view serviceName) -> void
    {
        std::unique_lock lock(servicesMutex_);
        services_.erase(std::string(serviceName));
        LOG4CXX_DEBUG(logger, "Service unregistered: " << serviceName);
    }

    auto PluginContext::hasService(std::string_view serviceName) const noexcept -> bool
    {
        std::shared_lock lock(servicesMutex_);
        return services_.count(std::string(serviceName)) > 0;
    }

    // ============================================
    // Configuration Access
    // ============================================

    auto PluginContext::getConfigValue(std::string_view key) const
        -> std::optional<std::string>
    {
        std::shared_lock lock(configMutex_);
        auto it = configuration_.find(std::string(key));
        if (it != configuration_.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    auto PluginContext::getConfigValue(std::string_view key,
                                        std::string_view defaultValue) const
        -> std::string
    {
        auto value = getConfigValue(key);
        return value.value_or(std::string(defaultValue));
    }

    auto PluginContext::setConfigValue(std::string_view key, std::string_view value) -> void
    {
        std::unique_lock lock(configMutex_);
        configuration_[std::string(key)] = std::string(value);
    }

    // ============================================
    // Path Access
    // ============================================

    auto PluginContext::getPluginDirectory() const noexcept -> std::filesystem::path
    {
        return pluginDirectory_;
    }

    auto PluginContext::getDataDirectory() const noexcept -> std::filesystem::path
    {
        return dataDirectory_;
    }

    auto PluginContext::getConfigDirectory() const noexcept -> std::filesystem::path
    {
        return configDirectory_;
    }

    auto PluginContext::getTempDirectory() const noexcept -> std::filesystem::path
    {
        return tempDirectory_;
    }

    auto PluginContext::getApplicationDirectory() const noexcept -> std::filesystem::path
    {
        return applicationDirectory_;
    }

    // ============================================
    // Logging
    // ============================================

    auto PluginContext::log(LogLevel level, std::string_view message) -> void
    {
        switch (level)
        {
            case LogLevel::Trace:
                LOG4CXX_TRACE(logger, "[" << pluginName_ << "] " << message);
                break;
            case LogLevel::Debug:
                LOG4CXX_DEBUG(logger, "[" << pluginName_ << "] " << message);
                break;
            case LogLevel::Info:
                LOG4CXX_INFO(logger, "[" << pluginName_ << "] " << message);
                break;
            case LogLevel::Warning:
                LOG4CXX_WARN(logger, "[" << pluginName_ << "] " << message);
                break;
            case LogLevel::Error:
                LOG4CXX_ERROR(logger, "[" << pluginName_ << "] " << message);
                break;
            case LogLevel::Fatal:
                LOG4CXX_FATAL(logger, "[" << pluginName_ << "] " << message);
                break;
            default:
                LOG4CXX_INFO(logger, "[" << pluginName_ << "] " << message);
                break;
        }
    }

    // ============================================
    // Event Handling
    // ============================================

    auto PluginContext::publishEvent(const PluginEvent& event) -> void
    {
        eventBus_.publish(event);
    }

    auto PluginContext::publishEvent(std::string_view eventType, EventPayload payload) -> void
    {
        eventBus_.publish(eventType, std::move(payload), pluginName_);
    }

    auto PluginContext::subscribeToEvent(std::string_view eventType,
                                          EventHandler handler) -> std::uint64_t
    {
        auto id = eventBus_.subscribe(eventType, std::move(handler));
        subscriptions_.push_back(id);
        return id;
    }

    auto PluginContext::unsubscribeFromEvent(std::uint64_t subscriptionId) -> void
    {
        eventBus_.unsubscribe(subscriptionId);
        subscriptions_.erase(
            std::remove(subscriptions_.begin(), subscriptions_.end(), subscriptionId),
            subscriptions_.end()
        );
    }

    // ============================================
    // Plugin Interaction
    // ============================================

    auto PluginContext::getPluginName() const noexcept -> std::string_view
    {
        return pluginName_;
    }

    auto PluginContext::getPlugin(std::string_view pluginName) const
        -> std::weak_ptr<IPlugin>
    {
        return registry_.getPlugin(pluginName);
    }

    auto PluginContext::getLoadedPlugins() const -> std::vector<std::string>
    {
        return registry_.getPluginsByState(PluginState::Started);
    }

    // ============================================
    // Host Information
    // ============================================

    auto PluginContext::getHostName() const noexcept -> std::string_view
    {
        return "Premisses";
    }

    auto PluginContext::getHostVersion() const noexcept -> std::string_view
    {
        return "1.0.0";
    }

    auto PluginContext::getPluginApiVersion() const noexcept -> std::string_view
    {
        return "1.0.0";
    }

    // ============================================
    // Additional Methods
    // ============================================

    auto PluginContext::setPluginDirectory(const std::filesystem::path& path) -> void
    {
        pluginDirectory_ = path;
    }

    auto PluginContext::setDataDirectory(const std::filesystem::path& path) -> void
    {
        dataDirectory_ = path;
    }

    auto PluginContext::setConfigDirectory(const std::filesystem::path& path) -> void
    {
        configDirectory_ = path;
    }

    auto PluginContext::setTempDirectory(const std::filesystem::path& path) -> void
    {
        tempDirectory_ = path;
    }

    auto PluginContext::setApplicationDirectory(const std::filesystem::path& path) -> void
    {
        applicationDirectory_ = path;
    }

    auto PluginContext::loadConfiguration(const std::filesystem::path& path) -> bool
    {
        try
        {
            std::ifstream file(path);
            if (!file)
            {
                return false;
            }

            std::unique_lock lock(configMutex_);
            std::string line;
            while (std::getline(file, line))
            {
                if (line.empty() || line[0] == '#')
                {
                    continue;
                }

                auto pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    // Trim whitespace
                    while (!key.empty() && std::isspace(key.back())) key.pop_back();
                    while (!value.empty() && std::isspace(value.front())) value.erase(0, 1);

                    configuration_[key] = value;
                }
            }

            LOG4CXX_DEBUG(logger, "Loaded configuration from: " << path);
            return true;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Failed to load configuration: " << e.what());
            return false;
        }
    }

} // namespace com::github::doevelopper::atlassians::plugin::context
