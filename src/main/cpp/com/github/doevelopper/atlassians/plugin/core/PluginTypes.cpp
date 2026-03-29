/**
 * @file PluginTypes.cpp
 * @brief Plugin types implementation - utility functions not defined in header
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    // ============================================
    // PluginState Parse from String
    // ============================================

    auto pluginStateFromString(std::string_view str) noexcept -> PluginState
    {
        if (str == "Unloaded") return PluginState::Unloaded;
        if (str == "Discovered") return PluginState::Discovered;
        if (str == "Loaded") return PluginState::Loaded;
        if (str == "Initialized") return PluginState::Initialized;
        if (str == "Starting") return PluginState::Starting;
        if (str == "Started") return PluginState::Started;
        if (str == "Stopping") return PluginState::Stopping;
        if (str == "Stopped") return PluginState::Stopped;
        if (str == "Failed") return PluginState::Failed;
        if (str == "Disabled") return PluginState::Disabled;
        return PluginState::Unloaded;
    }

    // ============================================
    // IsolationLevel String Conversion
    // ============================================

    auto toString(IsolationLevel level) noexcept -> std::string_view
    {
        switch (level)
        {
            case IsolationLevel::None:
                return "None";
            case IsolationLevel::Thread:
                return "Thread";
            case IsolationLevel::Process:
                return "Process";
        }
        return "None";
    }

    auto isolationLevelFromString(std::string_view str) noexcept -> IsolationLevel
    {
        if (str == "None") return IsolationLevel::None;
        if (str == "Thread") return IsolationLevel::Thread;
        if (str == "Process") return IsolationLevel::Process;
        return IsolationLevel::None;
    }

    // ============================================
    // PluginPriority String Conversion
    // ============================================

    auto toString(PluginPriority priority) noexcept -> std::string_view
    {
        switch (priority)
        {
            case PluginPriority::Critical:
                return "Critical";
            case PluginPriority::High:
                return "High";
            case PluginPriority::Normal:
                return "Normal";
            case PluginPriority::Low:
                return "Low";
            case PluginPriority::Optional:
                return "Optional";
        }
        return "Normal";
    }

    auto pluginPriorityFromString(std::string_view str) noexcept -> PluginPriority
    {
        if (str == "Critical") return PluginPriority::Critical;
        if (str == "High") return PluginPriority::High;
        if (str == "Normal") return PluginPriority::Normal;
        if (str == "Low") return PluginPriority::Low;
        if (str == "Optional") return PluginPriority::Optional;
        return PluginPriority::Normal;
    }

    // ============================================
    // PluginCapability String Conversion
    // ============================================

    auto toString(PluginCapability capability) noexcept -> std::string_view
    {
        switch (capability)
        {
            case PluginCapability::None:
                return "None";
            case PluginCapability::HotReloadable:
                return "HotReloadable";
            case PluginCapability::ThreadSafe:
                return "ThreadSafe";
            case PluginCapability::StatePersistent:
                return "StatePersistent";
            case PluginCapability::NetworkEnabled:
                return "NetworkEnabled";
            case PluginCapability::FileSystemAccess:
                return "FileSystemAccess";
            case PluginCapability::UIProvider:
                return "UIProvider";
            case PluginCapability::ServiceProvider:
                return "ServiceProvider";
            case PluginCapability::EventPublisher:
                return "EventPublisher";
            case PluginCapability::EventSubscriber:
                return "EventSubscriber";
            case PluginCapability::Configurable:
                return "Configurable";
            default:
                return "Unknown";
        }
    }

    // ============================================
    // LogLevel Parse from String
    // ============================================

    auto logLevelFromString(std::string_view str) noexcept -> LogLevel
    {
        if (str == "TRACE" || str == "Trace") return LogLevel::Trace;
        if (str == "DEBUG" || str == "Debug") return LogLevel::Debug;
        if (str == "INFO" || str == "Info") return LogLevel::Info;
        if (str == "WARNING" || str == "Warning") return LogLevel::Warning;
        if (str == "ERROR" || str == "Error") return LogLevel::Error;
        if (str == "CRITICAL" || str == "Critical") return LogLevel::Critical;
        if (str == "FATAL" || str == "Fatal") return LogLevel::Fatal;
        return LogLevel::Info;
    }

    // ============================================
    // Capability Helper Functions (non-constexpr modifiers)
    // ============================================

    auto addCapability(PluginCapability& capabilities, PluginCapability toAdd) noexcept -> void
    {
        capabilities = capabilities | toAdd;
    }

    auto removeCapability(PluginCapability& capabilities, PluginCapability toRemove) noexcept -> void
    {
        capabilities = static_cast<PluginCapability>(
            static_cast<std::uint32_t>(capabilities) & ~static_cast<std::uint32_t>(toRemove));
    }

} // namespace com::github::doevelopper::atlassians::plugin
