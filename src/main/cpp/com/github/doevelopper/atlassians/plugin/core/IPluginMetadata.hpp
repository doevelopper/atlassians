/**
 * @file IPluginMetadata.hpp
 * @brief Interface for plugin metadata access
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINMETADATA_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINMETADATA_HPP

#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for accessing plugin metadata.
     *
     * Plugin metadata provides essential information about a plugin including
     * its name, version, dependencies, capabilities, and configuration.
     */
    class IPluginMetadata
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginMetadata() noexcept = default;

        // ============================================
        // Identity Information
        // ============================================

        /**
         * @brief Get the unique name of the plugin.
         * @return Plugin name as a string view.
         */
        [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the semantic version of the plugin.
         * @return Version string (e.g., "1.2.3").
         */
        [[nodiscard]] virtual auto getVersion() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the required host API version.
         * @return API version string this plugin is compatible with.
         */
        [[nodiscard]] virtual auto getApiVersion() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the plugin description.
         * @return Human-readable description of the plugin.
         */
        [[nodiscard]] virtual auto getDescription() const noexcept -> std::string_view = 0;

        // ============================================
        // Author Information
        // ============================================

        /**
         * @brief Get the plugin author.
         * @return Author name or organization.
         */
        [[nodiscard]] virtual auto getAuthor() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the plugin license.
         * @return License identifier (e.g., "MIT", "BSD-3-Clause").
         */
        [[nodiscard]] virtual auto getLicense() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the plugin homepage URL.
         * @return Optional URL to the plugin's homepage.
         */
        [[nodiscard]] virtual auto getHomepage() const noexcept -> std::optional<std::string_view> = 0;

        // ============================================
        // Services and Capabilities
        // ============================================

        /**
         * @brief Get the list of services this plugin provides.
         * @return Vector of service identifiers.
         */
        [[nodiscard]] virtual auto getProvidedServices() const noexcept
            -> const std::vector<std::string>& = 0;

        /**
         * @brief Get the list of services this plugin requires.
         * @return Vector of required service identifiers.
         */
        [[nodiscard]] virtual auto getRequiredServices() const noexcept
            -> const std::vector<std::string>& = 0;

        /**
         * @brief Get the plugin's capabilities.
         * @return Vector of capability strings.
         */
        [[nodiscard]] virtual auto getCapabilities() const noexcept
            -> const std::vector<std::string>& = 0;

        /**
         * @brief Check if the plugin has a specific capability.
         * @param capability The capability to check for.
         * @return True if the plugin has the capability.
         */
        [[nodiscard]] virtual auto hasCapability(std::string_view capability) const noexcept -> bool = 0;

        // ============================================
        // Dependencies
        // ============================================

        /**
         * @brief Get the list of plugin dependencies.
         * @return Vector of dependency specifications.
         */
        [[nodiscard]] virtual auto getDependencies() const noexcept
            -> const std::vector<PluginDependency>& = 0;

        /**
         * @brief Check if this plugin depends on another.
         * @param pluginName Name of the potential dependency.
         * @return True if there is a dependency on the named plugin.
         */
        [[nodiscard]] virtual auto dependsOn(std::string_view pluginName) const noexcept -> bool = 0;

        // ============================================
        // Plugin Configuration
        // ============================================

        /**
         * @brief Get a custom property value.
         * @param key Property key.
         * @return Property value if found.
         */
        [[nodiscard]] virtual auto getProperty(std::string_view key) const noexcept
            -> std::optional<std::string_view> = 0;

        /**
         * @brief Get all custom properties.
         * @return Map of property key-value pairs.
         */
        [[nodiscard]] virtual auto getProperties() const noexcept
            -> const std::map<std::string, std::string>& = 0;

        // ============================================
        // Entry Point and Paths
        // ============================================

        /**
         * @brief Get the plugin entry point.
         * @return Library filename or class name for loading.
         */
        [[nodiscard]] virtual auto getEntryPoint() const noexcept
            -> std::optional<std::string_view> = 0;

        /**
         * @brief Get the plugin icon path.
         * @return Path to the plugin's icon file.
         */
        [[nodiscard]] virtual auto getIcon() const noexcept
            -> std::optional<std::string_view> = 0;

        // ============================================
        // Platform Support
        // ============================================

        /**
         * @brief Get the list of supported platforms.
         * @return Vector of platform identifiers (e.g., "linux", "windows", "macos").
         */
        [[nodiscard]] virtual auto getSupportedPlatforms() const noexcept
            -> const std::vector<std::string>& = 0;

        /**
         * @brief Check if the plugin supports the current platform.
         * @return True if the plugin supports the current platform.
         */
        [[nodiscard]] virtual auto supportsCurrentPlatform() const noexcept -> bool = 0;

        // ============================================
        // Build Information
        // ============================================

        /**
         * @brief Get the plugin build date.
         * @return Build timestamp.
         */
        [[nodiscard]] virtual auto getBuildDate() const noexcept
            -> std::chrono::system_clock::time_point = 0;

        /**
         * @brief Get the plugin checksum for integrity verification.
         * @return Checksum string (e.g., "sha256:...").
         */
        [[nodiscard]] virtual auto getChecksum() const noexcept
            -> std::optional<std::string_view> = 0;

        // ============================================
        // Priority and Loading Order
        // ============================================

        /**
         * @brief Get the plugin priority.
         * @return Plugin priority for load ordering.
         */
        [[nodiscard]] virtual auto getPriority() const noexcept -> PluginPriority = 0;

        /**
         * @brief Check if the plugin is enabled.
         * @return True if the plugin is enabled.
         */
        [[nodiscard]] virtual auto isEnabled() const noexcept -> bool = 0;

    protected:
        IPluginMetadata() = default;
        IPluginMetadata(const IPluginMetadata&) = default;
        IPluginMetadata(IPluginMetadata&&) = default;
        IPluginMetadata& operator=(const IPluginMetadata&) = default;
        IPluginMetadata& operator=(IPluginMetadata&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINMETADATA_HPP
