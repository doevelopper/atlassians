/**
 * @file PluginMetadata.hpp
 * @brief Concrete implementation of plugin metadata
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_METADATA_PLUGINMETADATA_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_METADATA_PLUGINMETADATA_HPP

#include <algorithm>
#include <chrono>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::metadata
{
    /**
     * @brief Concrete implementation of IPluginMetadata.
     *
     * This class provides a mutable implementation of plugin metadata
     * that can be populated from various sources (JSON, XML, programmatic).
     */
    class PluginMetadata : public IPluginMetadata
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        // ============================================
        // Constructors
        // ============================================

        /**
         * @brief Default constructor.
         */
        PluginMetadata() noexcept;

        /**
         * @brief Constructor with name and version.
         */
        PluginMetadata(std::string name, std::string version) noexcept;

        /**
         * @brief Destructor.
         */
        ~PluginMetadata() noexcept override;

        /**
         * @brief Copy constructor.
         */
        PluginMetadata(const PluginMetadata& other);

        /**
         * @brief Move constructor.
         */
        PluginMetadata(PluginMetadata&& other) noexcept;

        /**
         * @brief Copy assignment operator.
         */
        auto operator=(const PluginMetadata& other) -> PluginMetadata&;

        /**
         * @brief Move assignment operator.
         */
        auto operator=(PluginMetadata&& other) noexcept -> PluginMetadata&;

        // ============================================
        // IPluginMetadata Implementation
        // ============================================

        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getVersion() const noexcept -> std::string_view override;
        [[nodiscard]] auto getApiVersion() const noexcept -> std::string_view override;
        [[nodiscard]] auto getDescription() const noexcept -> std::string_view override;
        [[nodiscard]] auto getAuthor() const noexcept -> std::string_view override;
        [[nodiscard]] auto getLicense() const noexcept -> std::string_view override;
        [[nodiscard]] auto getHomepage() const noexcept -> std::optional<std::string_view> override;

        [[nodiscard]] auto getProvidedServices() const noexcept
            -> const std::vector<std::string>& override;
        [[nodiscard]] auto getRequiredServices() const noexcept
            -> const std::vector<std::string>& override;
        [[nodiscard]] auto getCapabilities() const noexcept
            -> const std::vector<std::string>& override;
        [[nodiscard]] auto hasCapability(std::string_view capability) const noexcept -> bool override;

        [[nodiscard]] auto getDependencies() const noexcept
            -> const std::vector<PluginDependency>& override;
        [[nodiscard]] auto dependsOn(std::string_view pluginName) const noexcept -> bool override;

        [[nodiscard]] auto getProperty(std::string_view key) const noexcept
            -> std::optional<std::string_view> override;
        [[nodiscard]] auto getProperties() const noexcept
            -> const std::map<std::string, std::string>& override;

        [[nodiscard]] auto getEntryPoint() const noexcept
            -> std::optional<std::string_view> override;
        [[nodiscard]] auto getIcon() const noexcept
            -> std::optional<std::string_view> override;

        [[nodiscard]] auto getSupportedPlatforms() const noexcept
            -> const std::vector<std::string>& override;
        [[nodiscard]] auto supportsCurrentPlatform() const noexcept -> bool override;

        [[nodiscard]] auto getBuildDate() const noexcept
            -> std::chrono::system_clock::time_point override;
        [[nodiscard]] auto getChecksum() const noexcept
            -> std::optional<std::string_view> override;

        [[nodiscard]] auto getPriority() const noexcept -> PluginPriority override;
        [[nodiscard]] auto isEnabled() const noexcept -> bool override;

        // ============================================
        // Setters (Fluent Interface)
        // ============================================

        auto setName(std::string name) -> PluginMetadata&;
        auto setVersion(std::string version) -> PluginMetadata&;
        auto setApiVersion(std::string apiVersion) -> PluginMetadata&;
        auto setDescription(std::string description) -> PluginMetadata&;
        auto setAuthor(std::string author) -> PluginMetadata&;
        auto setLicense(std::string license) -> PluginMetadata&;
        auto setHomepage(std::string homepage) -> PluginMetadata&;

        auto addProvidedService(std::string service) -> PluginMetadata&;
        auto setProvidedServices(std::vector<std::string> services) -> PluginMetadata&;
        auto addRequiredService(std::string service) -> PluginMetadata&;
        auto setRequiredServices(std::vector<std::string> services) -> PluginMetadata&;
        auto addCapability(std::string capability) -> PluginMetadata&;
        auto setCapabilities(std::vector<std::string> capabilities) -> PluginMetadata&;

        auto addDependency(PluginDependency dependency) -> PluginMetadata&;
        auto addDependency(std::string name, std::string versionConstraint, bool optional = false)
            -> PluginMetadata&;
        auto setDependencies(std::vector<PluginDependency> dependencies) -> PluginMetadata&;

        auto setProperty(std::string key, std::string value) -> PluginMetadata&;
        auto setProperties(std::map<std::string, std::string> properties) -> PluginMetadata&;

        auto setEntryPoint(std::string entryPoint) -> PluginMetadata&;
        auto setIcon(std::string icon) -> PluginMetadata&;

        auto addSupportedPlatform(std::string platform) -> PluginMetadata&;
        auto setSupportedPlatforms(std::vector<std::string> platforms) -> PluginMetadata&;

        auto setBuildDate(std::chrono::system_clock::time_point buildDate) -> PluginMetadata&;
        auto setChecksum(std::string checksum) -> PluginMetadata&;

        auto setPriority(PluginPriority priority) -> PluginMetadata&;
        auto setEnabled(bool enabled) -> PluginMetadata&;

        // ============================================
        // Factory Methods
        // ============================================

        /**
         * @brief Create a basic metadata instance.
         */
        [[nodiscard]] static auto create(std::string name, std::string version)
            -> PluginMetadata;

    private:
        std::string name_;
        std::string version_;
        std::string apiVersion_;
        std::string description_;
        std::string author_;
        std::string license_;
        std::optional<std::string> homepage_;

        std::vector<std::string> providedServices_;
        std::vector<std::string> requiredServices_;
        std::vector<std::string> capabilities_;

        std::vector<PluginDependency> dependencies_;
        std::map<std::string, std::string> properties_;

        std::optional<std::string> entryPoint_;
        std::optional<std::string> icon_;

        std::vector<std::string> supportedPlatforms_;

        std::chrono::system_clock::time_point buildDate_;
        std::optional<std::string> checksum_;

        PluginPriority priority_{PluginPriority::Normal};
        bool enabled_{true};
    };

} // namespace com::github::doevelopper::atlassians::plugin::metadata

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_METADATA_PLUGINMETADATA_HPP
