/**
 * @file BasePlugin.hpp
 * @brief Abstract base class for plugin implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_BASEPLUGIN_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_BASEPLUGIN_HPP

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginContext.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>
#include <com/github/doevelopper/atlassians/plugin/metadata/PluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::core
{
    /**
     * @brief Abstract base class for plugin implementation.
     *
     * This class provides a Template Method pattern implementation for
     * plugin lifecycle management. Plugin doeveloppers should extend this
     * class and implement the abstract methods:
     * - onInitialize()
     * - onStart()
     * - onStop()
     * - onShutdown()
     *
     * The base class handles:
     * - State management
     * - Lifecycle transitions
     * - Error handling
     * - Thread safety
     */
    class BasePlugin : public IPlugin
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Default constructor.
         */
        BasePlugin() noexcept;

        /**
         * @brief Constructor with metadata.
         */
        explicit BasePlugin(metadata::PluginMetadata metadata) noexcept;

        /**
         * @brief Virtual destructor.
         */
        ~BasePlugin() noexcept override;

        // Delete copy operations
        BasePlugin(const BasePlugin&) = delete;
        auto operator=(const BasePlugin&) -> BasePlugin& = delete;

        // Allow move operations
        BasePlugin(BasePlugin&& other) noexcept;
        auto operator=(BasePlugin&& other) noexcept -> BasePlugin&;

        // ============================================
        // IPlugin Implementation (Template Method Pattern)
        // ============================================

        /**
         * @brief Initialize the plugin (Template Method).
         *
         * This method manages state transitions and calls onInitialize().
         *
         * @param context The plugin context.
         * @throws PluginInitializationException if initialization fails.
         */
        auto initialize(IPluginContext& context) -> void final;

        /**
         * @brief Start the plugin (Template Method).
         *
         * This method manages state transitions and calls onStart().
         *
         * @throws PluginStateException if the plugin cannot be started.
         */
        auto start() -> void final;

        /**
         * @brief Stop the plugin (Template Method).
         *
         * This method manages state transitions and calls onStop().
         *
         * @throws PluginStateException if the plugin cannot be stopped.
         */
        auto stop() -> void final;

        /**
         * @brief Shutdown the plugin (Template Method).
         *
         * This method manages state transitions and calls onShutdown().
         */
        auto shutdown() -> void final;

        /**
         * @brief Get the plugin metadata.
         */
        [[nodiscard]] auto getMetadata() const noexcept -> const IPluginMetadata& override;

        /**
         * @brief Get the current plugin state.
         */
        [[nodiscard]] auto getState() const noexcept -> PluginState override;

        /**
         * @brief Check if the plugin has a capability.
         */
        [[nodiscard]] auto hasCapability(std::string_view capability) const -> bool override;

        /**
         * @brief Get provided services.
         */
        [[nodiscard]] auto getProvidedServices() const -> std::vector<std::string> override;

        /**
         * @brief Get required services.
         */
        [[nodiscard]] auto getRequiredServices() const -> std::vector<std::string> override;

    protected:
        // ============================================
        // Template Methods (to be overridden)
        // ============================================

        /**
         * @brief Called during initialization.
         *
         * Override this method to perform plugin-specific initialization.
         *
         * @param context The plugin context.
         * @throws PluginInitializationException if initialization fails.
         */
        virtual auto onInitialize(IPluginContext& context) -> void = 0;

        /**
         * @brief Called when the plugin starts.
         *
         * Override this method to start plugin operations.
         */
        virtual auto onStart() -> void = 0;

        /**
         * @brief Called when the plugin stops.
         *
         * Override this method to stop plugin operations.
         */
        virtual auto onStop() -> void = 0;

        /**
         * @brief Called during shutdown.
         *
         * Override this method to release resources.
         */
        virtual auto onShutdown() -> void = 0;

        // ============================================
        // Protected Accessors
        // ============================================

        /**
         * @brief Get the plugin context.
         * @return Pointer to the context, or nullptr if not initialized.
         */
        [[nodiscard]] auto getContext() const noexcept -> IPluginContext*;

        /**
         * @brief Get mutable metadata for subclass modification.
         */
        [[nodiscard]] auto metadata() noexcept -> metadata::PluginMetadata&;

        /**
         * @brief Set the plugin state.
         *
         * Subclasses can use this to set custom states (e.g., Failed).
         */
        auto setState(PluginState state) -> void;

        /**
         * @brief Log a message using the plugin context.
         */
        auto logMessage(LogLevel level, std::string_view message) const -> void;

    private:
        // ============================================
        // State Validation
        // ============================================

        /**
         * @brief Validate that a state transition is allowed.
         */
        auto validateStateTransition(PluginState targetState) const -> void;

        /**
         * @brief Check if transition from current state to target is valid.
         */
        [[nodiscard]] auto isValidTransition(PluginState from, PluginState to) const noexcept -> bool;

        // ============================================
        // Member Variables
        // ============================================

        metadata::PluginMetadata metadata_;
        IPluginContext* context_{nullptr};
        PluginState state_{PluginState::Unloaded};
        mutable std::mutex stateMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::core

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_BASEPLUGIN_HPP
