/*******************************************************************
 * @file   Version.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Semantic Versioning 2.0.0 compliant Version class implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/semver/Version.hpp>
#include <com/github/doevelopper/atlassians/semver/VersionConstraint.hpp>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iomanip>
#include <numeric>
#include <sstream>

namespace com::github::doevelopper::atlassians::semver
{

// ============================================
// Logger Definition
// ============================================

log4cxx::LoggerPtr Version::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.semver.Version"));

// ============================================
// PIMPL Implementation Struct
// ============================================

struct Version::Impl
{
    std::uint32_t major = 0;
    std::uint32_t minor = 0;
    std::uint32_t patch = 0;
    std::string preRelease;
    std::string buildMetadata;
    IdentifierList preReleaseIds;
    IdentifierList buildMetadataIds;

    Impl() noexcept = default;

    Impl(std::uint32_t maj, std::uint32_t min, std::uint32_t pat)
        : major(maj)
        , minor(min)
        , patch(pat)
    {
    }

    Impl(std::uint32_t maj, std::uint32_t min, std::uint32_t pat,
         std::string_view preRel, std::string_view build,
         IdentifierList preRelIds, IdentifierList buildIds)
        : major(maj)
        , minor(min)
        , patch(pat)
        , preRelease(preRel)
        , buildMetadata(build)
        , preReleaseIds(std::move(preRelIds))
        , buildMetadataIds(std::move(buildIds))
    {
    }

    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

// ============================================
// Rule of Five: Special Member Functions
// ============================================

Version::Version() noexcept
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Version::Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch)
    : m_pImpl(std::make_unique<Impl>(major, minor, patch))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Creating version " << major << "." << minor << "." << patch);
}

Version::Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch,
                 std::string_view preRelease)
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    m_pImpl->major = major;
    m_pImpl->minor = minor;
    m_pImpl->patch = patch;

    if (!preRelease.empty())
    {
        // Validate and parse pre-release
        auto ids = split(preRelease, '.');
        for (const auto& id : ids)
        {
            validatePreReleaseIdentifier(id);
        }
        m_pImpl->preRelease = std::string(preRelease);
        m_pImpl->preReleaseIds = std::move(ids);
    }
}

Version::Version(std::uint32_t major, std::uint32_t minor, std::uint32_t patch,
                 std::string_view preRelease, std::string_view buildMetadata)
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    m_pImpl->major = major;
    m_pImpl->minor = minor;
    m_pImpl->patch = patch;

    if (!preRelease.empty())
    {
        auto ids = split(preRelease, '.');
        for (const auto& id : ids)
        {
            validatePreReleaseIdentifier(id);
        }
        m_pImpl->preRelease = std::string(preRelease);
        m_pImpl->preReleaseIds = std::move(ids);
    }

    if (!buildMetadata.empty())
    {
        auto ids = split(buildMetadata, '.');
        for (const auto& id : ids)
        {
            validateBuildMetadataIdentifier(id);
        }
        m_pImpl->buildMetadata = std::string(buildMetadata);
        m_pImpl->buildMetadataIds = std::move(ids);
    }
}

Version::~Version() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Version::Version(const Version& other)
    : m_pImpl(std::make_unique<Impl>(*other.m_pImpl))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto Version::operator=(const Version& other) -> Version&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

Version::Version(Version&& other) noexcept = default;

auto Version::operator=(Version&& other) noexcept -> Version& = default;

// ============================================
// Static Factory Methods
// ============================================

auto Version::parse(std::string_view versionString) -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing: " << versionString);

    if (versionString.empty())
    {
        throw VersionException("Version string cannot be empty");
    }

    // Use the SemVer 2.0.0 regex pattern
    static const std::regex semverRegex(SEMVER_REGEX_PATTERN);

    std::string versionStr(versionString);
    std::smatch match;

    if (!std::regex_match(versionStr, match, semverRegex))
    {
        throw VersionException("Invalid SemVer 2.0.0 format: " + versionStr);
    }

    // Parse major version
    std::uint32_t major = 0;
    {
        const std::string& majorStr = match[1].str();
        auto [ptr, ec] = std::from_chars(majorStr.data(), majorStr.data() + majorStr.size(), major);
        if (ec != std::errc())
        {
            throw VersionException("Invalid major version: " + majorStr);
        }
    }

    // Parse minor version
    std::uint32_t minor = 0;
    {
        const std::string& minorStr = match[2].str();
        auto [ptr, ec] = std::from_chars(minorStr.data(), minorStr.data() + minorStr.size(), minor);
        if (ec != std::errc())
        {
            throw VersionException("Invalid minor version: " + minorStr);
        }
    }

    // Parse patch version
    std::uint32_t patch = 0;
    {
        const std::string& patchStr = match[3].str();
        auto [ptr, ec] = std::from_chars(patchStr.data(), patchStr.data() + patchStr.size(), patch);
        if (ec != std::errc())
        {
            throw VersionException("Invalid patch version: " + patchStr);
        }
    }

    // Parse pre-release (optional)
    std::string preRelease;
    IdentifierList preReleaseIds;
    if (match[4].matched && !match[4].str().empty())
    {
        preRelease = match[4].str();
        preReleaseIds = split(preRelease, '.');

        // Validate each pre-release identifier
        for (const auto& id : preReleaseIds)
        {
            validatePreReleaseIdentifier(id);
        }
    }

    // Parse build metadata (optional)
    std::string buildMetadata;
    IdentifierList buildMetadataIds;
    if (match[5].matched && !match[5].str().empty())
    {
        buildMetadata = match[5].str();
        buildMetadataIds = split(buildMetadata, '.');

        // Validate each build metadata identifier
        for (const auto& id : buildMetadataIds)
        {
            validateBuildMetadataIdentifier(id);
        }
    }

    Version result;
    result.m_pImpl->major = major;
    result.m_pImpl->minor = minor;
    result.m_pImpl->patch = patch;
    result.m_pImpl->preRelease = std::move(preRelease);
    result.m_pImpl->buildMetadata = std::move(buildMetadata);
    result.m_pImpl->preReleaseIds = std::move(preReleaseIds);
    result.m_pImpl->buildMetadataIds = std::move(buildMetadataIds);

    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Parsed version: " << result.toString());

    return result;
}

auto Version::tryParse(std::string_view versionString) noexcept -> std::optional<Version>
{
    try
    {
        return parse(versionString);
    }
    catch (const VersionException&)
    {
        return std::nullopt;
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

auto Version::isValid(std::string_view versionString) noexcept -> bool
{
    return tryParse(versionString).has_value();
}

// ============================================
// Accessors (Getters)
// ============================================

auto Version::major() const noexcept -> std::uint32_t
{
    return m_pImpl->major;
}

auto Version::minor() const noexcept -> std::uint32_t
{
    return m_pImpl->minor;
}

auto Version::patch() const noexcept -> std::uint32_t
{
    return m_pImpl->patch;
}

auto Version::preRelease() const noexcept -> std::string_view
{
    return m_pImpl->preRelease;
}

auto Version::preReleaseIdentifiers() const noexcept -> const IdentifierList&
{
    return m_pImpl->preReleaseIds;
}

auto Version::buildMetadata() const noexcept -> std::string_view
{
    return m_pImpl->buildMetadata;
}

auto Version::buildMetadataIdentifiers() const noexcept -> const IdentifierList&
{
    return m_pImpl->buildMetadataIds;
}

auto Version::hasPreRelease() const noexcept -> bool
{
    return !m_pImpl->preRelease.empty();
}

auto Version::hasBuildMetadata() const noexcept -> bool
{
    return !m_pImpl->buildMetadata.empty();
}

auto Version::isStable() const noexcept -> bool
{
    // A version is stable if:
    // 1. Major version is > 0 (not in initial development)
    // 2. Has no pre-release identifier
    return m_pImpl->major > 0 && !hasPreRelease();
}

auto Version::isPreRelease() const noexcept -> bool
{
    return hasPreRelease();
}

// ============================================
// Version Increment Operations
// ============================================

auto Version::incrementMajor(bool preserveBuildMetadata) const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major + 1, 0, 0);

    if (preserveBuildMetadata && hasBuildMetadata())
    {
        result.m_pImpl->buildMetadata = m_pImpl->buildMetadata;
        result.m_pImpl->buildMetadataIds = m_pImpl->buildMetadataIds;
    }

    return result;
}

auto Version::incrementMinor(bool preserveBuildMetadata) const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor + 1, 0);

    if (preserveBuildMetadata && hasBuildMetadata())
    {
        result.m_pImpl->buildMetadata = m_pImpl->buildMetadata;
        result.m_pImpl->buildMetadataIds = m_pImpl->buildMetadataIds;
    }

    return result;
}

auto Version::incrementPatch(bool preserveBuildMetadata) const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor, m_pImpl->patch + 1);

    if (preserveBuildMetadata && hasBuildMetadata())
    {
        result.m_pImpl->buildMetadata = m_pImpl->buildMetadata;
        result.m_pImpl->buildMetadataIds = m_pImpl->buildMetadataIds;
    }

    return result;
}

auto Version::withPreRelease(std::string_view preRelease) const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor, m_pImpl->patch,
                   preRelease, m_pImpl->buildMetadata);
    return result;
}

auto Version::withBuildMetadata(std::string_view buildMetadata) const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor, m_pImpl->patch,
                   m_pImpl->preRelease, buildMetadata);
    return result;
}

auto Version::withoutPreRelease() const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor, m_pImpl->patch,
                   "", m_pImpl->buildMetadata);
    return result;
}

auto Version::withoutBuildMetadata() const -> Version
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    Version result(m_pImpl->major, m_pImpl->minor, m_pImpl->patch,
                   m_pImpl->preRelease, "");
    return result;
}

// ============================================
// Comparison Operations
// ============================================

auto Version::compare(const Version& other) const noexcept -> int
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Step 1: Compare MAJOR version
    if (m_pImpl->major != other.m_pImpl->major)
    {
        return static_cast<int>(m_pImpl->major) - static_cast<int>(other.m_pImpl->major);
    }

    // Step 2: Compare MINOR version
    if (m_pImpl->minor != other.m_pImpl->minor)
    {
        return static_cast<int>(m_pImpl->minor) - static_cast<int>(other.m_pImpl->minor);
    }

    // Step 3: Compare PATCH version
    if (m_pImpl->patch != other.m_pImpl->patch)
    {
        return static_cast<int>(m_pImpl->patch) - static_cast<int>(other.m_pImpl->patch);
    }

    // Step 4: Pre-release comparison
    // A version without pre-release has HIGHER precedence than one with pre-release
    const bool thisHasPreRelease = hasPreRelease();
    const bool otherHasPreRelease = other.hasPreRelease();

    if (!thisHasPreRelease && !otherHasPreRelease)
    {
        // Both are stable releases with same M.M.P - they are equal
        return 0;
    }

    if (!thisHasPreRelease && otherHasPreRelease)
    {
        // this (stable) > other (pre-release)
        return 1;
    }

    if (thisHasPreRelease && !otherHasPreRelease)
    {
        // this (pre-release) < other (stable)
        return -1;
    }

    // Both have pre-release identifiers - compare them
    const auto& thisIds = m_pImpl->preReleaseIds;
    const auto& otherIds = other.m_pImpl->preReleaseIds;

    const std::size_t minSize = std::min(thisIds.size(), otherIds.size());

    for (std::size_t i = 0; i < minSize; ++i)
    {
        int cmp = compareIdentifiers(thisIds[i], otherIds[i]);
        if (cmp != 0)
        {
            return cmp;
        }
    }

    // All compared identifiers are equal
    // The version with more identifiers has higher precedence
    if (thisIds.size() != otherIds.size())
    {
        return static_cast<int>(thisIds.size()) - static_cast<int>(otherIds.size());
    }

    // Completely equal (build metadata is ignored for precedence)
    return 0;
}

auto Version::satisfies(const VersionConstraint& constraint) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return constraint.isSatisfiedBy(*this);
}

// ============================================
// Comparison Operators
// ============================================

auto Version::operator==(const Version& other) const noexcept -> bool
{
    return compare(other) == 0;
}

auto Version::operator!=(const Version& other) const noexcept -> bool
{
    return compare(other) != 0;
}

auto Version::operator<(const Version& other) const noexcept -> bool
{
    return compare(other) < 0;
}

auto Version::operator<=(const Version& other) const noexcept -> bool
{
    return compare(other) <= 0;
}

auto Version::operator>(const Version& other) const noexcept -> bool
{
    return compare(other) > 0;
}

auto Version::operator>=(const Version& other) const noexcept -> bool
{
    return compare(other) >= 0;
}

auto Version::operator<=>(const Version& other) const noexcept -> std::strong_ordering
{
    int cmp = compare(other);
    if (cmp < 0)
        return std::strong_ordering::less;
    if (cmp > 0)
        return std::strong_ordering::greater;
    return std::strong_ordering::equal;
}

// ============================================
// String Conversion
// ============================================

auto Version::toString() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::ostringstream oss;
    oss << m_pImpl->major << "." << m_pImpl->minor << "." << m_pImpl->patch;

    if (!m_pImpl->preRelease.empty())
    {
        oss << "-" << m_pImpl->preRelease;
    }

    if (!m_pImpl->buildMetadata.empty())
    {
        oss << "+" << m_pImpl->buildMetadata;
    }

    return oss.str();
}

auto Version::toCoreString() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::ostringstream oss;
    oss << m_pImpl->major << "." << m_pImpl->minor << "." << m_pImpl->patch;
    return oss.str();
}

Version::operator std::string() const
{
    return toString();
}

auto operator<<(std::ostream& os, const Version& version) -> std::ostream&
{
    os << version.toString();
    return os;
}

// ============================================
// Hash Support
// ============================================

auto Version::hash() const noexcept -> std::size_t
{
    // Hash based on precedence-relevant fields only (not build metadata)
    std::size_t h = 0;

    // Combine major, minor, patch
    h ^= std::hash<std::uint32_t>{}(m_pImpl->major) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<std::uint32_t>{}(m_pImpl->minor) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<std::uint32_t>{}(m_pImpl->patch) + 0x9e3779b9 + (h << 6) + (h >> 2);

    // Include pre-release in hash
    h ^= std::hash<std::string>{}(m_pImpl->preRelease) + 0x9e3779b9 + (h << 6) + (h >> 2);

    return h;
}

// ============================================
// Private Helper Functions
// ============================================

auto Version::validatePreReleaseIdentifier(std::string_view identifier) -> void
{
    if (identifier.empty())
    {
        throw VersionException("Pre-release identifier cannot be empty");
    }

    // Check valid characters: [0-9A-Za-z-]
    for (char c : identifier)
    {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-')
        {
            throw VersionException("Invalid character in pre-release identifier: " +
                                   std::string(1, c));
        }
    }

    // If purely numeric, must not have leading zeros (unless it's just "0")
    if (isNumeric(identifier) && identifier.size() > 1 && identifier[0] == '0')
    {
        throw VersionException("Numeric pre-release identifier cannot have leading zeros: " +
                               std::string(identifier));
    }
}

auto Version::validateBuildMetadataIdentifier(std::string_view identifier) -> void
{
    if (identifier.empty())
    {
        throw VersionException("Build metadata identifier cannot be empty");
    }

    // Check valid characters: [0-9A-Za-z-]
    for (char c : identifier)
    {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-')
        {
            throw VersionException("Invalid character in build metadata identifier: " +
                                   std::string(1, c));
        }
    }

    // Note: Build metadata allows leading zeros (unlike pre-release numeric identifiers)
}

auto Version::split(std::string_view str, char delimiter) -> IdentifierList
{
    IdentifierList result;

    if (str.empty())
    {
        return result;
    }

    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    while (end != std::string_view::npos)
    {
        result.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    result.emplace_back(str.substr(start));

    return result;
}

auto Version::isNumeric(std::string_view str) noexcept -> bool
{
    if (str.empty())
    {
        return false;
    }

    return std::all_of(str.begin(), str.end(), [](char c)
    {
        return std::isdigit(static_cast<unsigned char>(c));
    });
}

auto Version::compareIdentifiers(std::string_view a, std::string_view b) noexcept -> int
{
    const bool aIsNumeric = isNumeric(a);
    const bool bIsNumeric = isNumeric(b);

    // Rule: Numeric identifiers always have lower precedence than alphanumeric identifiers
    if (aIsNumeric && !bIsNumeric)
    {
        return -1;  // numeric < alphanumeric
    }
    if (!aIsNumeric && bIsNumeric)
    {
        return 1;   // alphanumeric > numeric
    }

    if (aIsNumeric && bIsNumeric)
    {
        // Compare as integers
        std::uint64_t numA = 0;
        std::uint64_t numB = 0;

        std::from_chars(a.data(), a.data() + a.size(), numA);
        std::from_chars(b.data(), b.data() + b.size(), numB);

        if (numA < numB)
            return -1;
        if (numA > numB)
            return 1;
        return 0;
    }

    // Both are alphanumeric - compare lexicographically (ASCII sort)
    const int cmp = a.compare(b);
    if (cmp < 0)
        return -1;
    if (cmp > 0)
        return 1;
    return 0;
}

}  // namespace com::github::doevelopper::atlassians::semver
