/**
 * @file DependencyResolver.hpp
 * @brief Plugin dependency resolution implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_RESOLVER_DEPENDENCYRESOLVER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_RESOLVER_DEPENDENCYRESOLVER_HPP

#include <map>
#include <memory>
#include <set>
#include <shared_mutex>
#include <string>
#include <tuple>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginDependencyResolver.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginRegistry.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::resolver
{
    /**
     * @brief Concrete dependency resolver implementation.
     *
     * Implements topological sorting for dependency resolution with:
     * - Circular dependency detection
     * - Version compatibility checking
     * - Optional dependency handling
     * - Load order determination
     */
    class DependencyResolver : public IPluginDependencyResolver
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Configuration for the dependency resolver.
         */
        struct Config
        {
            bool strictVersionChecking{true};
            bool allowOptionalMissing{true};
            bool enableCaching{true};
        };

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Default constructor.
         */
        DependencyResolver();

        /**
         * @brief Construct with configuration.
         * @param config Configuration settings
         */
        explicit DependencyResolver(const Config& config);

        /**
         * @brief Destructor.
         */
        ~DependencyResolver() noexcept override;

        // Delete copy operations
        DependencyResolver(const DependencyResolver&) = delete;
        auto operator=(const DependencyResolver&) -> DependencyResolver& = delete;

        // ============================================
        // IPluginDependencyResolver Implementation
        // ============================================

        [[nodiscard]] auto resolve(const std::vector<const IPluginMetadata*>& plugins)
            -> ResolutionResult override;

        [[nodiscard]] auto resolveFor(const IPluginMetadata& plugin,
                                      const std::vector<const IPluginMetadata*>& availablePlugins)
            -> ResolutionResult override;

        [[nodiscard]] auto getDependencyChain(std::string_view pluginName,
                                              const std::vector<const IPluginMetadata*>& availablePlugins) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto getDependents(std::string_view pluginName,
                                         const std::vector<const IPluginMetadata*>& allPlugins) const
            -> std::vector<std::string> override;

        [[nodiscard]] auto isVersionCompatible(std::string_view version,
                                               std::string_view constraint) const
            -> bool override;

        [[nodiscard]] auto parseVersion(std::string_view version) const
            -> std::tuple<int, int, int, std::string, std::string> override;

        [[nodiscard]] auto compareVersions(std::string_view version1,
                                           std::string_view version2) const
            -> int override;

        [[nodiscard]] auto detectCircularDependencies(
            const std::vector<const IPluginMetadata*>& plugins) const
            -> std::vector<std::vector<std::string>> override;

        [[nodiscard]] auto wouldCreateCycle(std::string_view fromPlugin,
                                            std::string_view toPlugin,
                                            const std::vector<const IPluginMetadata*>& allPlugins) const
            -> bool override;

        [[nodiscard]] auto validateDependencies(
            const std::vector<const IPluginMetadata*>& plugins) const
            -> ResolutionResult override;

        [[nodiscard]] auto getMissingOptionalDependencies(
            const IPluginMetadata& plugin,
            const std::vector<const IPluginMetadata*>& availablePlugins) const
            -> std::vector<std::string> override;

        // ============================================
        // Additional Methods
        // ============================================

        /**
         * @brief Clear the resolution cache.
         */
        auto clearCache() -> void;

        /**
         * @brief Get current configuration.
         * @return Configuration
         */
        [[nodiscard]] auto getConfig() const noexcept -> const Config&;

        /**
         * @brief Set configuration.
         * @param config New configuration
         */
        auto setConfig(const Config& config) -> void;

    private:
        // ============================================
        // Internal Types
        // ============================================

        enum class NodeColor { White, Gray, Black };

        // ============================================
        // Internal Methods
        // ============================================

        auto topologicalSort(const std::vector<const IPluginMetadata*>& plugins) const
            -> std::vector<std::string>;

        auto detectCycleHelper(const std::string& node,
                               const std::map<std::string, std::vector<std::string>>& graph,
                               std::map<std::string, NodeColor>& colors,
                               std::vector<std::string>& path,
                               std::vector<std::vector<std::string>>& cycles) const -> bool;

        // ============================================
        // Member Variables
        // ============================================

        Config config_;

        // Cache for resolution results
        mutable std::map<std::string, ResolutionResult> resolutionCache_;
        mutable std::shared_mutex cacheMutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::resolver

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_RESOLVER_DEPENDENCYRESOLVER_HPP
