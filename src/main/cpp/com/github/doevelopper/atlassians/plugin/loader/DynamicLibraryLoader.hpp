/**
 * @file DynamicLibraryLoader.hpp
 * @brief Dynamic library plugin loader implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_LOADER_DYNAMICLIBRARYLOADER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_LOADER_DYNAMICLIBRARYLOADER_HPP

#include <filesystem>
#include <map>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/plugin/core/IPluginLoader.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExport.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::loader
{
    /**
     * @brief Plugin loader that loads plugins from dynamic libraries.
     *
     * This loader supports:
     * - .so files on Linux
     * - .dll files on Windows
     * - .dylib files on macOS
     *
     * Plugins must export the standard entry points:
     * - createPlugin() - Creates a plugin instance
     * - destroyPlugin(IPlugin*) - Destroys a plugin instance
     * - getPluginApiVersion() - Returns the API version
     */
    class DynamicLibraryLoader : public IPluginLoader
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        /**
         * @brief Information about a loaded library.
         */
        struct LoadedLibrary
        {
            void* handle{nullptr};                          ///< Platform-specific library handle
            std::filesystem::path path;                     ///< Path to the library
            CreatePluginFunc createFunc{nullptr};           ///< Plugin creation function
            DestroyPluginFunc destroyFunc{nullptr};         ///< Plugin destruction function
            GetApiVersionFunc getVersionFunc{nullptr};      ///< API version function
            std::filesystem::file_time_type lastModified;   ///< Last modification time
            std::string pluginName;                         ///< Name of the loaded plugin
        };

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Default constructor.
         */
        DynamicLibraryLoader();

        /**
         * @brief Destructor - unloads all libraries.
         */
        ~DynamicLibraryLoader() noexcept override;

        // Delete copy operations
        DynamicLibraryLoader(const DynamicLibraryLoader&) = delete;
        auto operator=(const DynamicLibraryLoader&) -> DynamicLibraryLoader& = delete;

        // Allow move operations
        DynamicLibraryLoader(DynamicLibraryLoader&& other) noexcept;
        auto operator=(DynamicLibraryLoader&& other) noexcept -> DynamicLibraryLoader&;

        // ============================================
        // IPluginLoader Implementation
        // ============================================

        [[nodiscard]] auto scanDirectory(const std::filesystem::path& directory,
                                          bool recursive = false) const
            -> std::vector<std::filesystem::path> override;

        [[nodiscard]] auto isValidPlugin(const std::filesystem::path& path) const
            -> bool override;

        [[nodiscard]] auto getSupportedExtensions() const noexcept
            -> std::vector<std::string> override;

        [[nodiscard]] auto loadPlugin(const std::filesystem::path& path)
            -> std::unique_ptr<IPlugin> override;

        auto unloadPlugin(const std::string& pluginName) -> void override;

        [[nodiscard]] auto isLoaded(std::string_view pluginName) const noexcept
            -> bool override;

        [[nodiscard]] auto readMetadata(const std::filesystem::path& path) const
            -> std::unique_ptr<IPluginMetadata> override;

        [[nodiscard]] auto getPluginApiVersion(const std::filesystem::path& path) const
            -> std::string override;

        [[nodiscard]] auto supportsHotReload() const noexcept -> bool override;

        [[nodiscard]] auto reloadPlugin(const std::string& pluginName)
            -> std::unique_ptr<IPlugin> override;

        [[nodiscard]] auto getLoaderName() const noexcept -> std::string_view override;

        [[nodiscard]] auto getLastModificationTime(std::string_view pluginName) const
            -> std::filesystem::file_time_type override;

        // ============================================
        // Additional Methods
        // ============================================

        /**
         * @brief Get the number of loaded libraries.
         */
        [[nodiscard]] auto getLoadedCount() const noexcept -> std::size_t;

        /**
         * @brief Get information about a loaded library.
         */
        [[nodiscard]] auto getLoadedLibraryInfo(std::string_view pluginName) const
            -> std::optional<LoadedLibrary>;

        /**
         * @brief Get all loaded plugin names.
         */
        [[nodiscard]] auto getLoadedPluginNames() const -> std::vector<std::string>;

    private:
        // ============================================
        // Platform-Specific Operations
        // ============================================

        /**
         * @brief Load a dynamic library.
         */
        [[nodiscard]] auto loadLibrary(const std::filesystem::path& path) -> void*;

        /**
         * @brief Unload a dynamic library.
         */
        auto unloadLibrary(void* handle) -> void;

        /**
         * @brief Get a symbol from a loaded library.
         */
        [[nodiscard]] auto getSymbol(void* handle, const char* symbolName) const -> void*;

        /**
         * @brief Get the last error message.
         */
        [[nodiscard]] auto getLastError() const -> std::string;

        /**
         * @brief Get the platform-specific library extension.
         */
        [[nodiscard]] static auto getPlatformExtension() noexcept -> std::string_view;

        // ============================================
        // Member Variables
        // ============================================

        std::map<std::string, LoadedLibrary> loadedLibraries_;
        mutable std::shared_mutex mutex_;
    };

} // namespace com::github::doevelopper::atlassians::plugin::loader

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_LOADER_DYNAMICLIBRARYLOADER_HPP
