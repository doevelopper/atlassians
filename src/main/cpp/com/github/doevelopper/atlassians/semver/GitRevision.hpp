/*******************************************************************
 * @file   GitRevision.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Git integration utilities for version management (header-only)
 *
 * This header provides compile-time and runtime utilities for integrating
 * Git revision information into version metadata.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_GITREVISION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_GITREVISION_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>

namespace com::github::doevelopper::atlassians::semver
{

// ============================================
// Compile-time Version Information
// ============================================

// These macros should be defined by the build system (CMake)
// If not defined, provide sensible defaults

#ifndef PROJECT_VERSION_MAJOR
    #define PROJECT_VERSION_MAJOR 0
#endif

#ifndef PROJECT_VERSION_MINOR
    #define PROJECT_VERSION_MINOR 0
#endif

#ifndef PROJECT_VERSION_PATCH
    #define PROJECT_VERSION_PATCH 0
#endif

#ifndef PROJECT_VERSION_PRERELEASE
    #define PROJECT_VERSION_PRERELEASE ""
#endif

#ifndef PROJECT_VERSION_BUILD_METADATA
    #define PROJECT_VERSION_BUILD_METADATA ""
#endif

#ifndef GIT_COMMIT_HASH
    #define GIT_COMMIT_HASH "unknown"
#endif

#ifndef GIT_COMMIT_HASH_SHORT
    #define GIT_COMMIT_HASH_SHORT "unknown"
#endif

#ifndef GIT_BRANCH
    #define GIT_BRANCH "unknown"
#endif

#ifndef GIT_TAG
    #define GIT_TAG ""
#endif

#ifndef GIT_IS_DIRTY
    #define GIT_IS_DIRTY false
#endif

#ifndef GIT_COMMIT_DATE
    #define GIT_COMMIT_DATE "unknown"
#endif

#ifndef GIT_COMMIT_COUNT
    #define GIT_COMMIT_COUNT 0
#endif

#ifndef BUILD_TIMESTAMP
    #define BUILD_TIMESTAMP __DATE__ " " __TIME__
#endif

#ifndef BUILD_TYPE
    #define BUILD_TYPE "Debug"
#endif

#ifndef COMPILER_ID
    #define COMPILER_ID "unknown"
#endif

#ifndef COMPILER_VERSION
    #define COMPILER_VERSION "unknown"
#endif

/**
 * @brief Compile-time Git and version information
 *
 * This struct provides access to version and Git information that is
 * embedded at compile time. All methods are constexpr where possible.
 *
 * @example
 * @code
 * // Access version information
 * std::cout << "Version: " << GitRevision::versionString() << std::endl;
 * std::cout << "Commit: " << GitRevision::commitHash() << std::endl;
 * std::cout << "Branch: " << GitRevision::branch() << std::endl;
 *
 * // Check if build is dirty
 * if (GitRevision::isDirty()) {
 *     std::cout << "Warning: Build from dirty working tree" << std::endl;
 * }
 * @endcode
 */
struct GitRevision
{
    // ============================================
    // Version Components
    // ============================================

    /**
     * @brief Get major version number
     */
    [[nodiscard]] static constexpr auto major() noexcept -> std::uint32_t
    {
        return PROJECT_VERSION_MAJOR;
    }

    /**
     * @brief Get minor version number
     */
    [[nodiscard]] static constexpr auto minor() noexcept -> std::uint32_t
    {
        return PROJECT_VERSION_MINOR;
    }

    /**
     * @brief Get patch version number
     */
    [[nodiscard]] static constexpr auto patch() noexcept -> std::uint32_t
    {
        return PROJECT_VERSION_PATCH;
    }

    /**
     * @brief Get pre-release string
     */
    [[nodiscard]] static constexpr auto preRelease() noexcept -> std::string_view
    {
        return PROJECT_VERSION_PRERELEASE;
    }

    /**
     * @brief Get build metadata string
     */
    [[nodiscard]] static constexpr auto buildMetadata() noexcept -> std::string_view
    {
        return PROJECT_VERSION_BUILD_METADATA;
    }

    /**
     * @brief Check if this is a pre-release version
     */
    [[nodiscard]] static constexpr auto isPreRelease() noexcept -> bool
    {
        return std::string_view(PROJECT_VERSION_PRERELEASE).size() > 0;
    }

    /**
     * @brief Get version as numeric value (for quick comparisons)
     *
     * Format: MAJOR * 1000000 + MINOR * 1000 + PATCH
     */
    [[nodiscard]] static constexpr auto versionNumber() noexcept -> std::uint64_t
    {
        return static_cast<std::uint64_t>(major()) * 1000000 +
               static_cast<std::uint64_t>(minor()) * 1000 +
               static_cast<std::uint64_t>(patch());
    }

