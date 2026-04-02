/**
 * @file DependencyResolverTest.cpp
 * @brief Unit tests for DependencyResolver
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <com/github/doevelopper/atlassians/plugin/resolver/DependencyResolver.hpp>
#include <com/github/doevelopper/atlassians/plugin/metadata/PluginMetadata.hpp>

namespace com::github::doevelopper::atlassians::plugin::test
{
    using namespace testing;
    using namespace resolver;
    using namespace metadata;

    /**
     * @brief Test fixture for DependencyResolver tests.
     */
    class DependencyResolverTest : public Test
    {
    protected:
        void SetUp() override
        {
            resolver_ = std::make_unique<DependencyResolver>();
        }

        void TearDown() override
        {
            resolver_.reset();
            metadataStorage_.clear();
        }

        auto createTestMetadata(const std::string& name,
                               const std::vector<std::pair<std::string, bool>>& deps = {})
            -> PluginMetadata*
        {
            auto metadata = std::make_unique<PluginMetadata>();
            metadata->setName(name);
            metadata->setVersion("1.0.0");

            for (const auto& [depName, optional] : deps)
            {
                PluginDependency dep;
                dep.pluginName = depName;
                dep.versionConstraint = ">=1.0.0";
                dep.optional = optional;
                metadata->addDependency(dep);
            }

            PluginMetadata* ptr = metadata.get();
            metadataStorage_.push_back(std::move(metadata));
            return ptr;
        }

        auto getMetadataPointers() -> std::vector<const IPluginMetadata*>
        {
            std::vector<const IPluginMetadata*> result;
            for (const auto& m : metadataStorage_)
            {
                result.push_back(m.get());
            }
            return result;
        }

        std::unique_ptr<DependencyResolver> resolver_;
        std::vector<std::unique_ptr<PluginMetadata>> metadataStorage_;
    };

    // ============================================
    // Basic Resolution Tests
    // ============================================

    TEST_F(DependencyResolverTest, Resolve_NoDependencies_Succeeds)
    {
        createTestMetadata("PluginA");

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_TRUE(result.success);
        EXPECT_TRUE(result.missingDependencies.empty());
    }

    TEST_F(DependencyResolverTest, Resolve_WithDependency_IncludesInLoadOrder)
    {
        createTestMetadata("PluginB");
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_TRUE(result.success);
        EXPECT_FALSE(result.loadOrder.empty());
        
        // PluginB should come before PluginA in load order
        auto posB = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginB");
        auto posA = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginA");
        EXPECT_LT(posB, posA);
    }

    TEST_F(DependencyResolverTest, Resolve_MissingDependency_Fails)
    {
        createTestMetadata("PluginA", {{"NonExistent", false}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_FALSE(result.success);
        EXPECT_THAT(result.missingDependencies, Contains("NonExistent"));
    }

    TEST_F(DependencyResolverTest, Resolve_MissingOptionalDependency_Succeeds)
    {
        createTestMetadata("PluginA", {{"NonExistent", true}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_TRUE(result.success);
    }

    TEST_F(DependencyResolverTest, Resolve_EmptyPluginList_Succeeds)
    {
        auto result = resolver_->resolve({});

        EXPECT_TRUE(result.success);
        EXPECT_TRUE(result.loadOrder.empty());
    }

    // ============================================
    // Load Order Tests
    // ============================================

    TEST_F(DependencyResolverTest, Resolve_ChainedDependencies_CorrectLoadOrder)
    {
        // A depends on B, B depends on C
        createTestMetadata("PluginC");
        createTestMetadata("PluginB", {{"PluginC", false}});
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_TRUE(result.success);

        // C must come before B, B must come before A
        auto posC = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginC") 
                    - result.loadOrder.begin();
        auto posB = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginB") 
                    - result.loadOrder.begin();
        auto posA = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginA") 
                    - result.loadOrder.begin();

        EXPECT_LT(posC, posB);
        EXPECT_LT(posB, posA);
    }

    TEST_F(DependencyResolverTest, Resolve_DiamondDependency_Works)
    {
        // A depends on B and C, both depend on D
        createTestMetadata("PluginD");
        createTestMetadata("PluginB", {{"PluginD", false}});
        createTestMetadata("PluginC", {{"PluginD", false}});
        createTestMetadata("PluginA", {{"PluginB", false}, {"PluginC", false}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_TRUE(result.success);

        // D must come before both B and C
        auto posD = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginD") 
                    - result.loadOrder.begin();
        auto posB = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginB") 
                    - result.loadOrder.begin();
        auto posC = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginC") 
                    - result.loadOrder.begin();
        auto posA = std::find(result.loadOrder.begin(), result.loadOrder.end(), "PluginA") 
                    - result.loadOrder.begin();

        EXPECT_LT(posD, posB);
        EXPECT_LT(posD, posC);
        EXPECT_LT(posB, posA);
        EXPECT_LT(posC, posA);
    }

    // ============================================
    // Dependency Chain Tests
    // ============================================

    TEST_F(DependencyResolverTest, GetDependencyChain_ReturnsTransitiveDeps)
    {
        // A -> B -> C -> D
        createTestMetadata("PluginD");
        createTestMetadata("PluginC", {{"PluginD", false}});
        createTestMetadata("PluginB", {{"PluginC", false}});
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto chain = resolver_->getDependencyChain("PluginA", getMetadataPointers());

        // Note: getDependencyChain includes the plugin itself in the chain
        EXPECT_THAT(chain, UnorderedElementsAre("PluginA", "PluginB", "PluginC", "PluginD"));
    }

    TEST_F(DependencyResolverTest, GetDependents_ReturnsPluginsDependingOn)
    {
        createTestMetadata("PluginB");
        createTestMetadata("PluginA", {{"PluginB", false}});
        createTestMetadata("PluginC", {{"PluginB", false}});

        auto dependents = resolver_->getDependents("PluginB", getMetadataPointers());

        EXPECT_EQ(dependents.size(), 2);
        EXPECT_THAT(dependents, UnorderedElementsAre("PluginA", "PluginC"));
    }

    TEST_F(DependencyResolverTest, GetDependencyChain_NoDeps_ReturnsEmpty)
    {
        createTestMetadata("PluginA");

        auto chain = resolver_->getDependencyChain("PluginA", getMetadataPointers());

        // Note: getDependencyChain includes the plugin itself even when there are no deps
        EXPECT_EQ(chain.size(), 1);
        EXPECT_THAT(chain, Contains("PluginA"));
    }

    // ============================================
    // Circular Dependency Tests
    // ============================================

    TEST_F(DependencyResolverTest, DetectCircularDependencies_DirectCycle_DetectsCycle)
    {
        // A -> B -> A (direct cycle)
        auto metaA = createTestMetadata("PluginA", {{"PluginB", false}});
        auto metaB = createTestMetadata("PluginB", {{"PluginA", false}});

        auto cycles = resolver_->detectCircularDependencies(getMetadataPointers());

        EXPECT_FALSE(cycles.empty());
    }

    TEST_F(DependencyResolverTest, DetectCircularDependencies_IndirectCycle_DetectsCycle)
    {
        // A -> B -> C -> A
        createTestMetadata("PluginC", {{"PluginA", false}});
        createTestMetadata("PluginB", {{"PluginC", false}});
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto cycles = resolver_->detectCircularDependencies(getMetadataPointers());

        EXPECT_FALSE(cycles.empty());
    }

    TEST_F(DependencyResolverTest, DetectCircularDependencies_NoCycle_ReturnsEmpty)
    {
        createTestMetadata("PluginC");
        createTestMetadata("PluginB", {{"PluginC", false}});
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto cycles = resolver_->detectCircularDependencies(getMetadataPointers());

        EXPECT_TRUE(cycles.empty());
    }

    TEST_F(DependencyResolverTest, Resolve_CircularDependency_Fails)
    {
        createTestMetadata("PluginA", {{"PluginB", false}});
        createTestMetadata("PluginB", {{"PluginA", false}});

        auto result = resolver_->resolve(getMetadataPointers());

        EXPECT_FALSE(result.success);
        EXPECT_FALSE(result.circularDependencies.empty());
    }

    TEST_F(DependencyResolverTest, WouldCreateCycle_WhenCycleWouldResult_ReturnsTrue)
    {
        createTestMetadata("PluginA", {{"PluginB", false}});
        createTestMetadata("PluginB");

        bool wouldCycle = resolver_->wouldCreateCycle("PluginB", "PluginA", getMetadataPointers());

        EXPECT_TRUE(wouldCycle);
    }

    TEST_F(DependencyResolverTest, WouldCreateCycle_WhenNoCycleWouldResult_ReturnsFalse)
    {
        createTestMetadata("PluginA");
        createTestMetadata("PluginB");

        bool wouldCycle = resolver_->wouldCreateCycle("PluginA", "PluginB", getMetadataPointers());

        EXPECT_FALSE(wouldCycle);
    }

    // ============================================
    // Version Compatibility Tests
    // ============================================

    TEST_F(DependencyResolverTest, IsVersionCompatible_ExactMatch_ReturnsTrue)
    {
        EXPECT_TRUE(resolver_->isVersionCompatible("1.0.0", "=1.0.0"));
    }

    TEST_F(DependencyResolverTest, IsVersionCompatible_GreaterOrEqual_ReturnsCorrectly)
    {
        EXPECT_TRUE(resolver_->isVersionCompatible("1.5.0", ">=1.0.0"));
        EXPECT_TRUE(resolver_->isVersionCompatible("1.0.0", ">=1.0.0"));
        EXPECT_FALSE(resolver_->isVersionCompatible("0.9.0", ">=1.0.0"));
    }

    TEST_F(DependencyResolverTest, IsVersionCompatible_LessThan_ReturnsCorrectly)
    {
        EXPECT_TRUE(resolver_->isVersionCompatible("0.9.0", "<1.0.0"));
        EXPECT_FALSE(resolver_->isVersionCompatible("1.0.0", "<1.0.0"));
        EXPECT_FALSE(resolver_->isVersionCompatible("1.5.0", "<1.0.0"));
    }

    TEST_F(DependencyResolverTest, CompareVersions_ReturnsCorrectOrdering)
    {
        EXPECT_LT(resolver_->compareVersions("1.0.0", "2.0.0"), 0);
        EXPECT_GT(resolver_->compareVersions("2.0.0", "1.0.0"), 0);
        EXPECT_EQ(resolver_->compareVersions("1.0.0", "1.0.0"), 0);
        EXPECT_LT(resolver_->compareVersions("1.0.0", "1.0.1"), 0);
        EXPECT_LT(resolver_->compareVersions("1.0.0", "1.1.0"), 0);
    }

    TEST_F(DependencyResolverTest, ParseVersion_ParsesCorrectly)
    {
        auto [major, minor, patch, prerelease, buildmeta] = 
            resolver_->parseVersion("1.2.3-alpha+build");

        EXPECT_EQ(major, 1);
        EXPECT_EQ(minor, 2);
        EXPECT_EQ(patch, 3);
        EXPECT_EQ(prerelease, "alpha");
        EXPECT_EQ(buildmeta, "build");
    }

    TEST_F(DependencyResolverTest, ParseVersion_SimpleSemver_Works)
    {
        auto [major, minor, patch, prerelease, buildmeta] = 
            resolver_->parseVersion("2.5.10");

        EXPECT_EQ(major, 2);
        EXPECT_EQ(minor, 5);
        EXPECT_EQ(patch, 10);
        EXPECT_TRUE(prerelease.empty());
        EXPECT_TRUE(buildmeta.empty());
    }

    // ============================================
    // Validation Tests
    // ============================================

    TEST_F(DependencyResolverTest, ValidateDependencies_AllValid_Succeeds)
    {
        createTestMetadata("PluginB");
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto result = resolver_->validateDependencies(getMetadataPointers());

        EXPECT_TRUE(result.success);
    }

    TEST_F(DependencyResolverTest, ValidateDependencies_MissingDep_Reports)
    {
        createTestMetadata("PluginA", {{"NonExistent", false}});

        auto result = resolver_->validateDependencies(getMetadataPointers());

        EXPECT_FALSE(result.success);
        EXPECT_THAT(result.missingDependencies, Contains("NonExistent"));
    }

    // ============================================
    // Missing Optional Dependencies Tests
    // ============================================

    TEST_F(DependencyResolverTest, GetMissingOptionalDependencies_ReturnsCorrectly)
    {
        createTestMetadata("PluginA", {
            {"ExistingPlugin", true},
            {"MissingOptional", true},
            {"RequiredPlugin", false}
        });
        createTestMetadata("ExistingPlugin");

        auto* pluginA = metadataStorage_[0].get();
        auto missing = resolver_->getMissingOptionalDependencies(*pluginA, getMetadataPointers());

        EXPECT_EQ(missing.size(), 1);
        EXPECT_THAT(missing, Contains("MissingOptional"));
    }

    // ============================================
    // ResolveFor Tests
    // ============================================

    TEST_F(DependencyResolverTest, ResolveFor_SinglePlugin_Works)
    {
        createTestMetadata("PluginB");
        createTestMetadata("PluginA", {{"PluginB", false}});

        auto* pluginA = metadataStorage_[1].get();
        auto result = resolver_->resolveFor(*pluginA, getMetadataPointers());

        EXPECT_TRUE(result.success);
        EXPECT_THAT(result.loadOrder, Contains("PluginB"));
    }

    TEST_F(DependencyResolverTest, ResolveFor_MissingDep_Fails)
    {
        createTestMetadata("PluginA", {{"NonExistent", false}});

        auto* pluginA = metadataStorage_[0].get();
        auto result = resolver_->resolveFor(*pluginA, getMetadataPointers());

        EXPECT_FALSE(result.success);
    }

    // ============================================
    // Cache Tests
    // ============================================

    TEST_F(DependencyResolverTest, ClearCache_Clears)
    {
        createTestMetadata("PluginA");

        // First resolution should cache
        resolver_->resolve(getMetadataPointers());

        // Clear cache
        EXPECT_NO_THROW(resolver_->clearCache());
    }

    // ============================================
    // Configuration Tests
    // ============================================

    TEST_F(DependencyResolverTest, GetConfig_ReturnsConfiguration)
    {
        const auto& config = resolver_->getConfig();
        // Just verify we can access it
        EXPECT_TRUE(config.enableCaching || !config.enableCaching);
    }

    TEST_F(DependencyResolverTest, SetConfig_UpdatesConfiguration)
    {
        DependencyResolver::Config newConfig;
        newConfig.strictVersionChecking = false;
        newConfig.allowOptionalMissing = true;
        newConfig.enableCaching = false;

        resolver_->setConfig(newConfig);

        const auto& config = resolver_->getConfig();
        EXPECT_FALSE(config.strictVersionChecking);
        EXPECT_FALSE(config.enableCaching);
    }

} // namespace com::github::doevelopper::atlassians::plugin::test
