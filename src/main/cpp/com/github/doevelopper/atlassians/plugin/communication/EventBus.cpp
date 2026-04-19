/**
 * @file EventBus.cpp
 * @brief Inter-plugin communication event bus implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <algorithm>

#include <com/github/doevelopper/atlassians/plugin/communication/EventBus.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

namespace com::github::doevelopper::atlassians::plugin::communication
{
    log4cxx::LoggerPtr EventBus::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.communication.EventBus"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    EventBus::EventBus()
        : EventBus(Config{})
    {
    }

    EventBus::EventBus(const Config& config)
        : config_(config)
    {
        LOG4CXX_TRACE(logger, "EventBus created with max queue size: " << config_.maxQueueSize);
    }

    EventBus::~EventBus() noexcept
    {
        stopAsyncProcessing();
        LOG4CXX_TRACE(logger, "EventBus destroyed");
    }

    EventBus::EventBus(EventBus&& other) noexcept
        : config_(other.config_)
    {
        std::scoped_lock lock(other.subscriptionMutex_, other.handlerMutex_);
        subscriptions_ = std::move(other.subscriptions_);
        subscriptionIndex_ = std::move(other.subscriptionIndex_);
        nextSubscriptionId_.store(other.nextSubscriptionId_.load());
        requestHandlers_ = std::move(other.requestHandlers_);
        disabledTopics_ = std::move(other.disabledTopics_);
        deadLetterQueue_ = std::move(other.deadLetterQueue_);
    }

    auto EventBus::operator=(EventBus&& other) noexcept -> EventBus&
    {
        if (this != &other)
        {
            stopAsyncProcessing();
            std::scoped_lock lock(subscriptionMutex_, other.subscriptionMutex_,
                                  handlerMutex_, other.handlerMutex_);
            config_ = other.config_;
            subscriptions_ = std::move(other.subscriptions_);
            subscriptionIndex_ = std::move(other.subscriptionIndex_);
            nextSubscriptionId_.store(other.nextSubscriptionId_.load());
            requestHandlers_ = std::move(other.requestHandlers_);
            disabledTopics_ = std::move(other.disabledTopics_);
            deadLetterQueue_ = std::move(other.deadLetterQueue_);
        }
        return *this;
    }

    // ============================================
    // IPluginCommunicator Implementation
    // ============================================

    auto EventBus::publish(const PluginEvent& event) -> void
    {
        publish(event.eventType, event, config_.defaultDeliveryMode);
    }

    auto EventBus::publish(std::string_view eventType,
                           const EventPayload& payload,
                           std::string_view sourcePlugin) -> void
    {
        PluginEvent event;
        event.eventType = std::string(eventType);
        event.payload = payload;
        event.sourcePlugin = std::string(sourcePlugin);
        event.timestamp = std::chrono::system_clock::now();
        publish(event);
    }

    auto EventBus::publishAsync(const PluginEvent& event) -> std::future<void>
    {
        return std::async(std::launch::async, [this, event]() {
            publish(event);
        });
    }

    auto EventBus::subscribe(std::string_view eventType, EventHandler handler) -> std::uint64_t
    {
        return subscribe(eventType, std::move(handler), 0);
    }

    auto EventBus::subscribe(std::string_view eventType,
                             EventHandler handler,
                             int priority) -> std::uint64_t
    {
        return subscribeWithPriority(eventType, std::move(handler), "", priority);
    }

    auto EventBus::subscribeWithPriority(std::string_view topic,
                                          EventHandler handler,
                                          std::string_view subscriberId,
                                          int priority) -> std::uint64_t
    {
        const std::string topicStr(topic);
        const std::string subId(subscriberId);

        LOG4CXX_DEBUG(logger, "Subscribe to topic '" << topicStr
                     << "' by '" << subId << "' with priority " << priority);

        std::unique_lock lock(subscriptionMutex_);

        Subscription sub;
        sub.subscriberId = subId;
        sub.handler = std::move(handler);
        sub.priority = priority;
        sub.createdAt = std::chrono::system_clock::now();

        auto& topicSubs = subscriptions_[topicStr];
        topicSubs.push_back(std::move(sub));

        // Sort by priority (descending)
        std::sort(topicSubs.begin(), topicSubs.end(),
                  [](const Subscription& a, const Subscription& b) {
                      return a.priority > b.priority;
                  });

        std::size_t index = topicSubs.size() - 1;

        std::uint64_t id = nextSubscriptionId_++;
        subscriptionIndex_.emplace(id, std::make_pair(topicStr, index));

        LOG4CXX_INFO(logger, "Subscription created: " << id << " for topic '" << topicStr << "'");
        return id;
    }

    auto EventBus::unsubscribe(std::uint64_t subscriptionId) -> void
    {
        LOG4CXX_DEBUG(logger, "Unsubscribing: " << subscriptionId);

        std::unique_lock lock(subscriptionMutex_);

        auto it = subscriptionIndex_.find(subscriptionId);
        if (it == subscriptionIndex_.end())
        {
            LOG4CXX_WARN(logger, "Subscription not found: " << subscriptionId);
            return;
        }

        const auto& [topic, index] = it->second;

        auto topicIt = subscriptions_.find(topic);
        if (topicIt != subscriptions_.end() && index < topicIt->second.size())
        {
            topicIt->second.erase(topicIt->second.begin() + static_cast<std::ptrdiff_t>(index));

            if (topicIt->second.empty())
            {
                subscriptions_.erase(topicIt);
            }
        }

        subscriptionIndex_.erase(it);

        // Update indices for remaining subscriptions on this topic
        for (auto& [id, pair] : subscriptionIndex_)
        {
            if (pair.first == topic && pair.second > index)
            {
                --pair.second;
            }
        }

        LOG4CXX_INFO(logger, "Unsubscribed: " << subscriptionId);
    }

    auto EventBus::unsubscribeAll(std::string_view eventType) -> void
    {
        const std::string topicStr(eventType);
        LOG4CXX_DEBUG(logger, "Unsubscribing all for topic: " << topicStr);

        std::unique_lock lock(subscriptionMutex_);

        subscriptions_.erase(topicStr);

        // Remove entries from subscription index
        for (auto it = subscriptionIndex_.begin(); it != subscriptionIndex_.end();)
        {
            if (it->second.first == topicStr)
            {
                it = subscriptionIndex_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        LOG4CXX_INFO(logger, "Unsubscribed all for topic: " << topicStr);
    }

    auto EventBus::unsubscribePlugin(std::string_view pluginName) -> void
    {
        const std::string subId(pluginName);
        LOG4CXX_DEBUG(logger, "Unsubscribing all for plugin: " << subId);

        std::unique_lock lock(subscriptionMutex_);

        for (auto& [topic, subs] : subscriptions_)
        {
            subs.erase(
                std::remove_if(subs.begin(), subs.end(),
                              [&subId](const Subscription& s) {
                                  return s.subscriberId == subId;
                              }),
                subs.end());
        }

        // Clean up empty topics
        for (auto it = subscriptions_.begin(); it != subscriptions_.end();)
        {
            if (it->second.empty())
            {
                it = subscriptions_.erase(it);
            }
            else
            {
                ++it;
            }
        }

        LOG4CXX_INFO(logger, "Unsubscribed plugin: " << subId);
    }

    auto EventBus::request(std::string_view targetPlugin,
                           std::string_view method,
                           const std::any& params,
                           std::chrono::milliseconds timeout) -> std::optional<std::any>
    {
        const std::string targetStr(targetPlugin);
        const std::string methodStr(method);
        LOG4CXX_DEBUG(logger, "Sending request to: " << targetStr
                     << "::" << methodStr << " with timeout " << timeout.count() << "ms");

        ++totalRequestsSent_;

        std::shared_lock lock(handlerMutex_);

        std::string key = targetStr + "::" + methodStr;
        auto it = requestHandlers_.find(key);
        if (it == requestHandlers_.end() || it->second.empty())
        {
            LOG4CXX_WARN(logger, "No handler for request: " << key);
            return std::nullopt;
        }

        const auto& handler = it->second.front();
        lock.unlock();

        try
        {
            auto result = handler.handler(params);
            ++totalRequestsHandled_;
            return result;
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Request handler threw exception: " << e.what());
            return std::nullopt;
        }
    }

    auto EventBus::requestAsync(std::string_view targetPlugin,
                                std::string_view method,
                                const std::any& params) -> std::future<std::optional<std::any>>
    {
        return std::async(std::launch::async, [this, target = std::string(targetPlugin),
                                                meth = std::string(method), params]() {
            return request(target, meth, params);
        });
    }

    auto EventBus::registerHandler(std::string_view method,
                                   RequestHandler handler) -> bool
    {
        const std::string methodStr(method);

        LOG4CXX_DEBUG(logger, "Registering request handler for '" << methodStr << "'");

        std::unique_lock lock(handlerMutex_);

        RequestHandlerEntry entry;
        entry.handlerId = methodStr;
        entry.handler = std::move(handler);

        requestHandlers_[methodStr].push_back(std::move(entry));

        LOG4CXX_INFO(logger, "Request handler registered for: " << methodStr);
        return true;
    }

    auto EventBus::unregisterHandler(std::string_view method) -> void
    {
        const std::string methodStr(method);

        LOG4CXX_DEBUG(logger, "Unregistering request handler for '" << methodStr << "'");

        std::unique_lock lock(handlerMutex_);
        requestHandlers_.erase(methodStr);

        LOG4CXX_INFO(logger, "Request handler unregistered for: " << methodStr);
    }

    auto EventBus::broadcast(std::string_view method,
                             const std::any& params,
                             std::chrono::milliseconds timeout)
        -> std::vector<std::pair<std::string, std::any>>
    {
        std::vector<std::pair<std::string, std::any>> results;
        // Simplified implementation - in a full implementation this would
        // iterate over all registered plugins
        LOG4CXX_DEBUG(logger, "Broadcasting method: " << method);
        return results;
    }

    auto EventBus::isReachable(std::string_view pluginName) const noexcept -> bool
    {
        // Simplified - all plugins are reachable in this local implementation
        return true;
    }

    auto EventBus::getSubscriberCount(std::string_view eventType) const noexcept -> std::size_t
    {
        std::shared_lock lock(subscriptionMutex_);

        auto it = subscriptions_.find(std::string(eventType));
        if (it == subscriptions_.end())
        {
            return 0;
        }

        return it->second.size();
    }

    auto EventBus::getSubscribedEventTypes() const -> std::vector<std::string>
    {
        return getTopics();
    }

    auto EventBus::hasSubscribers(std::string_view topic) const noexcept -> bool
    {
        return getSubscriberCount(topic) > 0;
    }

    auto EventBus::getTopics() const -> std::vector<std::string>
    {
        std::shared_lock lock(subscriptionMutex_);

        std::vector<std::string> topics;
        topics.reserve(subscriptions_.size());

        for (const auto& [topic, subs] : subscriptions_)
        {
            topics.push_back(topic);
        }

        return topics;
    }

    auto EventBus::enableTopic(std::string_view topic) -> void
    {
        std::unique_lock lock(topicMutex_);
        disabledTopics_.erase(std::string(topic));
        LOG4CXX_DEBUG(logger, "Topic enabled: " << topic);
    }

    auto EventBus::disableTopic(std::string_view topic) -> void
    {
        std::unique_lock lock(topicMutex_);
        disabledTopics_.insert(std::string(topic));
        LOG4CXX_DEBUG(logger, "Topic disabled: " << topic);
    }

    auto EventBus::isTopicEnabled(std::string_view topic) const noexcept -> bool
    {
        std::shared_lock lock(topicMutex_);
        return disabledTopics_.find(std::string(topic)) == disabledTopics_.end();
    }

    // ============================================
    // Extended Methods
    // ============================================

    auto EventBus::publish(std::string_view topic,
                           const PluginEvent& event,
                           EventDeliveryMode mode) -> void
    {
        const std::string topicStr(topic);

        LOG4CXX_DEBUG(logger, "Publishing event '" << event.eventType
                     << "' to topic '" << topicStr << "'");

        ++totalEventsPublished_;

        // Check if topic is enabled
        if (!isTopicEnabled(topic))
        {
            LOG4CXX_DEBUG(logger, "Topic disabled, skipping: " << topicStr);
            return;
        }

        switch (mode)
        {
            case EventDeliveryMode::Synchronous:
            case EventDeliveryMode::Immediate:
                deliverEvent(topicStr, event);
                break;

            case EventDeliveryMode::Asynchronous:
            {
                std::lock_guard lock(queueMutex_);
                if (asyncQueue_.size() >= config_.maxQueueSize)
                {
                    LOG4CXX_WARN(logger, "Async queue full, dropping event");
                    addToDeadLetter(event, "Queue full");
                    return;
                }
                asyncQueue_.push({topicStr, event});
                queueCondition_.notify_one();
                break;
            }
        }
    }

    auto EventBus::getDeadLetters() const -> std::vector<PluginEvent>
    {
        std::lock_guard lock(deadLetterMutex_);

        std::vector<PluginEvent> result;
        result.reserve(deadLetterQueue_.size());

        for (const auto& [event, reason] : deadLetterQueue_)
        {
            result.push_back(event);
        }

        return result;
    }

    auto EventBus::clearDeadLetters() -> void
    {
        std::lock_guard lock(deadLetterMutex_);
        deadLetterQueue_.clear();
        LOG4CXX_DEBUG(logger, "Dead letter queue cleared");
    }

    auto EventBus::startAsyncProcessing() -> void
    {
        if (processingThread_)
        {
            LOG4CXX_WARN(logger, "Async processing already running");
            return;
        }

        stopProcessing_ = false;
        processingThread_ = std::make_unique<std::thread>(&EventBus::processAsyncQueue, this);
        LOG4CXX_INFO(logger, "Async event processing started");
    }

    auto EventBus::stopAsyncProcessing() -> void
    {
        if (!processingThread_)
        {
            return;
        }

        stopProcessing_ = true;
        queueCondition_.notify_all();

        if (processingThread_->joinable())
        {
            processingThread_->join();
        }

        processingThread_.reset();
        LOG4CXX_INFO(logger, "Async event processing stopped");
    }

    auto EventBus::getConfig() const noexcept -> const Config&
    {
        return config_;
    }

    auto EventBus::getStatistics() const noexcept -> Statistics
    {
        Statistics stats;
        stats.totalEventsPublished = totalEventsPublished_.load();
        stats.totalEventsDelivered = totalEventsDelivered_.load();
        stats.totalEventsFailed = totalEventsFailed_.load();
        stats.totalRequestsSent = totalRequestsSent_.load();
        stats.totalRequestsHandled = totalRequestsHandled_.load();

        {
            std::lock_guard lock(deadLetterMutex_);
            stats.deadLetterCount = deadLetterQueue_.size();
        }

        return stats;
    }

    // ============================================
    // Internal Methods
    // ============================================

    auto EventBus::deliverEvent(const std::string& topic, const PluginEvent& event) -> void
    {
        std::shared_lock lock(subscriptionMutex_);

        auto it = subscriptions_.find(topic);
        if (it == subscriptions_.end())
        {
            LOG4CXX_DEBUG(logger, "No subscribers for topic: " << topic);
            return;
        }

        // Copy handlers to avoid holding lock during delivery
        std::vector<Subscription> handlers = it->second;
        lock.unlock();

        bool anyDelivered = false;

        for (const auto& sub : handlers)
        {
            try
            {
                sub.handler(event);
                ++totalEventsDelivered_;
                anyDelivered = true;
            }
            catch (const std::exception& e)
            {
                ++totalEventsFailed_;
                LOG4CXX_WARN(logger, "Event handler '" << sub.subscriberId
                            << "' threw exception: " << e.what());
            }
        }

        if (!anyDelivered && config_.enableDeadLetterQueue)
        {
            addToDeadLetter(event, "No successful deliveries");
        }
    }

    auto EventBus::processAsyncQueue() -> void
    {
        LOG4CXX_TRACE(logger, "Async queue processing thread started");

        while (!stopProcessing_)
        {
            QueuedEvent queuedEvent;

            {
                std::unique_lock lock(queueMutex_);
                queueCondition_.wait(lock, [this] {
                    return stopProcessing_ || !asyncQueue_.empty();
                });

                if (stopProcessing_ && asyncQueue_.empty())
                {
                    break;
                }

                if (!asyncQueue_.empty())
                {
                    queuedEvent = std::move(asyncQueue_.front());
                    asyncQueue_.pop();
                }
            }

            if (!queuedEvent.topic.empty())
            {
                deliverEvent(queuedEvent.topic, queuedEvent.event);
            }
        }

        LOG4CXX_TRACE(logger, "Async queue processing thread stopped");
    }

    auto EventBus::addToDeadLetter(const PluginEvent& event, const std::string& reason) -> void
    {
        if (!config_.enableDeadLetterQueue)
        {
            return;
        }

        std::lock_guard lock(deadLetterMutex_);
        deadLetterQueue_.emplace_back(event, reason);

        LOG4CXX_DEBUG(logger, "Event added to dead letter queue: " << reason);
    }

} // namespace com::github::doevelopper::atlassians::plugin::communication
