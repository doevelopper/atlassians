/**
 * @file IPluginLoader.hpp
 * @brief Interface for loading plugins from various sources
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINLOADER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINLOADER_HPP

#include <filesystem>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/IPluginMetadata.hpp>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Interface for loading plugins from various sources.
     *
     * Plugin loaders are responsible for:
     * - Discovering plugins in specified locations
     * - Loading plugin libraries
     * - Resolving plugin symbols
     * - Creating plugin instances
     * - Unloading plugins
     *
     * Different implementations can load from:
     * - Dynamic libraries (.so, .dll, .dylib)
     * - Static libraries
     * - Script files (for scripting language plugins)
     */
    class IPluginLoader
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~IPluginLoader() noexcept = default;

        // ============================================
        // Plugin Discovery
        // ============================================

        /**
         * @brief Scan a directory for plugins.
         * @param directory The directory to scan.
         * @param recursive Whether to scan subdirectories.
         * @return List of discovered plugin paths.
         */
        [[nodiscard]] virtual auto scanDirectory(const std::filesystem::path& directory,
                                                  bool recursive = false) const
            -> std::vector<std::filesystem::path> = 0;

        /**
         * @brief Check if a path contains a valid plugin.
         * @param path Path to the potential plugin.
         * @return True if the path contains a loadable plugin.
         */
        [[nodiscard]] virtual auto isValidPlugin(const std::filesystem::path& path) const
            -> bool = 0;

        /**
         * @brief Get the file extensions this loader supports.
         * @return Vector of file extensions (e.g., ".so", ".dll").
         */
        [[nodiscard]] virtual auto getSupportedExtensions() const noexcept
            -> std::vector<std::string> = 0;

        // ============================================
        // Plugin Loading
        // ============================================

        /**
         * @brief Load a plugin from the specified path.
         * @param path Path to the plugin library.
         * @return Unique pointer to the loaded plugin.
         * @throws PluginLoadException if loading fails.
         */
        [[nodiscard]] virtual auto loadPlugin(const std::filesystem::path& path)
            -> std::unique_ptr<IPlugin> = 0;

        /**
         * @brief Unload a plugin.
         * @param pluginName Name of the plugin to unload.
         * @throws PluginUnloadException if unloading fails.
         */
        virtual auto unloadPlugin(const std::string& pluginName) -> void = 0;

        /**
         * @brief Check if a plugin is loaded.
         * @param pluginName Name of the plugin.
         * @return True if the plugin is currently loaded.
         */
        [[nodiscard]] virtual auto isLoaded(std::string_view pluginName) const noexcept
            -> bool = 0;

        // ============================================
        // Metadata Access
        // ============================================

        /**
         * @brief Read plugin metadata without fully loading the plugin.
         * @param path Path to the plugin.
         * @return Plugin metadata if available.
         * @throws PluginLoadException if metadata cannot be read.
         */
        [[nodiscard]] virtual auto readMetadata(const std::filesystem::path& path) const
            -> std::unique_ptr<IPluginMetadata> = 0;

        /**
         * @brief Get the API version of a plugin without loading it.
         * @param path Path to the plugin.
         * @return API version string, or empty if not determinable.
         */
        [[nodiscard]] virtual auto getPluginApiVersion(const std::filesystem::path& path) const
            -> std::string = 0;

        // ============================================
        // Reload Support
        // ============================================

        /**
         * @brief Check if this loader supports hot-reloading.
         * @return True if hot-reload is supported.
         */
        [[nodiscard]] virtual auto supportsHotReload() const noexcept -> bool = 0;

        /**
         * @brief Reload a plugin.
         * @param pluginName Name of the plugin to reload.
         * @return The reloaded plugin instance.
         * @throws PluginLoadException if reloading fails.
         */
        [[nodiscard]] virtual auto reloadPlugin(const std::string& pluginName)
            -> std::unique_ptr<IPlugin> = 0;

        // ============================================
        // Library Information
        // ============================================

        /**
         * @brief Get the loader name/type.
         * @return Loader identifier (e.g., "DynamicLibraryLoader").
         */
        [[nodiscard]] virtual auto getLoaderName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the last modification time of a loaded plugin.
         * @param pluginName Name of the plugin.
         * @return Last modification time, or epoch if not available.
         */
        [[nodiscard]] virtual auto getLastModificationTime(std::string_view pluginName) const
            -> std::filesystem::file_time_type = 0;

    protected:
        IPluginLoader() = default;
        IPluginLoader(const IPluginLoader&) = default;
        IPluginLoader(IPluginLoader&&) = default;
        IPluginLoader& operator=(const IPluginLoader&) = default;
        IPluginLoader& operator=(IPluginLoader&&) = default;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_IPLUGINLOADER_HPP
