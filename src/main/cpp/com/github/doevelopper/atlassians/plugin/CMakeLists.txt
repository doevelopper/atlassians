# ============================================================================
# Plugin Framework CMake Configuration
# ============================================================================
#
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2024 ACME Technology Corp. All rights reserved.
#

cmake_minimum_required(VERSION 3.20)

# ============================================================================
# Plugin Core Library
# ============================================================================

set(PLUGIN_SOURCES
    # Core
    ${CMAKE_CURRENT_SOURCE_DIR}/core/BasePlugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PluginTypes.cpp

    # Metadata
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/PluginMetadata.cpp

    # Loader
    ${CMAKE_CURRENT_SOURCE_DIR}/loader/DynamicLibraryLoader.cpp

    # Registry
    ${CMAKE_CURRENT_SOURCE_DIR}/registry/PluginRegistry.cpp

    # Communication
    ${CMAKE_CURRENT_SOURCE_DIR}/communication/EventBus.cpp

    # Resolver
    ${CMAKE_CURRENT_SOURCE_DIR}/resolver/DependencyResolver.cpp

    # Manager
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/PluginManager.cpp

    # Context
    ${CMAKE_CURRENT_SOURCE_DIR}/context/PluginContext.cpp
)

set(PLUGIN_HEADERS
    # Core Interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPlugin.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginContext.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginLoader.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginRegistry.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginMetadata.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginCommunicator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginValidator.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/IPluginDependencyResolver.hpp

    # Core Types and Utilities
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PluginTypes.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PluginExport.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/PluginExceptions.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/BasePlugin.hpp

    # Metadata
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata/PluginMetadata.hpp

    # Loader
    ${CMAKE_CURRENT_SOURCE_DIR}/loader/DynamicLibraryLoader.hpp

    # Registry
    ${CMAKE_CURRENT_SOURCE_DIR}/registry/PluginRegistry.hpp

    # Communication
    ${CMAKE_CURRENT_SOURCE_DIR}/communication/EventBus.hpp

    # Resolver
    ${CMAKE_CURRENT_SOURCE_DIR}/resolver/DependencyResolver.hpp

    # Manager
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/PluginManager.hpp

    # Context
    ${CMAKE_CURRENT_SOURCE_DIR}/context/PluginContext.hpp
)

# Create the plugin library
add_library(PremissesPlugin SHARED
    ${PLUGIN_SOURCES}
    ${PLUGIN_HEADERS}
)

add_library(Premisses::Plugin ALIAS PremissesPlugin)

# Set library properties
set_target_properties(PremissesPlugin PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "premisses-plugin"
)

# Include directories
target_include_directories(PremissesPlugin
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/main/cpp>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link dependencies
target_link_libraries(PremissesPlugin
    PUBLIC
        log4cxx::log4cxx
    PRIVATE
        ${CMAKE_DL_LIBS}
        Threads::Threads
)

# Compiler options
target_compile_options(PremissesPlugin
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fdiagnostics-parseable-fixits>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:GNU>>:${PREMISSES_GNU_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:${PREMISSES_COMMON_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CXX_COMPILER_ID:MSVC>>:/W4>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Debug>>:${PREMISSES_DEBUG_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release>>:${PREMISSES_RELEASE_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:RelWithDebInfo>>:${PREMISSES_RELWITHDEBINFO_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:MinSizeRel>>:${PREMISSES_MINSIZEREL_CXX_FLAGS}>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${ENABLE_COVERAGE}>,$<CONFIG:Debug>>:${PREMISSES_COVERAGE_CXX_FLAGS}>
)

target_link_options(PremissesPlugin
    PRIVATE
        $<$<CONFIG:Release>:-Wl,--gc-sections>
        $<$<AND:$<BOOL:${ENABLE_COVERAGE}>,$<CONFIG:Debug>>:${PREMISSES_COVERAGE_LINK_FLAGS}>
)

# Export definitions
target_compile_definitions(PremissesPlugin
    PRIVATE
        PLUGIN_BUILDING_DLL
    PUBLIC
        $<$<PLATFORM_ID:Windows>:PLUGIN_PLATFORM_WINDOWS>
        $<$<PLATFORM_ID:Linux>:PLUGIN_PLATFORM_LINUX>
        $<$<PLATFORM_ID:Darwin>:PLUGIN_PLATFORM_MACOS>
)

# ============================================================================
# Example Plugin
# ============================================================================

if(BUILD_EXAMPLES)
    add_library(SimpleCalculatorPlugin MODULE
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/SimpleCalculatorPlugin.cpp
    )

    set_target_properties(SimpleCalculatorPlugin PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        PREFIX ""  # Remove 'lib' prefix on Unix
        OUTPUT_NAME "simple_calculator_plugin"
    )

    target_include_directories(SimpleCalculatorPlugin
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/main/cpp
    )

    target_link_libraries(SimpleCalculatorPlugin
        PRIVATE
            Premisses::Plugin
            log4cxx::log4cxx
    )
endif()

# ============================================================================
# Installation
# ============================================================================

include(GNUInstallDirs)

install(TARGETS PremissesPlugin
    EXPORT PremissesPluginTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/com/github/doevelopper/atlassians/plugin
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT PremissesPluginTargets
    FILE PremissesPluginTargets.cmake
    NAMESPACE Premisses::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PremissesPlugin
)

# Generate and install package config
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/PremissesPluginConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/src/main/resources/cmake/PremissesPluginConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PremissesPluginConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PremissesPlugin
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PremissesPluginConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PremissesPluginConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PremissesPlugin
)
