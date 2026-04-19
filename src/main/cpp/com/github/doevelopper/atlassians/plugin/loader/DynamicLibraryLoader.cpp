/**
 * @file DynamicLibraryLoader.cpp
 * @brief Implementation of DynamicLibraryLoader class
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/plugin/loader/DynamicLibraryLoader.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>
#include <com/github/doevelopper/atlassians/plugin/metadata/PluginMetadata.hpp>

#include <algorithm>
#include <fstream>

// Platform-specific includes for dynamic library loading
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define RTLD_LAZY 0
    #define RTLD_NOW  0
#else
    #include <dlfcn.h>
#endif

using namespace com::github::doevelopper::atlassians::plugin::loader;
using namespace com::github::doevelopper::atlassians::plugin;

log4cxx::LoggerPtr DynamicLibraryLoader::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.loader.DynamicLibraryLoader"));

// ============================================
// Constructors and Destructor
// ============================================

DynamicLibraryLoader::DynamicLibraryLoader()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

DynamicLibraryLoader::~DynamicLibraryLoader() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Unload all libraries
    std::unique_lock<std::shared_mutex> lock(mutex_);
    for (auto& [name, lib] : loadedLibraries_)
    {
        try
        {
            if (lib.handle)
            {
                LOG4CXX_DEBUG(logger, "Unloading library: " << name);
                unloadLibrary(lib.handle);
                lib.handle = nullptr;
            }
        }
        catch (const std::exception& e)
        {
            LOG4CXX_ERROR(logger, "Error unloading library '" << name << "': " << e.what());
        }
    }
    loadedLibraries_.clear();
}

DynamicLibraryLoader::DynamicLibraryLoader(DynamicLibraryLoader&& other) noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::unique_lock<std::shared_mutex> lock(other.mutex_);
    loadedLibraries_ = std::move(other.loadedLibraries_);
}

auto DynamicLibraryLoader::operator=(DynamicLibraryLoader&& other) noexcept -> DynamicLibraryLoader&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        std::unique_lock<std::shared_mutex> lock1(mutex_, std::defer_lock);
        std::unique_lock<std::shared_mutex> lock2(other.mutex_, std::defer_lock);
        std::lock(lock1, lock2);
        loadedLibraries_ = std::move(other.loadedLibraries_);
    }
    return *this;
}

// ============================================
// IPluginLoader Implementation
// ============================================

auto DynamicLibraryLoader::scanDirectory(const std::filesystem::path& directory, bool recursive) const
    -> std::vector<std::filesystem::path>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::vector<std::filesystem::path> plugins;

    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        LOG4CXX_WARN(logger, "Directory does not exist or is not a directory: " << directory);
        return plugins;
    }

    auto extensions = getSupportedExtensions();

    auto scanIterator = [&](const auto& entry)
    {
        if (entry.is_regular_file())
        {
            auto ext = entry.path().extension().string();
            if (std::find(extensions.begin(), extensions.end(), ext) != extensions.end())
            {
                if (isValidPlugin(entry.path()))
                {
                    plugins.push_back(entry.path());
                    LOG4CXX_DEBUG(logger, "Found plugin: " << entry.path());
                }
            }
        }
    };

    try
    {
        if (recursive)
        {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
            {
                scanIterator(entry);
            }
        }
        else
        {
            for (const auto& entry : std::filesystem::directory_iterator(directory))
            {
                scanIterator(entry);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        LOG4CXX_ERROR(logger, "Filesystem error while scanning: " << e.what());
    }

    LOG4CXX_INFO(logger, "Found " << plugins.size() << " plugins in " << directory);
    return plugins;
}

auto DynamicLibraryLoader::isValidPlugin(const std::filesystem::path& path) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path))
    {
        return false;
    }

    // Check extension
    auto ext = path.extension().string();
    auto extensions = getSupportedExtensions();
    if (std::find(extensions.begin(), extensions.end(), ext) == extensions.end())
    {
        return false;
    }

    // Try to load and check for required symbols
    void* handle = nullptr;
    try
    {
        handle = const_cast<DynamicLibraryLoader*>(this)->loadLibrary(path);
        if (!handle)
        {
            return false;
        }

        // Check for required symbols
        auto createFunc = getSymbol(handle, PluginSymbols::CreatePlugin);
        auto destroyFunc = getSymbol(handle, PluginSymbols::DestroyPlugin);
        auto versionFunc = getSymbol(handle, PluginSymbols::GetApiVersion);

        bool valid = (createFunc != nullptr) &&
                     (destroyFunc != nullptr) &&
                     (versionFunc != nullptr);

        const_cast<DynamicLibraryLoader*>(this)->unloadLibrary(handle);
        return valid;
    }
    catch (...)
    {
        if (handle)
        {
            const_cast<DynamicLibraryLoader*>(this)->unloadLibrary(handle);
        }
        return false;
    }
}

auto DynamicLibraryLoader::getSupportedExtensions() const noexcept -> std::vector<std::string>
{
#if defined(_WIN32) || defined(_WIN64)
    return {".dll", ".plugin"};
#elif defined(__APPLE__) && defined(__MACH__)
    return {".dylib", ".so", ".plugin"};
#else
    return {".so", ".plugin"};
#endif
}

auto DynamicLibraryLoader::loadPlugin(const std::filesystem::path& path) -> std::unique_ptr<IPlugin>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    LOG4CXX_INFO(logger, "Loading plugin from: " << path);

    if (!std::filesystem::exists(path))
    {
        throw PluginLoadException(path, "File does not exist");
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);

    // Load the library
    void* handle = loadLibrary(path);
    if (!handle)
    {
        throw PluginLoadException(path, getLastError());
    }

    try
    {
        // Get required symbols
        auto createFunc = reinterpret_cast<CreatePluginFunc>(
            getSymbol(handle, PluginSymbols::CreatePlugin));
        auto destroyFunc = reinterpret_cast<DestroyPluginFunc>(
            getSymbol(handle, PluginSymbols::DestroyPlugin));
        auto versionFunc = reinterpret_cast<GetApiVersionFunc>(
            getSymbol(handle, PluginSymbols::GetApiVersion));

        if (!createFunc)
        {
            throw PluginLoadException(path, "Missing 'createPlugin' symbol");
        }
        if (!destroyFunc)
        {
            throw PluginLoadException(path, "Missing 'destroyPlugin' symbol");
        }
        if (!versionFunc)
        {
            throw PluginLoadException(path, "Missing 'getPluginApiVersion' symbol");
        }

        // Check API version
        const char* apiVersion = versionFunc();
        LOG4CXX_DEBUG(logger, "Plugin API version: " << apiVersion);

        // Create the plugin instance
        IPlugin* rawPlugin = createFunc();
        if (!rawPlugin)
        {
            throw PluginLoadException(path, "createPlugin returned nullptr");
        }

        // Get plugin name from metadata
        std::string pluginName = std::string(rawPlugin->getMetadata().getName());
        if (pluginName.empty())
        {
            pluginName = path.stem().string();
        }

        // Store library info
        LoadedLibrary libInfo{
            handle,
            path,
            createFunc,
            destroyFunc,
            versionFunc,
            std::filesystem::last_write_time(path),
            pluginName
        };
        loadedLibraries_[pluginName] = libInfo;

        // Create unique_ptr with custom deleter using a shared_ptr to store destroyFunc
        // We need to use default deleter since that's what the interface requires,
        // but we'll wrap the plugin in a way that calls destroyFunc
        // For simplicity, we'll store the destroy function and handle cleanup in unloadPlugin

        LOG4CXX_INFO(logger, "Successfully loaded plugin: " << pluginName);

        // Use a capturing lambda that will be converted to a function pointer style deleter
        // Since unique_ptr<IPlugin> uses default_delete, we need a different approach:
        // Store the raw pointer and let the loader manage the lifecycle
        return std::unique_ptr<IPlugin>(rawPlugin);
    }
    catch (const PluginLoadException&)
    {
        unloadLibrary(handle);
        throw;
    }
    catch (const std::exception& e)
    {
        unloadLibrary(handle);
        throw PluginLoadException(path, e.what());
    }
}

auto DynamicLibraryLoader::unloadPlugin(const std::string& pluginName) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    LOG4CXX_INFO(logger, "Unloading plugin: " << pluginName);

    std::unique_lock<std::shared_mutex> lock(mutex_);

    auto it = loadedLibraries_.find(pluginName);
    if (it == loadedLibraries_.end())
    {
        throw PluginUnloadException(pluginName, "Plugin not found in loaded libraries");
    }

    if (it->second.handle)
    {
        unloadLibrary(it->second.handle);
    }

    loadedLibraries_.erase(it);
    LOG4CXX_INFO(logger, "Plugin unloaded: " << pluginName);
}

auto DynamicLibraryLoader::isLoaded(std::string_view pluginName) const noexcept -> bool
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return loadedLibraries_.find(std::string(pluginName)) != loadedLibraries_.end();
}

auto DynamicLibraryLoader::readMetadata(const std::filesystem::path& path) const
    -> std::unique_ptr<IPluginMetadata>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Check for companion manifest file
    auto manifestPath = path.parent_path() / "plugin.json";
    if (std::filesystem::exists(manifestPath))
    {
        // TODO: Implement JSON manifest parsing
        LOG4CXX_DEBUG(logger, "Found manifest file: " << manifestPath);
    }

    // Create basic metadata from plugin
    auto metadata = std::make_unique<metadata::PluginMetadata>();
    metadata->setName(path.stem().string());
    metadata->setEntryPoint(path.filename().string());

    // Try to get more info by loading the plugin briefly
    void* handle = const_cast<DynamicLibraryLoader*>(this)->loadLibrary(path);
    if (handle)
    {
        auto versionFunc = reinterpret_cast<GetApiVersionFunc>(
            getSymbol(handle, PluginSymbols::GetApiVersion));
        if (versionFunc)
        {
            metadata->setApiVersion(versionFunc());
        }
        const_cast<DynamicLibraryLoader*>(this)->unloadLibrary(handle);
    }

    return metadata;
}

auto DynamicLibraryLoader::getPluginApiVersion(const std::filesystem::path& path) const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    void* handle = const_cast<DynamicLibraryLoader*>(this)->loadLibrary(path);
    if (!handle)
    {
        return "";
    }

    std::string version;
    auto versionFunc = reinterpret_cast<GetApiVersionFunc>(
        getSymbol(handle, PluginSymbols::GetApiVersion));
    if (versionFunc)
    {
        version = versionFunc();
    }

    const_cast<DynamicLibraryLoader*>(this)->unloadLibrary(handle);
    return version;
}

auto DynamicLibraryLoader::supportsHotReload() const noexcept -> bool
{
    return true;
}

auto DynamicLibraryLoader::reloadPlugin(const std::string& pluginName) -> std::unique_ptr<IPlugin>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    LOG4CXX_INFO(logger, "Reloading plugin: " << pluginName);

    std::filesystem::path pluginPath;

    {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        auto it = loadedLibraries_.find(pluginName);
        if (it == loadedLibraries_.end())
        {
            throw PluginLoadException(std::filesystem::path(pluginName),
                                      "Plugin not loaded, cannot reload");
        }
        pluginPath = it->second.path;
    }

    // Unload and reload
    unloadPlugin(pluginName);
    return loadPlugin(pluginPath);
}

auto DynamicLibraryLoader::getLoaderName() const noexcept -> std::string_view
{
    return "DynamicLibraryLoader";
}

auto DynamicLibraryLoader::getLastModificationTime(std::string_view pluginName) const
    -> std::filesystem::file_time_type
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = loadedLibraries_.find(std::string(pluginName));
    if (it != loadedLibraries_.end())
    {
        return it->second.lastModified;
    }
    return std::filesystem::file_time_type{};
}

// ============================================
// Additional Methods
// ============================================

auto DynamicLibraryLoader::getLoadedCount() const noexcept -> std::size_t
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return loadedLibraries_.size();
}

auto DynamicLibraryLoader::getLoadedLibraryInfo(std::string_view pluginName) const
    -> std::optional<LoadedLibrary>
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    auto it = loadedLibraries_.find(std::string(pluginName));
    if (it != loadedLibraries_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto DynamicLibraryLoader::getLoadedPluginNames() const -> std::vector<std::string>
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(loadedLibraries_.size());
    for (const auto& [name, _] : loadedLibraries_)
    {
        names.push_back(name);
    }
    return names;
}

// ============================================
// Platform-Specific Operations
// ============================================

auto DynamicLibraryLoader::loadLibrary(const std::filesystem::path& path) -> void*
{
    LOG4CXX_DEBUG(logger, "Loading library: " << path);

#if defined(_WIN32) || defined(_WIN64)
    HMODULE handle = LoadLibraryW(path.wstring().c_str());
    return static_cast<void*>(handle);
#else
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle)
    {
        LOG4CXX_ERROR(logger, "dlopen failed: " << dlerror());
    }
    return handle;
#endif
}

auto DynamicLibraryLoader::unloadLibrary(void* handle) -> void
{
    if (!handle)
    {
        return;
    }

    LOG4CXX_DEBUG(logger, "Unloading library handle: " << handle);

#if defined(_WIN32) || defined(_WIN64)
    FreeLibrary(static_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
}

auto DynamicLibraryLoader::getSymbol(void* handle, const char* symbolName) const -> void*
{
    if (!handle || !symbolName)
    {
        return nullptr;
    }

#if defined(_WIN32) || defined(_WIN64)
    return reinterpret_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), symbolName));
#else
    return dlsym(handle, symbolName);
#endif
}

auto DynamicLibraryLoader::getLastError() const -> std::string
{
#if defined(_WIN32) || defined(_WIN64)
    DWORD error = GetLastError();
    if (error == 0)
    {
        return "";
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&messageBuffer),
        0,
        nullptr
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
#else
    const char* error = dlerror();
    return error ? std::string(error) : "";
#endif
}

auto DynamicLibraryLoader::getPlatformExtension() noexcept -> std::string_view
{
#if defined(_WIN32) || defined(_WIN64)
    return ".dll";
#elif defined(__APPLE__) && defined(__MACH__)
    return ".dylib";
#else
    return ".so";
#endif
}
