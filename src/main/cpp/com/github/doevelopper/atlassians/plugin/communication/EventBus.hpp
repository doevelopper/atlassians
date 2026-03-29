/**
 * @file EventBus.hpp
 * @brief Inter-plugin communication event bus implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_COMMUNICATION_EVENTBUS_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_COMMUNICATION_EVENTBUS_HPP

#include <any>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginCommunicator.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::communication
{
    /**
     * @brief Delivery mode for events.
     */
    enum class EventDeliveryMode
    {
        Synchronous,   ///< Block until all handlers complete
        Asynchronous,  ///< Queue for async delivery
        Immediate      ///< Call handlers immediately, no queuing
    };

    /**
     * @brief Request handler with timeout support.
     */
    template<typename Response>
    using RequestHandler = std::function<Response(const std::any& request)>;

    /**
     * @brief Thread-safe event bus for inter-plugin communication.
     *
     * Implements the Observer pattern with support for:
     * - Topic-based publish/subscribe
     * - Request/response communication
     * - Synchronous and asynchronous delivery
     * - Event prioritization
     * - Dead letter handling
     */
    class EventBus : public IPluginCommunicator
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Configuration for the event bus.
         */
        struct Config
        {
            std::size_t maxQueueSize{10000};
            std::chrono::milliseconds defaultTimeout{5000};
            bool enableDeadLetterQueue{true};
            EventDeliveryMode defaultDeliveryMode{EventDeliveryMode::Synchronous};
        };

        /**
         * @brief Subscription entry.
         */
        struct Subscription
        {
            std::string subscriberId;
            EventHandler handler;
            int priority{0};
            std::chrono::system_clock::time_point createdAt;
        };

        /**
         * @brief Request handler entry.
         */
        struct RequestHandlerEntry
        {
            std::string handlerId;
            std::function<std::any(const std::any&)> handler;
        };

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Construct with default configuration.
         */
        EventBus();

        /**
         * @brief Construct with custom configuration.
         * @param config Configuration settings
         */
        explicit EventBus(const Config& config);

        /**
         * @brief Destructor - stops async processing.
         */
        ~EventBus() noexcept override;

        // Delete copy operations
        EventBus(const EventBus&) = delete;
        auto operator=(const EventBus&) -> EventBus& = delete;

        // Allow move operations
        EventBus(EventBus&& other) noexcept;
        auto operator=(EventBus&& other) noexcept -> EventBus&;

        // ============================================
        // IPluginCommunicator Implementation
        // ============================================

        auto publish(const PluginEvent& event) -> void override;

        auto publish(std::string_view eventType,
                     const EventPayload& payload,
                     std::string_view sourcePlugin = "") -> void override;

        [[nodiscard]] auto publishAsync(const PluginEvent& event) -> std::future<void> override;

        [[nodiscard]] auto subscribe(std::string_view eventType,
                                     EventHandler handler) -> std::uint64_t override;

        [[nodiscard]] auto subscribe(std::string_view eventType,
                                     EventHandler handler,
                                     int priority) -> std::uint64_t override;

        auto unsubscribe(std::uint64_t subscriptionId) -> void override;

        auto unsubscribeAll(std::string_view eventType) -> void override;

        auto unsubscribePlugin(std::string_view pluginName) -> void override;

        [[nodiscard]] auto request(std::string_view targetPlugin,
                                   std::string_view method,
                                   const std::any& params = {},
                                   std::chrono::milliseconds timeout = std::chrono::seconds(30))
            -> std::optional<std::any> override;

        [[nodiscard]] auto requestAsync(std::string_view targetPlugin,
                                        std::string_view method,
                                        const std::any& params = {})
            -> std::future<std::optional<std::any>> override;

        [[nodiscard]] auto registerHandler(std::string_view method,
                                           RequestHandler handler) -> bool override;

        auto unregisterHandler(std::string_view method) -> void override;

        [[nodiscard]] auto broadcast(std::string_view method,
                                     const std::any& params = {},
                                     std::chrono::milliseconds timeout = std::chrono::seconds(30))
            -> std::vector<std::pair<std::string, std::any>> override;

        [[nodiscard]] auto isReachable(std::string_view pluginName) const noexcept -> bool override;

        [[nodiscard]] auto getSubscriberCount(std::string_view eventType) const noexcept
            -> std::size_t override;

        [[nodiscard]] auto getSubscribedEventTypes() const -> std::vector<std::string> override;

        // ============================================
        // Extended Methods (not in interface)
        // ============================================

        /**
         * @brief Check if a topic has any subscribers.
         */
        [[nodiscard]] auto hasSubscribers(std::string_view topic) const noexcept -> bool;

        /**
         * @brief Get all registered topics.
         */
        [[nodiscard]] auto getTopics() const -> std::vector<std::string>;

        /**
         * @brief Enable a topic.
         */
        auto enableTopic(std::string_view topic) -> void;

        /**
         * @brief Disable a topic.
         */
        auto disableTopic(std::string_view topic) -> void;

        /**
         * @brief Check if a topic is enabled.
         */
        [[nodiscard]] auto isTopicEnabled(std::string_view topic) const noexcept -> bool;

        /**
         * @brief Subscribe with priority.
         * @param topic Topic to subscribe to
         * @param handler Event handler
         * @param subscriberId Subscriber identifier
         * @param priority Handler priority (higher = called first)
         * @return Subscription ID
         */
        [[nodiscard]] auto subscribeWithPriority(std::string_view topic,
                                                  EventHandler handler,
                                                  std::string_view subscriberId,
                                                  int priority) -> std::uint64_t;

        /**
         * @brief Publish with specific delivery mode.
         * @param topic Topic to publish to
         * @param event Event to publish
         * @param mode Delivery mode
         */
        auto publish(std::string_view topic,
                    const PluginEvent& event,
                    EventDeliveryMode mode) -> void;

        /**
         * @brief Get dead letter queue.
         * @return Dead letter events
         */
        [[nodiscard]] auto getDeadLetters() const -> std::vector<PluginEvent>;

        /**
         * @brief Clear dead letter queue.
         */
        auto clearDeadLetters() -> void;

        /**
         * @brief Start async event processing.
         */
        auto startAsyncProcessing() -> void;

        /**
         * @brief Stop async event processing.
         */
        auto stopAsyncProcessing() -> void;

        /**
         * @brief Get current configuration.
         * @return Configuration
         */
        [[nodiscard]] auto getConfig() const noexcept -> const Config&;

        /**
         * @brief Get statistics.
         */
        struct Statistics
        {
            std::uint64_t totalEventsPublished{0};
            std::uint64_t totalEventsDelivered{0};
            std::uint64_t totalEventsFailed{0};
            std::uint64_t totalRequestsSent{0};
            std::uint64_t totalRequestsHandled{0};
            std::uint64_t deadLetterCount{0};
        };

        [[nodiscard]] auto getStatistics() const noexcept -> Statistics;

    private:
        // ============================================
        // Internal Methods
        // ============================================

        auto deliverEvent(const std::string& topic, const PluginEvent& event) -> void;
        auto processAsyncQueue() -> void;
        auto addToDeadLetter(const PluginEvent& event, const std::string& reason) -> void;

        // ============================================
        // Member Variables
        // ============================================

        Config config_;

        // Topic -> Subscriptions (sorted by priority)
        std::map<std::string, std::vector<Subscription>> subscriptions_;
        std::map<std::uint64_t, std::pair<std::string, std::size_t>> subscriptionIndex_;
        std::atomic<std::uint64_t> nextSubscriptionId_{1};

        // Request handlers: RequestType -> Handler
        std::map<std::string, std::vector<RequestHandlerEntry>> requestHandlers_;

        // Disabled topics
        std::set<std::string> disabledTopics_;

        // Async event queue
        struct QueuedEvent
        {
            std::string topic;
            PluginEvent event;
        };
        std::queue<QueuedEvent> asyncQueue_;
        std::condition_variable queueCondition_;
        std::mutex queueMutex_;

        // Async processing thread
        std::unique_ptr<std::thread> processingThread_;
        std::atomic<bool> stopProcessing_{false};

        // Dead letter queue
        std::vector<std::pair<PluginEvent, std::string>> deadLetterQueue_;

        // Statistics
        mutable std::atomic<std::uint64_t> totalEventsPublished_{0};
        mutable std::atomic<std::uint64_t> totalEventsDelivered_{0};
        mutable std::atomic<std::uint64_t> totalEventsFailed_{0};
        mutable std::atomic<std::uint64_t> totalRequestsSent_{0};
        mutable std::atomic<std::uint64_t> totalRequestsHandled_{0};

        // Mutexes
        mutable std::shared_mutex subscriptionMutex_;
        mutable std::shared_mutex handlerMutex_;
        mutable std::shared_mutex topicMutex_;
        mutable std::mutex deadLetterMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::communication

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_COMMUNICATION_EVENTBUS_HPP
