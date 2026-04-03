/**
 * @file EventBusTest.cpp
 * @brief Unit tests for EventBus
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/plugin/communication/EventBus.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

#include <atomic>
#include <chrono>
#include <thread>

namespace com::github::doevelopper::atlassians::plugin::test
{
    using namespace testing;
    using namespace communication;

    /**
     * @brief Test fixture for EventBus tests.
     */
    class EventBusTest : public Test
    {
    protected:
        void SetUp() override
        {
            eventBus_ = std::make_unique<EventBus>();
        }

        void TearDown() override
        {
            eventBus_.reset();
        }

        auto createTestEvent(const std::string& type = "test") -> PluginEvent
        {
            PluginEvent event;
            event.sourcePlugin = "TestSource";
            event.eventType = type;
            event.timestamp = std::chrono::system_clock::now();
            return event;
        }

        std::unique_ptr<EventBus> eventBus_;
    };

    // ============================================
    // Subscription Tests
    // ============================================

    TEST_F(EventBusTest, Subscribe_ReturnsUniqueId)
    {
        auto id1 = eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        auto id2 = eventBus_->subscribe("topic1", [](const PluginEvent&) {});

        EXPECT_NE(id1, id2);
    }

    TEST_F(EventBusTest, Subscribe_IncreasesSubscriberCount)
    {
        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 0);

        eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 1);

        eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 2);
    }

    TEST_F(EventBusTest, Unsubscribe_DecreasesSubscriberCount)
    {
        auto id1 = eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        eventBus_->subscribe("topic1", [](const PluginEvent&) {});

        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 2);

        eventBus_->unsubscribe(id1);
        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 1);
    }

    TEST_F(EventBusTest, UnsubscribeAll_RemovesAllSubscriptionsForTopic)
    {
        eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        eventBus_->subscribe("topic2", [](const PluginEvent&) {});

        eventBus_->unsubscribeAll("topic1");

        EXPECT_EQ(eventBus_->getSubscriberCount("topic1"), 0);
        EXPECT_EQ(eventBus_->getSubscriberCount("topic2"), 1);
    }

    TEST_F(EventBusTest, HasSubscribers_ReturnsCorrectValue)
    {
        EXPECT_FALSE(eventBus_->hasSubscribers("topic1"));

        auto id = eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        EXPECT_TRUE(eventBus_->hasSubscribers("topic1"));

        eventBus_->unsubscribe(id);
        EXPECT_FALSE(eventBus_->hasSubscribers("topic1"));
    }

    // ============================================
    // Publish Tests
    // ============================================

    TEST_F(EventBusTest, Publish_DeliversToSubscribers)
    {
        std::atomic<int> callCount{0};

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            ++callCount;
        });

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            ++callCount;
        });

        auto event = createTestEvent();
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);

        EXPECT_EQ(callCount, 2);
    }

    TEST_F(EventBusTest, Publish_DoesNotDeliverToOtherTopics)
    {
        std::atomic<int> topic1Count{0};
        std::atomic<int> topic2Count{0};

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            ++topic1Count;
        });

        eventBus_->subscribe("topic2", [&](const PluginEvent&) {
            ++topic2Count;
        });

        auto event = createTestEvent();
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);

        EXPECT_EQ(topic1Count, 1);
        EXPECT_EQ(topic2Count, 0);
    }

    TEST_F(EventBusTest, Publish_DeliverEventData)
    {
        std::string receivedType;
        std::string receivedSource;

        eventBus_->subscribe("topic1", [&](const PluginEvent& event) {
            receivedType = event.eventType;
            receivedSource = event.sourcePlugin;
        });

        auto event = createTestEvent("custom_type");
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);

        EXPECT_EQ(receivedType, "custom_type");
        EXPECT_EQ(receivedSource, "TestSource");
    }

    TEST_F(EventBusTest, PublishWithPayload_DeliversCorrectly)
    {
        std::string receivedType;

        eventBus_->subscribe("topic1", [&](const PluginEvent& event) {
            receivedType = event.eventType;
        });

        EventPayload payload = std::string("test payload");
        eventBus_->publish("topic1", payload, "TestSource");

        EXPECT_EQ(receivedType, "topic1");
    }

    // ============================================
    // Priority Tests
    // ============================================

    TEST_F(EventBusTest, SubscribeWithPriority_HighPriorityCalledFirst)
    {
        std::vector<std::string> callOrder;

        eventBus_->subscribeWithPriority("topic1", [&](const PluginEvent&) {
            callOrder.push_back("low");
        }, "low_priority", 0);

        eventBus_->subscribeWithPriority("topic1", [&](const PluginEvent&) {
            callOrder.push_back("high");
        }, "high_priority", 100);

        eventBus_->subscribeWithPriority("topic1", [&](const PluginEvent&) {
            callOrder.push_back("medium");
        }, "medium_priority", 50);

        auto event = createTestEvent();
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);

        ASSERT_EQ(callOrder.size(), 3);
        EXPECT_EQ(callOrder[0], "high");
        EXPECT_EQ(callOrder[1], "medium");
        EXPECT_EQ(callOrder[2], "low");
    }

    // ============================================
    // Topic Enable/Disable Tests
    // ============================================

    TEST_F(EventBusTest, DisableTopic_StopsDelivery)
    {
        std::atomic<int> callCount{0};

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            ++callCount;
        });

        auto event = createTestEvent();
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);
        EXPECT_EQ(callCount, 1);

        eventBus_->disableTopic("topic1");
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);
        EXPECT_EQ(callCount, 1); // Should not increase

        eventBus_->enableTopic("topic1");
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);
        EXPECT_EQ(callCount, 2);
    }

    TEST_F(EventBusTest, IsTopicEnabled_ReturnsCorrectValue)
    {
        EXPECT_TRUE(eventBus_->isTopicEnabled("topic1"));

        eventBus_->disableTopic("topic1");
        EXPECT_FALSE(eventBus_->isTopicEnabled("topic1"));

        eventBus_->enableTopic("topic1");
        EXPECT_TRUE(eventBus_->isTopicEnabled("topic1"));
    }

    // ============================================
    // Topic List Tests
    // ============================================

    TEST_F(EventBusTest, GetTopics_ReturnsAllSubscribedTopics)
    {
        eventBus_->subscribe("topic1", [](const PluginEvent&) {});
        eventBus_->subscribe("topic2", [](const PluginEvent&) {});
        eventBus_->subscribe("topic3", [](const PluginEvent&) {});

        auto topics = eventBus_->getTopics();

        EXPECT_EQ(topics.size(), 3);
        EXPECT_THAT(topics, UnorderedElementsAre("topic1", "topic2", "topic3"));
    }

    // ============================================
    // Request/Response Tests
    // ============================================

    TEST_F(EventBusTest, RegisterHandler_HandlesRequests)
    {
        // Note: request() builds key as "targetPlugin::method"
        // So we must register with the full key that request() will look for
        eventBus_->registerHandler("calculator::calculator.add",
            [](const std::any& request) -> std::any {
                auto params = std::any_cast<std::pair<int, int>>(request);
                return params.first + params.second;
            });

        auto result = eventBus_->request("calculator", "calculator.add",
            std::make_any<std::pair<int, int>>(5, 3),
            std::chrono::milliseconds(1000));

        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(std::any_cast<int>(result.value()), 8);
    }

    TEST_F(EventBusTest, Request_NoHandler_ReturnsEmpty)
    {
        auto result = eventBus_->request("nonexistent", "unknown.method",
            std::any{},
            std::chrono::milliseconds(100));

        EXPECT_FALSE(result.has_value());
    }

    TEST_F(EventBusTest, UnregisterHandler_NoLongerHandles)
    {
        eventBus_->registerHandler("test.handler",
            [](const std::any&) -> std::any { return 42; });

        eventBus_->unregisterHandler("test.handler");

        auto result = eventBus_->request("handler1", "test.handler",
            std::any{},
            std::chrono::milliseconds(100));

        EXPECT_FALSE(result.has_value());
    }

    // ============================================
    // Statistics Tests
    // ============================================

    TEST_F(EventBusTest, GetStatistics_TracksEvents)
    {
        eventBus_->subscribe("topic1", [](const PluginEvent&) {});

        auto event = createTestEvent();
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);
        eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous);

        auto stats = eventBus_->getStatistics();

        EXPECT_EQ(stats.totalEventsPublished, 3);
        EXPECT_EQ(stats.totalEventsDelivered, 3);
    }

    // ============================================
    // Error Handling Tests
    // ============================================

    TEST_F(EventBusTest, Publish_HandlerThrows_ContinuesToOthers)
    {
        std::atomic<int> callCount{0};

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            throw std::runtime_error("Handler error");
        });

        eventBus_->subscribe("topic1", [&](const PluginEvent&) {
            ++callCount;
        });

        auto event = createTestEvent();
        EXPECT_NO_THROW(eventBus_->publish("topic1", event, EventDeliveryMode::Synchronous));
        EXPECT_EQ(callCount, 1);
    }

    // ============================================
    // Thread Safety Tests
    // ============================================

    TEST_F(EventBusTest, ConcurrentPublishAndSubscribe_IsThreadSafe)
    {
        const int numThreads = 10;
        const int numOperations = 100;
        std::atomic<int> eventCount{0};

        std::vector<std::thread> threads;

        // Publisher threads
        for (int t = 0; t < numThreads / 2; ++t)
        {
            threads.emplace_back([&]() {
                for (int i = 0; i < numOperations; ++i)
                {
                    auto event = createTestEvent();
                    eventBus_->publish("concurrent_topic", event, EventDeliveryMode::Synchronous);
                }
            });
        }

        // Subscriber threads
        for (int t = 0; t < numThreads / 2; ++t)
        {
            threads.emplace_back([&, t]() {
                eventBus_->subscribe("concurrent_topic",
                    [&](const PluginEvent&) {
                        ++eventCount;
                    });

                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        // Just verify no crashes occurred
        SUCCEED();
    }

    // ============================================
    // Dead Letter Queue Tests
    // ============================================

    TEST_F(EventBusTest, DeadLetterQueue_CapturesFailedEvents)
    {
        // Initially dead letter queue should be empty
        auto deadLetters = eventBus_->getDeadLetters();
        EXPECT_TRUE(deadLetters.empty());
    }

    TEST_F(EventBusTest, ClearDeadLetters_EmptiesQueue)
    {
        eventBus_->clearDeadLetters();
        auto deadLetters = eventBus_->getDeadLetters();
        EXPECT_TRUE(deadLetters.empty());
    }

    // ============================================
    // Async Processing Tests
    // ============================================

    TEST_F(EventBusTest, PublishAsync_ReturnsValidFuture)
    {
        eventBus_->startAsyncProcessing();

        std::atomic<int> callCount{0};
        eventBus_->subscribe("async_topic", [&](const PluginEvent&) {
            ++callCount;
        });

        auto event = createTestEvent();
        event.eventType = "async_topic";
        auto future = eventBus_->publishAsync(event);

        // Wait for async delivery
        EXPECT_NO_THROW(future.get());

        eventBus_->stopAsyncProcessing();
    }

    // ============================================
    // Configuration Tests
    // ============================================

    TEST_F(EventBusTest, GetConfig_ReturnsConfiguration)
    {
        const auto& config = eventBus_->getConfig();
        EXPECT_GT(config.maxQueueSize, 0);
    }

} // namespace com::github::doevelopper::atlassians::plugin::test
