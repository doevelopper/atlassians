/**
 * @file PluginRegistryTest.cpp
 * @brief Unit tests for PluginRegistry
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/premisses/plugin/registry/PluginRegistry.hpp>
#include <com/github/doevelopper/premisses/plugin/metadata/PluginMetadata.hpp>
#include <com/github/doevelopper/premisses/plugin/core/PluginExceptions.hpp>

#include <thread>
#include <vector>

namespace com::github::doevelopper::premisses::plugin::test
{
    using namespace testing;
    using namespace registry;
    using namespace metadata;

    /**
     * @brief Mock plugin for testing.
     */
    class MockPlugin : public IPlugin
    {
    public:
        MockPlugin()
        {
            // Set up default behaviors
            ON_CALL(*this, getState()).WillByDefault(Return(PluginState::Loaded));
        }

        MOCK_METHOD(void, initialize, (IPluginContext&), (override));
        MOCK_METHOD(void, start, (), (override));
        MOCK_METHOD(void, stop, (), (override));
        MOCK_METHOD(void, shutdown, (), (override));
        MOCK_METHOD(const IPluginMetadata&, getMetadata, (), (const, noexcept, override));
        MOCK_METHOD(PluginState, getState, (), (const, noexcept, override));
        MOCK_METHOD(bool, hasCapability, (std::string_view), (const, override));
        MOCK_METHOD(std::vector<std::string>, getProvidedServices, (), (const, override));
        MOCK_METHOD(std::vector<std::string>, getRequiredServices, (), (const, override));
    };

    /**
     * @brief Test fixture for PluginRegistry tests.
     */
    class PluginRegistryTest : public Test
    {
    protected:
        void SetUp() override
        {
            registry_ = std::make_unique<PluginRegistry>();
        }

        void TearDown() override
        {
            registry_.reset();
        }

        auto createTestMetadata(const std::string& name) -> std::unique_ptr<PluginMetadata>
        {
            auto metadata = std::make_unique<PluginMetadata>();
            metadata->setName(name);
            metadata->setVersion("1.0.0");
            metadata->setDescription("Test plugin");
            return metadata;
        }

        std::unique_ptr<PluginRegistry> registry_;
    };

    // ============================================
    // Registration Tests
    // ============================================

    TEST_F(PluginRegistryTest, RegisterPlugin_WithValidMetadata_Succeeds)
    {
        auto metadata = createTestMetadata("TestPlugin");

        EXPECT_NO_THROW(registry_->registerPlugin(std::move(metadata)));
        EXPECT_TRUE(registry_->isRegistered("TestPlugin"));
    }

    TEST_F(PluginRegistryTest, RegisterPlugin_WithNullMetadata_Throws)
    {
        EXPECT_THROW(registry_->registerPlugin(nullptr), PluginException);
    }

    TEST_F(PluginRegistryTest, RegisterPlugin_Duplicate_Throws)
    {
        auto metadata1 = createTestMetadata("TestPlugin");
        auto metadata2 = createTestMetadata("TestPlugin");

        registry_->registerPlugin(std::move(metadata1));

        EXPECT_THROW(registry_->registerPlugin(std::move(metadata2)), PluginException);
    }

    TEST_F(PluginRegistryTest, UnregisterPlugin_Existing_Succeeds)
    {
        auto metadata = createTestMetadata("TestPlugin");
        registry_->registerPlugin(std::move(metadata));

        EXPECT_NO_THROW(registry_->unregisterPlugin("TestPlugin"));
        EXPECT_FALSE(registry_->isRegistered("TestPlugin"));
    }

    TEST_F(PluginRegistryTest, UnregisterPlugin_NonExistent_Throws)
    {
        EXPECT_THROW(registry_->unregisterPlugin("NonExistent"), PluginNotFoundException);
    }

    // ============================================
    // Query Tests
    // ============================================

    TEST_F(PluginRegistryTest, GetPlugin_AfterUpdate_ReturnsPlugin)
    {
        auto metadata = createTestMetadata("TestPlugin");
        registry_->registerPlugin(std::move(metadata));

        auto plugin = std::make_shared<MockPlugin>();
        registry_->updatePlugin("TestPlugin", plugin);

        auto retrieved = registry_->getPlugin("TestPlugin");
        EXPECT_EQ(retrieved, plugin);
    }

    TEST_F(PluginRegistryTest, GetPlugin_NonExistent_ReturnsNull)
    {
        EXPECT_EQ(registry_->getPlugin("NonExistent"), nullptr);
    }

    TEST_F(PluginRegistryTest, GetMetadata_Existing_ReturnsMetadata)
    {
        auto metadata = createTestMetadata("TestPlugin");
        registry_->registerPlugin(std::move(metadata));

        const auto* retrieved = registry_->getMetadata("TestPlugin");
        ASSERT_NE(retrieved, nullptr);
        EXPECT_EQ(retrieved->getName(), "TestPlugin");
    }

    TEST_F(PluginRegistryTest, GetAllMetadata_MultiplePlugins_ReturnsAll)
    {
        registry_->registerPlugin(createTestMetadata("Plugin1"));
        registry_->registerPlugin(createTestMetadata("Plugin2"));
        registry_->registerPlugin(createTestMetadata("Plugin3"));

        auto allMetadata = registry_->getAllMetadata();
        EXPECT_EQ(allMetadata.size(), 3);
    }

    // ============================================
    // State Tests
    // ============================================

    TEST_F(PluginRegistryTest, GetPluginState_NewPlugin_ReturnsUnloaded)
    {
        auto metadata = createTestMetadata("TestPlugin");
        registry_->registerPlugin(std::move(metadata));

        EXPECT_EQ(registry_->getPluginState("TestPlugin"), PluginState::Unloaded);
    }

    TEST_F(PluginRegistryTest, SetPluginState_ValidPlugin_UpdatesState)
    {
        auto metadata = createTestMetadata("TestPlugin");
        registry_->registerPlugin(std::move(metadata));

        registry_->setPluginState("TestPlugin", PluginState::Started);

        EXPECT_EQ(registry_->getPluginState("TestPlugin"), PluginState::Started);
    }

    TEST_F(PluginRegistryTest, GetPluginsByState_ReturnsCorrectPlugins)
    {
        registry_->registerPlugin(createTestMetadata("Plugin1"));
        registry_->registerPlugin(createTestMetadata("Plugin2"));
        registry_->registerPlugin(createTestMetadata("Plugin3"));

        registry_->setPluginState("Plugin1", PluginState::Started);
        registry_->setPluginState("Plugin2", PluginState::Started);

        auto started = registry_->getPluginsByState(PluginState::Started);
        EXPECT_EQ(started.size(), 2);
        EXPECT_THAT(started, UnorderedElementsAre("Plugin1", "Plugin2"));
    }

    // ============================================
    // Capability Index Tests
    // ============================================

    TEST_F(PluginRegistryTest, FindByCapability_WithMatches_ReturnsPlugins)
    {
        auto metadata1 = createTestMetadata("Plugin1");
        metadata1->addCapability("math");
        metadata1->addCapability("graphics");

        auto metadata2 = createTestMetadata("Plugin2");
        metadata2->addCapability("math");

        registry_->registerPlugin(std::move(metadata1));
        registry_->registerPlugin(std::move(metadata2));

        auto mathPlugins = registry_->findByCapability("math");
        EXPECT_EQ(mathPlugins.size(), 2);

        auto graphicsPlugins = registry_->findByCapability("graphics");
        EXPECT_EQ(graphicsPlugins.size(), 1);
    }

    TEST_F(PluginRegistryTest, FindByService_WithMatches_ReturnsPlugins)
    {
        auto metadata = createTestMetadata("Plugin1");
        metadata->addProvidedService("ICalculator");

        registry_->registerPlugin(std::move(metadata));

        auto providers = registry_->findByService("ICalculator");
        EXPECT_EQ(providers.size(), 1);
        EXPECT_EQ(providers[0], "Plugin1");
    }

    // ============================================
    // Count Tests
    // ============================================

    TEST_F(PluginRegistryTest, GetPluginCount_ReturnsCorrectCount)
    {
        EXPECT_EQ(registry_->getPluginCount(), 0);

        registry_->registerPlugin(createTestMetadata("Plugin1"));
        EXPECT_EQ(registry_->getPluginCount(), 1);

        registry_->registerPlugin(createTestMetadata("Plugin2"));
        EXPECT_EQ(registry_->getPluginCount(), 2);

        registry_->unregisterPlugin("Plugin1");
        EXPECT_EQ(registry_->getPluginCount(), 1);
    }

    TEST_F(PluginRegistryTest, GetLoadedPluginCount_ReturnsCorrectCount)
    {
        registry_->registerPlugin(createTestMetadata("Plugin1"));
        registry_->registerPlugin(createTestMetadata("Plugin2"));
        registry_->registerPlugin(createTestMetadata("Plugin3"));

        registry_->setPluginState("Plugin1", PluginState::Started);
        registry_->setPluginState("Plugin2", PluginState::Loaded);

        EXPECT_EQ(registry_->getLoadedPluginCount(), 2);
    }

    // ============================================
    // Callback Tests
    // ============================================

    TEST_F(PluginRegistryTest, OnRegistryChange_CallbackIsCalled)
    {
        bool called = false;
        std::string changedPlugin;
        PluginState changedState;

        registry_->onRegistryChange([&](std::string_view name, PluginState state) {
            called = true;
            changedPlugin = std::string(name);
            changedState = state;
        });

        registry_->registerPlugin(createTestMetadata("TestPlugin"));

        EXPECT_TRUE(called);
        EXPECT_EQ(changedPlugin, "TestPlugin");
        EXPECT_EQ(changedState, PluginState::Unloaded);
    }

    TEST_F(PluginRegistryTest, RemoveRegistryChangeCallback_NoLongerCalled)
    {
        int callCount = 0;

        auto handle = registry_->onRegistryChange([&](std::string_view, PluginState) {
            ++callCount;
        });

        registry_->registerPlugin(createTestMetadata("Plugin1"));
        EXPECT_EQ(callCount, 1);

        registry_->removeRegistryChangeCallback(handle);

        registry_->registerPlugin(createTestMetadata("Plugin2"));
        EXPECT_EQ(callCount, 1); // Should not increase
    }

    // ============================================
    // Plugin Visitor Tests
    // ============================================

    TEST_F(PluginRegistryTest, ForEachPlugin_VisitsAllPlugins)
    {
        registry_->registerPlugin(createTestMetadata("Plugin1"));
        registry_->registerPlugin(createTestMetadata("Plugin2"));
        registry_->registerPlugin(createTestMetadata("Plugin3"));

        std::vector<std::string> visitedPlugins;

        registry_->forEachPlugin([&](const std::string& name, 
                                     const IPluginMetadata&, 
                                     std::shared_ptr<IPlugin>) {
            visitedPlugins.push_back(name);
        });

        EXPECT_EQ(visitedPlugins.size(), 3);
        EXPECT_THAT(visitedPlugins, UnorderedElementsAre("Plugin1", "Plugin2", "Plugin3"));
    }

    // ============================================
    // Thread Safety Tests
    // ============================================

    TEST_F(PluginRegistryTest, ConcurrentAccess_IsThreadSafe)
    {
        const int numThreads = 10;
        const int numOperations = 100;

        // Pre-register some plugins
        for (int i = 0; i < 50; ++i)
        {
            registry_->registerPlugin(createTestMetadata("Plugin" + std::to_string(i)));
        }

        std::vector<std::thread> threads;

        // Readers
        for (int t = 0; t < numThreads / 2; ++t)
        {
            threads.emplace_back([&]() {
                for (int i = 0; i < numOperations; ++i)
                {
                    registry_->getPluginCount();
                    registry_->getAllPluginNames();
                    registry_->isRegistered("Plugin0");
                }
            });
        }

        // Writers
        for (int t = 0; t < numThreads / 2; ++t)
        {
            threads.emplace_back([&, t]() {
                for (int i = 0; i < numOperations / 10; ++i)
                {
                    std::string name = "NewPlugin" + std::to_string(t) + "_" + std::to_string(i);
                    try
                    {
                        registry_->registerPlugin(createTestMetadata(name));
                    }
                    catch (...)
                    {
                        // Ignore duplicates
                    }
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        // Verify registry is still consistent
        EXPECT_GE(registry_->getPluginCount(), 50);
    }

    // ============================================
    // Additional Tests
    // ============================================

    TEST_F(PluginRegistryTest, GetAllPluginNames_ReturnsCorrectNames)
    {
        registry_->registerPlugin(createTestMetadata("Alpha"));
        registry_->registerPlugin(createTestMetadata("Beta"));
        registry_->registerPlugin(createTestMetadata("Gamma"));

        auto names = registry_->getAllPluginNames();

        EXPECT_EQ(names.size(), 3);
        EXPECT_THAT(names, UnorderedElementsAre("Alpha", "Beta", "Gamma"));
    }

    TEST_F(PluginRegistryTest, UpdatePlugin_NonExistent_Throws)
    {
        auto plugin = std::make_shared<MockPlugin>();

        EXPECT_THROW(registry_->updatePlugin("NonExistent", plugin), PluginNotFoundException);
    }

    TEST_F(PluginRegistryTest, FindByRequiredService_ReturnsCorrectPlugins)
    {
        auto metadata = createTestMetadata("Plugin1");
        metadata->addRequiredService("ILogger");

        registry_->registerPlugin(std::move(metadata));

        auto requirers = registry_->findByRequiredService("ILogger");
        EXPECT_EQ(requirers.size(), 1);
        EXPECT_EQ(requirers[0], "Plugin1");
    }

    TEST_F(PluginRegistryTest, IsRegistered_ReturnsFalseForNonExistent)
    {
        EXPECT_FALSE(registry_->isRegistered("NonExistent"));
    }

    TEST_F(PluginRegistryTest, GetMetadata_NonExistent_ReturnsNull)
    {
        EXPECT_EQ(registry_->getMetadata("NonExistent"), nullptr);
    }

} // namespace com::github::doevelopper::premisses::plugin::test
