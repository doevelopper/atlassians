/**
 * @file PluginExport.hpp
 * @brief Platform-specific export macros for plugin development
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXPORT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXPORT_HPP

// Platform-specific export/import macros
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
    #ifdef BUILDING_PLUGIN
        #ifdef __GNUC__
            #define PLUGIN_EXPORT __attribute__((dllexport))
        #else
            #define PLUGIN_EXPORT __declspec(dllexport)
        #endif
    #else
        #ifdef __GNUC__
            #define PLUGIN_IMPORT __attribute__((dllimport))
        #else
            #define PLUGIN_IMPORT __declspec(dllimport)
        #endif
    #endif
    #define PLUGIN_LOCAL
#else
    #if __GNUC__ >= 4 || defined(__clang__)
        #define PLUGIN_EXPORT __attribute__((visibility("default")))
        #define PLUGIN_IMPORT __attribute__((visibility("default")))
        #define PLUGIN_LOCAL  __attribute__((visibility("hidden")))
    #else
        #define PLUGIN_EXPORT
        #define PLUGIN_IMPORT
        #define PLUGIN_LOCAL
    #endif
#endif

// Unified PLUGIN_API macro
#ifdef BUILDING_PLUGIN
    #define PLUGIN_API PLUGIN_EXPORT
#else
    #define PLUGIN_API PLUGIN_IMPORT
#endif

// C linkage macro for plugin entry points
#ifdef __cplusplus
    #define PLUGIN_EXTERN_C extern "C"
#else
    #define PLUGIN_EXTERN_C
#endif

// Forward declaration for plugin interface
namespace com::github::doevelopper::atlassians::plugin
{
    class IPlugin;
}

/**
 * @brief Plugin API version string.
 *
 * This should be updated when breaking changes are made to the plugin API.
 */
#define PLUGIN_API_VERSION "2.0.0"

/**
 * @brief Macro to define standard plugin entry points.
 *
 * This macro should be used at the end of the plugin implementation file
 * to export the required entry points for plugin loading.
 *
 * @param PluginClass The class implementing IPlugin interface
 *
 * @example
 * @code
 * DECLARE_PLUGIN(MyPlugin)
 * @endcode
 */
#define DECLARE_PLUGIN(PluginClass) \
    PLUGIN_EXTERN_C { \
        PLUGIN_API com::github::doevelopper::atlassians::plugin::IPlugin* createPlugin() \
        { \
            return new PluginClass(); \
        } \
        \
        PLUGIN_API void destroyPlugin(com::github::doevelopper::atlassians::plugin::IPlugin* plugin) \
        { \
            delete plugin; \
        } \
        \
        PLUGIN_API const char* getPluginApiVersion() \
        { \
            return PLUGIN_API_VERSION; \
        } \
    }

/**
 * @brief Macro to define plugin entry points with custom API version.
 *
 * @param PluginClass The class implementing IPlugin interface
 * @param ApiVersion The API version string this plugin supports
 */
#define DECLARE_PLUGIN_WITH_VERSION(PluginClass, ApiVersion) \
    PLUGIN_EXTERN_C { \
        PLUGIN_API com::github::doevelopper::atlassians::plugin::IPlugin* createPlugin() \
        { \
            return new PluginClass(); \
        } \
        \
        PLUGIN_API void destroyPlugin(com::github::doevelopper::atlassians::plugin::IPlugin* plugin) \
        { \
            delete plugin; \
        } \
        \
        PLUGIN_API const char* getPluginApiVersion() \
        { \
            return ApiVersion; \
        } \
    }

/**
 * @brief Function pointer types for plugin entry points.
 */
namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Function pointer type for plugin creation.
     */
    using CreatePluginFunc = IPlugin* (*)();

    /**
     * @brief Function pointer type for plugin destruction.
     */
    using DestroyPluginFunc = void (*)(IPlugin*);

    /**
     * @brief Function pointer type for API version query.
     */
    using GetApiVersionFunc = const char* (*)();

    /**
     * @brief Symbol names for plugin entry points.
     */
    namespace PluginSymbols
    {
        inline constexpr const char* CreatePlugin = "createPlugin";
        inline constexpr const char* DestroyPlugin = "destroyPlugin";
        inline constexpr const char* GetApiVersion = "getPluginApiVersion";
    }

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXPORT_HPP
