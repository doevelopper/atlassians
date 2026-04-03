/**
 * @file IPluginDependencyResolver.hpp
 * @brief Interface for plugin dependency resolution
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINDEPENDENCYRESOLVER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINDEPENDENCYRESOLVER_HPP

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for resolving plugin dependencies.
     *
     * The dependency resolver is responsible for:
     * - Determining the load order of plugins
     * - Detecting missing dependencies
     * - Detecting circular dependencies
     * - Checking version compatibility
     */
    class IPluginDependencyResolver
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginDependencyResolver() noexcept = default;

        // ============================================
        // Dependency Resolution
        // ============================================

        /**
         * @brief Resolve dependencies for a set of plugins.
         * @param plugins Vector of plugin metadata.
         * @return Resolution result with load order and any issues.
         */
        [[nodiscard]] virtual auto resolve(const std::vector<const IPluginMetadata*>& plugins)
            -> ResolutionResult = 0;

        /**
         * @brief Resolve dependencies for a single plugin.
         * @param plugin The plugin to resolve.
         * @param availablePlugins All available plugins.
         * @return Resolution result.
         */
        [[nodiscard]] virtual auto resolveFor(const IPluginMetadata& plugin,
                                               const std::vector<const IPluginMetadata*>& availablePlugins)
            -> ResolutionResult = 0;

        /**
         * @brief Get the dependency chain for a plugin.
         * @param pluginName Name of the plugin.
         * @param availablePlugins All available plugins.
         * @return Ordered list of dependencies (dependencies first, plugin last).
         */
        [[nodiscard]] virtual auto getDependencyChain(std::string_view pluginName,
                                                       const std::vector<const IPluginMetadata*>& availablePlugins) const
            -> std::vector<std::string> = 0;

        /**
         * @brief Get plugins that depend on the specified plugin.
         * @param pluginName Name of the plugin.
         * @param allPlugins All plugins to check.
         * @return Names of plugins that depend on the specified plugin.
         */
        [[nodiscard]] virtual auto getDependents(std::string_view pluginName,
                                                  const std::vector<const IPluginMetadata*>& allPlugins) const
            -> std::vector<std::string> = 0;

        // ============================================
        // Version Compatibility
        // ============================================

        /**
         * @brief Check if a version satisfies a constraint.
         * @param version The version to check (e.g., "1.2.3").
         * @param constraint The version constraint (e.g., ">=1.0.0, <2.0.0").
         * @return True if the version satisfies the constraint.
         */
        [[nodiscard]] virtual auto isVersionCompatible(std::string_view version,
                                                        std::string_view constraint) const
            -> bool = 0;

        /**
         * @brief Parse a semantic version string.
         * @param version The version string to parse.
         * @return Tuple of (major, minor, patch, prerelease, build).
         */
        [[nodiscard]] virtual auto parseVersion(std::string_view version) const
            -> std::tuple<int, int, int, std::string, std::string> = 0;

        /**
         * @brief Compare two version strings.
         * @param version1 First version.
         * @param version2 Second version.
         * @return -1 if version1 < version2, 0 if equal, 1 if version1 > version2.
         */
        [[nodiscard]] virtual auto compareVersions(std::string_view version1,
                                                    std::string_view version2) const
            -> int = 0;

        // ============================================
        // Circular Dependency Detection
        // ============================================

        /**
         * @brief Detect circular dependencies.
         * @param plugins Plugins to check.
         * @return Vector of cycles (each cycle is a vector of plugin names).
         */
        [[nodiscard]] virtual auto detectCircularDependencies(
            const std::vector<const IPluginMetadata*>& plugins) const
            -> std::vector<std::vector<std::string>> = 0;

        /**
         * @brief Check if adding a dependency would create a cycle.
         * @param fromPlugin The plugin that would depend on another.
         * @param toPlugin The plugin that would be depended upon.
         * @param allPlugins All available plugins.
         * @return True if adding the dependency would create a cycle.
         */
        [[nodiscard]] virtual auto wouldCreateCycle(std::string_view fromPlugin,
                                                     std::string_view toPlugin,
                                                     const std::vector<const IPluginMetadata*>& allPlugins) const
            -> bool = 0;

        // ============================================
        // Validation
        // ============================================

        /**
         * @brief Validate that all dependencies can be satisfied.
         * @param plugins Available plugins.
         * @return Resolution result with validation details.
         */
        [[nodiscard]] virtual auto validateDependencies(
            const std::vector<const IPluginMetadata*>& plugins) const
            -> ResolutionResult = 0;

        /**
         * @brief Get missing optional dependencies.
         * @param plugin The plugin to check.
         * @param availablePlugins Available plugins.
         * @return Names of missing optional dependencies.
         */
        [[nodiscard]] virtual auto getMissingOptionalDependencies(
            const IPluginMetadata& plugin,
            const std::vector<const IPluginMetadata*>& availablePlugins) const
            -> std::vector<std::string> = 0;

    protected:
        IPluginDependencyResolver() = default;
        IPluginDependencyResolver(const IPluginDependencyResolver&) = default;
        IPluginDependencyResolver(IPluginDependencyResolver&&) = default;
        IPluginDependencyResolver& operator=(const IPluginDependencyResolver&) = default;
        IPluginDependencyResolver& operator=(IPluginDependencyResolver&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINDEPENDENCYRESOLVER_HPP
