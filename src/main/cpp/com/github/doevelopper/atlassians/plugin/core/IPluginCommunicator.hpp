/**
 * @file IPluginCommunicator.hpp
 * @brief Interface for inter-plugin communication
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCOMMUNICATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCOMMUNICATOR_HPP

#include <any>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for inter-plugin communication.
     *
     * The plugin communicator provides mechanisms for:
     * - Event-based publish/subscribe communication
     * - Direct request/response communication
     * - Asynchronous messaging
     *
     * This interface follows the Mediator pattern to decouple
     * plugins from each other.
     */
    class IPluginCommunicator
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginCommunicator() noexcept = default;

        // ============================================
        // Event Publishing
        // ============================================

        /**
         * @brief Publish an event to all subscribers.
         * @param event The event to publish.
         */
        virtual auto publish(const PluginEvent& event) -> void = 0;

        /**
         * @brief Publish an event by type and payload.
         * @param eventType The type of event.
         * @param payload The event data.
         * @param sourcePlugin The name of the publishing plugin.
         */
        virtual auto publish(std::string_view eventType,
                             const EventPayload& payload,
                             std::string_view sourcePlugin = "") -> void = 0;

        /**
         * @brief Publish an event asynchronously.
         * @param event The event to publish.
         * @return Future that completes when all handlers have been invoked.
         */
        [[nodiscard]] virtual auto publishAsync(const PluginEvent& event) -> std::future<void> = 0;

        // ============================================
        // Event Subscription
        // ============================================

        /**
         * @brief Subscribe to events of a specific type.
         * @param eventType The type of events to subscribe to (can use wildcards).
         * @param handler The handler to call when events occur.
         * @return A subscription ID for unsubscribing.
         */
        [[nodiscard]] virtual auto subscribe(std::string_view eventType,
                                              EventHandler handler) -> std::uint64_t = 0;

        /**
         * @brief Subscribe to events with a priority.
         * @param eventType The type of events to subscribe to.
         * @param handler The handler to call.
         * @param priority Higher priority handlers are called first.
         * @return A subscription ID for unsubscribing.
         */
        [[nodiscard]] virtual auto subscribe(std::string_view eventType,
                                              EventHandler handler,
                                              int priority) -> std::uint64_t = 0;

        /**
         * @brief Unsubscribe from events.
         * @param subscriptionId The ID returned from subscribe().
         */
        virtual auto unsubscribe(std::uint64_t subscriptionId) -> void = 0;

        /**
         * @brief Unsubscribe all handlers from an event type.
         * @param eventType The event type to unsubscribe from.
         */
        virtual auto unsubscribeAll(std::string_view eventType) -> void = 0;

        /**
         * @brief Unsubscribe all handlers registered by a plugin.
         * @param pluginName The name of the plugin.
         */
        virtual auto unsubscribePlugin(std::string_view pluginName) -> void = 0;

        // ============================================
        // Synchronous Request/Response
        // ============================================

        /**
         * @brief Send a request to a specific plugin and wait for response.
         * @param targetPlugin The name of the target plugin.
         * @param method The method to invoke.
         * @param params The method parameters.
         * @param timeout Maximum time to wait for response.
         * @return The response data, or empty if timeout or error.
         */
        [[nodiscard]] virtual auto request(std::string_view targetPlugin,
                                            std::string_view method,
                                            const std::any& params = {},
                                            std::chrono::milliseconds timeout = std::chrono::seconds(30))
            -> std::optional<std::any> = 0;

        /**
         * @brief Send a typed request and receive a typed response.
         * @tparam TResult The expected result type.
         * @tparam TParams The parameter type.
         * @param targetPlugin The target plugin name.
         * @param method The method to invoke.
         * @param params The method parameters.
         * @param timeout Maximum wait time.
         * @return The typed response, or empty if timeout/error.
         */
        template<typename TResult, typename TParams = std::monostate>
        [[nodiscard]] auto request(std::string_view targetPlugin,
                                   std::string_view method,
                                   const TParams& params = {},
                                   std::chrono::milliseconds timeout = std::chrono::seconds(30))
            -> std::optional<TResult>
        {
            auto result = request(targetPlugin, method, std::any(params), timeout);
            if (result.has_value())
            {
                try
                {
                    return std::any_cast<TResult>(*result);
                }
                catch (const std::bad_any_cast&)
                {
                    return std::nullopt;
                }
            }
            return std::nullopt;
        }

        // ============================================
        // Asynchronous Request/Response
        // ============================================

        /**
         * @brief Send an asynchronous request to a plugin.
         * @param targetPlugin The target plugin name.
         * @param method The method to invoke.
         * @param params The method parameters.
         * @return Future containing the response.
         */
        [[nodiscard]] virtual auto requestAsync(std::string_view targetPlugin,
                                                 std::string_view method,
                                                 const std::any& params = {})
            -> std::future<std::optional<std::any>> = 0;

        /**
         * @brief Send a typed asynchronous request.
         * @tparam TResult The expected result type.
         * @tparam TParams The parameter type.
         * @param targetPlugin The target plugin name.
         * @param method The method to invoke.
         * @param params The method parameters.
         * @return Future containing the typed response.
         */
        template<typename TResult, typename TParams = std::monostate>
        [[nodiscard]] auto requestAsync(std::string_view targetPlugin,
                                        std::string_view method,
                                        const TParams& params = {})
            -> std::future<std::optional<TResult>>
        {
            return std::async(std::launch::async, [this, targetPlugin = std::string(targetPlugin),
                                                    method = std::string(method), params]() {
                return request<TResult, TParams>(targetPlugin, method, params);
            });
        }

        // ============================================
        // Request Handlers
        // ============================================

        /**
         * @brief Request handler type for processing incoming requests.
         */
        using RequestHandler = std::function<std::any(const std::any& params)>;

        /**
         * @brief Register a handler for incoming requests.
         * @param method The method name to handle.
         * @param handler The handler function.
         * @return True if registration was successful.
         */
        [[nodiscard]] virtual auto registerHandler(std::string_view method,
                                                    RequestHandler handler) -> bool = 0;

        /**
         * @brief Unregister a request handler.
         * @param method The method name.
         */
        virtual auto unregisterHandler(std::string_view method) -> void = 0;

        // ============================================
        // Broadcast
        // ============================================

        /**
         * @brief Broadcast a request to all plugins and collect responses.
         * @param method The method to invoke.
         * @param params The method parameters.
         * @param timeout Maximum time to wait.
         * @return Map of plugin names to their responses.
         */
        [[nodiscard]] virtual auto broadcast(std::string_view method,
                                              const std::any& params = {},
                                              std::chrono::milliseconds timeout = std::chrono::seconds(30))
            -> std::vector<std::pair<std::string, std::any>> = 0;

        // ============================================
        // Utilities
        // ============================================

        /**
         * @brief Check if a plugin is reachable.
         * @param pluginName The name of the plugin.
         * @return True if the plugin can receive messages.
         */
        [[nodiscard]] virtual auto isReachable(std::string_view pluginName) const noexcept
            -> bool = 0;

        /**
         * @brief Get the number of subscribers for an event type.
         * @param eventType The event type.
         * @return Number of subscribers.
         */
        [[nodiscard]] virtual auto getSubscriberCount(std::string_view eventType) const noexcept
            -> std::size_t = 0;

        /**
         * @brief Get all event types with subscribers.
         * @return Vector of event type names.
         */
        [[nodiscard]] virtual auto getSubscribedEventTypes() const
            -> std::vector<std::string> = 0;

    protected:
        IPluginCommunicator() = default;
        IPluginCommunicator(const IPluginCommunicator&) = default;
        IPluginCommunicator(IPluginCommunicator&&) = default;
        IPluginCommunicator& operator=(const IPluginCommunicator&) = default;
        IPluginCommunicator& operator=(IPluginCommunicator&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINCOMMUNICATOR_HPP
