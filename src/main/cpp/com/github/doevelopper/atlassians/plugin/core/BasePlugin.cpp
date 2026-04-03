/**
 * @file BasePlugin.cpp
 * @brief Implementation of BasePlugin class
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/plugin/core/BasePlugin.hpp>

using namespace com::github::doevelopper::atlassians::plugin::core;
using namespace com::github::doevelopper::atlassians::plugin;

log4cxx::LoggerPtr BasePlugin::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.core.BasePlugin"));

// ============================================
// Constructors and Destructor
// ============================================

BasePlugin::BasePlugin() noexcept
    : metadata_()
    , context_(nullptr)
    , state_(PluginState::Unloaded)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

BasePlugin::BasePlugin(metadata::PluginMetadata metadata) noexcept
    : metadata_(std::move(metadata))
    , context_(nullptr)
    , state_(PluginState::Unloaded)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

BasePlugin::~BasePlugin() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Ensure proper cleanup if plugin wasn't shutdown properly
    if (state_ != PluginState::Unloaded && state_ != PluginState::Failed)
    {
        try
        {
            if (state_ == PluginState::Started)
            {
                LOG4CXX_WARN(logger, "Plugin '" << metadata_.getName()
                             << "' being destroyed while still started. Forcing stop.");
                onStop();
            }

            if (state_ != PluginState::Unloaded)
            {
                LOG4CXX_WARN(logger, "Plugin '" << metadata_.getName()
                             << "' being destroyed without proper shutdown.");
                onShutdown();
            }
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Error during plugin destruction: " << e.what());
        }
    }
}

BasePlugin::BasePlugin(BasePlugin&& other) noexcept
    : metadata_(std::move(other.metadata_))
    , context_(other.context_)
    , state_(other.state_)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    other.context_ = nullptr;
    other.state_ = PluginState::Unloaded;
}

auto BasePlugin::operator=(BasePlugin&& other) noexcept -> BasePlugin&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        metadata_ = std::move(other.metadata_);
        context_ = other.context_;
        state_ = other.state_;
        other.context_ = nullptr;
        other.state_ = PluginState::Unloaded;
    }
    return *this;
}

// ============================================
// IPlugin Implementation
// ============================================

auto BasePlugin::initialize(IPluginContext& context) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::lock_guard<std::mutex> lock(stateMutex_);

    validateStateTransition(PluginState::Initialized);

    LOG4CXX_INFO(logger, "Initializing plugin: " << metadata_.getName()
                 << " v" << metadata_.getVersion());

    try
    {
        context_ = &context;
        onInitialize(context);
        state_ = PluginState::Initialized;

        LOG4CXX_INFO(logger, "Plugin initialized: " << metadata_.getName());
    }
    catch (const std::exception& e)
    {
        state_ = PluginState::Failed;
        context_ = nullptr;
        LOG4CXX_ERROR(logger, "Failed to initialize plugin '" << metadata_.getName()
                      << "': " << e.what());
        throw PluginInitializationException(
            std::string(metadata_.getName()),
            e.what()
        );
    }
}

auto BasePlugin::start() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::lock_guard<std::mutex> lock(stateMutex_);

    validateStateTransition(PluginState::Started);

    LOG4CXX_INFO(logger, "Starting plugin: " << metadata_.getName());

    try
    {
        state_ = PluginState::Starting;
        onStart();
        state_ = PluginState::Started;

        LOG4CXX_INFO(logger, "Plugin started: " << metadata_.getName());
    }
    catch (const std::exception& e)
    {
        state_ = PluginState::Failed;
        LOG4CXX_ERROR(logger, "Failed to start plugin '" << metadata_.getName()
                      << "': " << e.what());
        throw PluginStateException(
            std::string(metadata_.getName()),
            "Starting",
            "Started"
        );
    }
}

auto BasePlugin::stop() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::lock_guard<std::mutex> lock(stateMutex_);

    validateStateTransition(PluginState::Stopped);

    LOG4CXX_INFO(logger, "Stopping plugin: " << metadata_.getName());

    try
    {
        state_ = PluginState::Stopping;
        onStop();
        state_ = PluginState::Stopped;

        LOG4CXX_INFO(logger, "Plugin stopped: " << metadata_.getName());
    }
    catch (const std::exception& e)
    {
        state_ = PluginState::Failed;
        LOG4CXX_ERROR(logger, "Failed to stop plugin '" << metadata_.getName()
                      << "': " << e.what());
        throw PluginStateException(
            std::string(metadata_.getName()),
            "Stopping",
            "Stopped"
        );
    }
}

auto BasePlugin::shutdown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::lock_guard<std::mutex> lock(stateMutex_);

    // Allow shutdown from most states
    if (state_ == PluginState::Unloaded)
    {
        return; // Already unloaded
    }

    LOG4CXX_INFO(logger, "Shutting down plugin: " << metadata_.getName());

    try
    {
        // Stop first if still running
        if (state_ == PluginState::Started || state_ == PluginState::Starting)
        {
            state_ = PluginState::Stopping;
            onStop();
            state_ = PluginState::Stopped;
        }

        onShutdown();
        context_ = nullptr;
        state_ = PluginState::Unloaded;

        LOG4CXX_INFO(logger, "Plugin shutdown complete: " << metadata_.getName());
    }
    catch (const std::exception& e)
    {
        state_ = PluginState::Failed;
        LOG4CXX_ERROR(logger, "Error during plugin shutdown '" << metadata_.getName()
                      << "': " << e.what());
        // Don't rethrow during shutdown to allow cleanup to continue
    }
}

auto BasePlugin::getMetadata() const noexcept -> const IPluginMetadata&
{
    return metadata_;
}

auto BasePlugin::getState() const noexcept -> PluginState
{
    std::lock_guard<std::mutex> lock(stateMutex_);
    return state_;
}

auto BasePlugin::hasCapability(std::string_view capability) const -> bool
{
    return metadata_.hasCapability(capability);
}

auto BasePlugin::getProvidedServices() const -> std::vector<std::string>
{
    return std::vector<std::string>(
        metadata_.getProvidedServices().begin(),
        metadata_.getProvidedServices().end()
    );
}

auto BasePlugin::getRequiredServices() const -> std::vector<std::string>
{
    return std::vector<std::string>(
        metadata_.getRequiredServices().begin(),
        metadata_.getRequiredServices().end()
    );
}

// ============================================
// Protected Methods
// ============================================

auto BasePlugin::getContext() const noexcept -> IPluginContext*
{
    return context_;
}

auto BasePlugin::metadata() noexcept -> metadata::PluginMetadata&
{
    return metadata_;
}

auto BasePlugin::setState(PluginState state) -> void
{
    std::lock_guard<std::mutex> lock(stateMutex_);
    state_ = state;
}

auto BasePlugin::logMessage(LogLevel level, std::string_view message) const -> void
{
    if (context_)
    {
        context_->log(level, message);
    }
    else
    {
        // Fallback to direct logging
        switch (level)
        {
            case LogLevel::Trace:
                LOG4CXX_TRACE(logger, message);
                break;
            case LogLevel::Debug:
                LOG4CXX_DEBUG(logger, message);
                break;
            case LogLevel::Info:
                LOG4CXX_INFO(logger, message);
                break;
            case LogLevel::Warning:
                LOG4CXX_WARN(logger, message);
                break;
            case LogLevel::Error:
            case LogLevel::Critical:
            case LogLevel::Fatal:
                LOG4CXX_ERROR(logger, message);
                break;
        }
    }
}

// ============================================
// Private Methods
// ============================================

auto BasePlugin::validateStateTransition(PluginState targetState) const -> void
{
    if (!isValidTransition(state_, targetState))
    {
        throw PluginStateException(
            std::string(metadata_.getName()),
            std::string(toString(state_)),
            std::string(toString(targetState))
        );
    }
}

auto BasePlugin::isValidTransition(PluginState from, PluginState to) const noexcept -> bool
{
    // Define valid state transitions
    switch (from)
    {
        case PluginState::Unloaded:
        case PluginState::Discovered:
        case PluginState::Loaded:
            return to == PluginState::Initialized;

        case PluginState::Initialized:
            return to == PluginState::Started || to == PluginState::Unloaded;

        case PluginState::Starting:
            return to == PluginState::Started || to == PluginState::Failed;

        case PluginState::Started:
            return to == PluginState::Stopped || to == PluginState::Stopping;

        case PluginState::Stopping:
            return to == PluginState::Stopped || to == PluginState::Failed;

        case PluginState::Stopped:
            return to == PluginState::Started || to == PluginState::Unloaded;

        case PluginState::Failed:
            return to == PluginState::Unloaded; // Can only unload from failed state

        case PluginState::Disabled:
            return to == PluginState::Unloaded;
    }

    return false;
}