    // ============================================
    // Git Information
    // ============================================

    /**
     * @brief Get full Git commit hash (40 characters)
     */
    [[nodiscard]] static constexpr auto commitHash() noexcept -> std::string_view
    {
        return GIT_COMMIT_HASH;
    }

    /**
     * @brief Get short Git commit hash (7 characters)
     */
    [[nodiscard]] static constexpr auto commitHashShort() noexcept -> std::string_view
    {
        return GIT_COMMIT_HASH_SHORT;
    }

    /**
     * @brief Get Git branch name
     */
    [[nodiscard]] static constexpr auto branch() noexcept -> std::string_view
    {
        return GIT_BRANCH;
    }

    /**
     * @brief Get Git tag (if any)
     */
    [[nodiscard]] static constexpr auto tag() noexcept -> std::string_view
    {
        return GIT_TAG;
    }

    /**
     * @brief Check if working tree was dirty at build time
     */
    [[nodiscard]] static constexpr auto isDirty() noexcept -> bool
    {
        return GIT_IS_DIRTY;
    }

    /**
     * @brief Get commit date
     */
    [[nodiscard]] static constexpr auto commitDate() noexcept -> std::string_view
    {
        return GIT_COMMIT_DATE;
    }

    /**
     * @brief Get commit count from repository root
     */
    [[nodiscard]] static constexpr auto commitCount() noexcept -> std::uint32_t
    {
        return GIT_COMMIT_COUNT;
    }

    // ============================================
    // Build Information
    // ============================================

    /**
     * @brief Get build timestamp
     */
    [[nodiscard]] static constexpr auto buildTimestamp() noexcept -> std::string_view
    {
        return BUILD_TIMESTAMP;
    }

    /**
     * @brief Get build type (Debug, Release, RelWithDebInfo, MinSizeRel)
     */
    [[nodiscard]] static constexpr auto buildType() noexcept -> std::string_view
    {
        return BUILD_TYPE;
    }

    /**
     * @brief Get compiler identifier
     */
    [[nodiscard]] static constexpr auto compilerId() noexcept -> std::string_view
    {
        return COMPILER_ID;
    }

    /**
     * @brief Get compiler version
     */
    [[nodiscard]] static constexpr auto compilerVersion() noexcept -> std::string_view
    {
        return COMPILER_VERSION;
    }

    // ============================================
    // String Representations
    // ============================================

    /**
     * @brief Get core version string (MAJOR.MINOR.PATCH)
     */
    [[nodiscard]] static auto coreVersionString() -> std::string
    {
        return std::to_string(major()) + "." +
               std::to_string(minor()) + "." +
               std::to_string(patch());
    }

    /**
     * @brief Get full SemVer 2.0.0 version string
     *
     * Format: MAJOR.MINOR.PATCH[-prerelease][+build]
     */
    [[nodiscard]] static auto versionString() -> std::string
    {
        std::string result = coreVersionString();

        if (!preRelease().empty())
        {
            result += "-";
            result += preRelease();
        }

        if (!buildMetadata().empty())
        {
            result += "+";
            result += buildMetadata();
        }

        return result;
    }

    /**
     * @brief Get version string with Git info
     *
     * Format: MAJOR.MINOR.PATCH[-prerelease]+git.HASH[.dirty]
     */
    [[nodiscard]] static auto versionStringWithGit() -> std::string
    {
        std::string result = coreVersionString();

        if (!preRelease().empty())
        {
            result += "-";
            result += preRelease();
        }

        result += "+git.";
        result += commitHashShort();

        if (isDirty())
        {
            result += ".dirty";
        }

        return result;
    }

    /**
     * @brief Get detailed version information string
     *
     * Includes version, Git info, build type, and compiler info.
     */
    [[nodiscard]] static auto detailedVersionString() -> std::string
    {
        std::string result;

        result += "Version: ";
        result += versionString();
        result += "\n";

        result += "Git Commit: ";
        result += commitHash();
        if (isDirty())
        {
            result += " (dirty)";
        }
        result += "\n";

        result += "Git Branch: ";
        result += branch();
        result += "\n";

        if (!tag().empty())
        {
            result += "Git Tag: ";
            result += tag();
            result += "\n";
        }

        result += "Build Type: ";
        result += buildType();
        result += "\n";

        result += "Build Time: ";
        result += buildTimestamp();
        result += "\n";

        result += "Compiler: ";
        result += compilerId();
        result += " ";
        result += compilerVersion();

        return result;
    }

