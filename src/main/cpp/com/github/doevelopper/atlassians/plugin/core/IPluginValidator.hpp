/**
 * @file IPluginValidator.hpp
 * @brief Interface for plugin validation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINVALIDATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINVALIDATOR_HPP

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginTypes.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for plugin validation.
     *
     * Plugin validators check various aspects of a plugin:
     * - Manifest validity
     * - Code signatures
     * - Checksums
     * - API version compatibility
     * - Security requirements
     *
     * Multiple validators can be chained together using the
     * Chain of Responsibility pattern.
     */
    class IPluginValidator
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginValidator() noexcept = default;

        // ============================================
        // Validation
        // ============================================

        /**
         * @brief Validate a plugin at a given path.
         * @param pluginPath Path to the plugin.
         * @return Validation result with errors and warnings.
         */
        [[nodiscard]] virtual auto validate(const std::filesystem::path& pluginPath) const
            -> ValidationResult = 0;

        /**
         * @brief Validate plugin metadata.
         * @param metadata The metadata to validate.
         * @return Validation result.
         */
        [[nodiscard]] virtual auto validateMetadata(const IPluginMetadata& metadata) const
            -> ValidationResult = 0;

        /**
         * @brief Validate API version compatibility.
         * @param pluginApiVersion The plugin's API version.
         * @param hostApiVersion The host's API version.
         * @return True if versions are compatible.
         */
        [[nodiscard]] virtual auto validateApiVersion(std::string_view pluginApiVersion,
                                                       std::string_view hostApiVersion) const
            -> bool = 0;

        // ============================================
        // Security Validation
        // ============================================

        /**
         * @brief Verify the plugin's digital signature.
         * @param pluginPath Path to the plugin.
         * @return Validation result.
         */
        [[nodiscard]] virtual auto verifySignature(const std::filesystem::path& pluginPath) const
            -> ValidationResult = 0;

        /**
         * @brief Verify the plugin's checksum.
         * @param pluginPath Path to the plugin.
         * @param expectedChecksum The expected checksum value.
         * @return True if checksum matches.
         */
        [[nodiscard]] virtual auto verifyChecksum(const std::filesystem::path& pluginPath,
                                                   std::string_view expectedChecksum) const
            -> bool = 0;

        // ============================================
        // Chain of Responsibility
        // ============================================

        /**
         * @brief Set the next validator in the chain.
         * @param next The next validator.
         */
        virtual auto setNext(std::shared_ptr<IPluginValidator> next) -> void = 0;

        /**
         * @brief Get the next validator in the chain.
         * @return The next validator, or nullptr if none.
         */
        [[nodiscard]] virtual auto getNext() const noexcept
            -> std::shared_ptr<IPluginValidator> = 0;

        // ============================================
        // Configuration
        // ============================================

        /**
         * @brief Get the validator name.
         * @return Name identifying this validator.
         */
        [[nodiscard]] virtual auto getName() const noexcept -> std::string_view = 0;

        /**
         * @brief Check if this validator is enabled.
         * @return True if the validator is enabled.
         */
        [[nodiscard]] virtual auto isEnabled() const noexcept -> bool = 0;

        /**
         * @brief Enable or disable this validator.
         * @param enabled Whether to enable the validator.
         */
        virtual auto setEnabled(bool enabled) -> void = 0;

    protected:
        IPluginValidator() = default;
        IPluginValidator(const IPluginValidator&) = default;
        IPluginValidator(IPluginValidator&&) = default;
        IPluginValidator& operator=(const IPluginValidator&) = default;
        IPluginValidator& operator=(IPluginValidator&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINVALIDATOR_HPP