    /**
     * @brief Get JSON representation of version info
     */
    [[nodiscard]] static auto toJson() -> std::string
    {
        std::string json = "{\n";
        json += "  \"major\": " + std::to_string(major()) + ",\n";
        json += "  \"minor\": " + std::to_string(minor()) + ",\n";
        json += "  \"patch\": " + std::to_string(patch()) + ",\n";
        json += "  \"preRelease\": \"" + std::string(preRelease()) + "\",\n";
        json += "  \"buildMetadata\": \"" + std::string(buildMetadata()) + "\",\n";
        json += "  \"version\": \"" + versionString() + "\",\n";
        json += "  \"git\": {\n";
        json += "    \"commit\": \"" + std::string(commitHash()) + "\",\n";
        json += "    \"commitShort\": \"" + std::string(commitHashShort()) + "\",\n";
        json += "    \"branch\": \"" + std::string(branch()) + "\",\n";
        json += "    \"tag\": \"" + std::string(tag()) + "\",\n";
        json += "    \"dirty\": " + std::string(isDirty() ? "true" : "false") + ",\n";
        json += "    \"date\": \"" + std::string(commitDate()) + "\",\n";
        json += "    \"commitCount\": " + std::to_string(commitCount()) + "\n";
        json += "  },\n";
        json += "  \"build\": {\n";
        json += "    \"timestamp\": \"" + std::string(buildTimestamp()) + "\",\n";
        json += "    \"type\": \"" + std::string(buildType()) + "\",\n";
        json += "    \"compiler\": \"" + std::string(compilerId()) + "\",\n";
        json += "    \"compilerVersion\": \"" + std::string(compilerVersion()) + "\"\n";
        json += "  }\n";
        json += "}";

        return json;
    }

    // ============================================
    // Version Checking Utilities
    // ============================================

    /**
     * @brief Check if current version is at least the specified version
     * @param reqMajor Required major version
     * @param reqMinor Required minor version (default: 0)
     * @param reqPatch Required patch version (default: 0)
     * @return true if current version >= required version
     */
    [[nodiscard]] static constexpr auto isAtLeast(std::uint32_t reqMajor,
                                                   std::uint32_t reqMinor = 0,
                                                   std::uint32_t reqPatch = 0) noexcept -> bool
    {
        if (major() > reqMajor) return true;
        if (major() < reqMajor) return false;
        if (minor() > reqMinor) return true;
        if (minor() < reqMinor) return false;
        return patch() >= reqPatch;
    }

    /**
     * @brief Check if current version is less than the specified version
     * @param maxMajor Maximum major version
     * @param maxMinor Maximum minor version (default: 0)
     * @param maxPatch Maximum patch version (default: 0)
     * @return true if current version < maximum version
     */
    [[nodiscard]] static constexpr auto isLessThan(std::uint32_t maxMajor,
                                                    std::uint32_t maxMinor = 0,
                                                    std::uint32_t maxPatch = 0) noexcept -> bool
    {
        if (major() < maxMajor) return true;
        if (major() > maxMajor) return false;
        if (minor() < maxMinor) return true;
        if (minor() > maxMinor) return false;
        return patch() < maxPatch;
    }

    /**
     * @brief Check if current version is exactly the specified version
     */
    [[nodiscard]] static constexpr auto isExactly(std::uint32_t reqMajor,
                                                   std::uint32_t reqMinor,
                                                   std::uint32_t reqPatch) noexcept -> bool
    {
        return major() == reqMajor && minor() == reqMinor && patch() == reqPatch;
    }

    /**
     * @brief Check if current version is within a range [min, max)
     */
    [[nodiscard]] static constexpr auto isInRange(std::uint32_t minMajor,
                                                   std::uint32_t minMinor,
                                                   std::uint32_t minPatch,
                                                   std::uint32_t maxMajor,
                                                   std::uint32_t maxMinor,
                                                   std::uint32_t maxPatch) noexcept -> bool
    {
        return isAtLeast(minMajor, minMinor, minPatch) &&
               isLessThan(maxMajor, maxMinor, maxPatch);
    }
};

// ============================================
// Library Information Constants
// ============================================

/// Library name
inline constexpr std::string_view LIBRARY_NAME = "premisses";

/// Library description
inline constexpr std::string_view LIBRARY_DESCRIPTION =
    "Production-grade C++ OOP framework with Semantic Versioning 2.0.0 support";

/// Library author
inline constexpr std::string_view LIBRARY_AUTHOR = "doevelopper";

/// Library license
inline constexpr std::string_view LIBRARY_LICENSE = "BSD-3-Clause";

/// Library URL
inline constexpr std::string_view LIBRARY_URL =
    "https://github.com/doevelopper/premisses";

}  // namespace com::github::doevelopper::atlassians::semver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_GITREVISION_HPP
