/*******************************************************************
 * @file   VersionConstraint.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Version constraint implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/semver/VersionConstraint.hpp>

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace com::github::doevelopper::atlassians::semver
{

// ============================================
// Logger Definitions
// ============================================

log4cxx::LoggerPtr VersionComparator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.semver.VersionComparator"));

log4cxx::LoggerPtr VersionRange::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.semver.VersionRange"));

log4cxx::LoggerPtr VersionConstraint::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.semver.VersionConstraint"));

// ============================================
// VersionComparator Implementation
// ============================================

struct VersionComparator::Impl
{
    ComparisonOperator op = ComparisonOperator::Equal;
    Version version;

    Impl() noexcept = default;

    Impl(ComparisonOperator o, const Version& v)
        : op(o)
        , version(v)
    {
    }

    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

VersionComparator::VersionComparator() noexcept
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionComparator::VersionComparator(ComparisonOperator op, const Version& version)
    : m_pImpl(std::make_unique<Impl>(op, version))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionComparator::VersionComparator(const VersionComparator& other)
    : m_pImpl(std::make_unique<Impl>(*other.m_pImpl))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto VersionComparator::operator=(const VersionComparator& other) -> VersionComparator&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

VersionComparator::VersionComparator(VersionComparator&& other) noexcept = default;

auto VersionComparator::operator=(VersionComparator&& other) noexcept -> VersionComparator& = default;

VersionComparator::~VersionComparator() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto VersionComparator::isSatisfiedBy(const Version& version) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    const int cmp = version.compare(m_pImpl->version);

    switch (m_pImpl->op)
    {
        case ComparisonOperator::Equal:
            return cmp == 0;
        case ComparisonOperator::NotEqual:
            return cmp != 0;
        case ComparisonOperator::GreaterThan:
            return cmp > 0;
        case ComparisonOperator::GreaterOrEqual:
            return cmp >= 0;
        case ComparisonOperator::LessThan:
            return cmp < 0;
        case ComparisonOperator::LessOrEqual:
            return cmp <= 0;
        default:
            return false;
    }
}

auto VersionComparator::getOperator() const noexcept -> ComparisonOperator
{
    return m_pImpl->op;
}

auto VersionComparator::getVersion() const noexcept -> const Version&
{
    return m_pImpl->version;
}

auto VersionComparator::toString() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::string opStr;
    switch (m_pImpl->op)
    {
        case ComparisonOperator::Equal:
            opStr = "=";
            break;
        case ComparisonOperator::NotEqual:
            opStr = "!=";
            break;
        case ComparisonOperator::GreaterThan:
            opStr = ">";
            break;
        case ComparisonOperator::GreaterOrEqual:
            opStr = ">=";
            break;
        case ComparisonOperator::LessThan:
            opStr = "<";
            break;
        case ComparisonOperator::LessOrEqual:
            opStr = "<=";
            break;
    }

    return opStr + m_pImpl->version.toString();
}

// ============================================
// VersionRange Implementation
// ============================================

struct VersionRange::Impl
{
    ComparatorList comparators;

    Impl() noexcept = default;

    explicit Impl(ComparatorList comps)
        : comparators(std::move(comps))
    {
    }

    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

VersionRange::VersionRange() noexcept
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionRange::VersionRange(ComparatorList comparators)
    : m_pImpl(std::make_unique<Impl>(std::move(comparators)))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionRange::VersionRange(const VersionRange& other)
    : m_pImpl(std::make_unique<Impl>(*other.m_pImpl))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto VersionRange::operator=(const VersionRange& other) -> VersionRange&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

VersionRange::VersionRange(VersionRange&& other) noexcept = default;

auto VersionRange::operator=(VersionRange&& other) noexcept -> VersionRange& = default;

VersionRange::~VersionRange() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto VersionRange::isSatisfiedBy(const Version& version) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Empty range matches all versions
    if (m_pImpl->comparators.empty())
    {
        return true;
    }

    // All comparators must be satisfied (AND operation)
    return std::all_of(m_pImpl->comparators.begin(), m_pImpl->comparators.end(),
                       [&version](const VersionComparator& comp)
                       {
                           return comp.isSatisfiedBy(version);
                       });
}

auto VersionRange::getComparators() const noexcept -> const ComparatorList&
{
    return m_pImpl->comparators;
}

auto VersionRange::isEmpty() const noexcept -> bool
{
    return m_pImpl->comparators.empty();
}

auto VersionRange::toString() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (m_pImpl->comparators.empty())
    {
        return "*";
    }

    std::ostringstream oss;
    bool first = true;
    for (const auto& comp : m_pImpl->comparators)
    {
        if (!first)
        {
            oss << " ";
        }
        oss << comp.toString();
        first = false;
    }

    return oss.str();
}

// ============================================
// VersionConstraint Implementation
// ============================================

struct VersionConstraint::Impl
{
    RangeList ranges;

    Impl() noexcept = default;

    explicit Impl(RangeList r)
        : ranges(std::move(r))
    {
    }

    Impl(const Impl&) = default;
    Impl(Impl&&) noexcept = default;
    Impl& operator=(const Impl&) = default;
    Impl& operator=(Impl&&) noexcept = default;
};

VersionConstraint::VersionConstraint() noexcept
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionConstraint::~VersionConstraint() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

VersionConstraint::VersionConstraint(const VersionConstraint& other)
    : m_pImpl(std::make_unique<Impl>(*other.m_pImpl))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto VersionConstraint::operator=(const VersionConstraint& other) -> VersionConstraint&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

VersionConstraint::VersionConstraint(VersionConstraint&& other) noexcept = default;

auto VersionConstraint::operator=(VersionConstraint&& other) noexcept -> VersionConstraint& = default;

VersionConstraint::VersionConstraint(RangeList ranges)
    : m_pImpl(std::make_unique<Impl>(std::move(ranges)))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// Static Factory Methods
// ============================================

auto VersionConstraint::parse(std::string_view constraintString) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing: " << constraintString);

    auto trimmed = trim(constraintString);

    if (trimmed.empty() || trimmed == "*")
    {
        return any();
    }

    // Split by || for OR operations
    auto orParts = split(trimmed, "||");

    RangeList ranges;
    ranges.reserve(orParts.size());

    for (const auto& orPart : orParts)
    {
        auto trimmedPart = trim(orPart);
        if (!trimmedPart.empty())
        {
            ranges.push_back(parseRange(trimmedPart));
        }
    }

    if (ranges.empty())
    {
        throw ConstraintException("Invalid constraint expression: " + std::string(constraintString));
    }

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::tryParse(std::string_view constraintString) noexcept
    -> std::optional<VersionConstraint>
{
    try
    {
        return parse(constraintString);
    }
    catch (const std::exception&)
    {
        return std::nullopt;
    }
}

auto VersionConstraint::exact(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::Equal, version);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::any() -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Empty range matches all
    RangeList ranges;
    ranges.emplace_back(VersionRange::ComparatorList{});

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::atLeast(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, version);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::greaterThan(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterThan, version);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::atMost(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::LessOrEqual, version);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::lessThan(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::LessThan, version);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::range(const Version& minVersion, const Version& maxVersion)
    -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, minVersion);
    comparators.emplace_back(ComparisonOperator::LessThan, maxVersion);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::caret(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Caret allows changes that do not modify the left-most non-zero element
    Version upperBound(0, 0, 0);

    if (version.major() != 0)
    {
        // ^1.2.3 → >=1.2.3 <2.0.0
        upperBound = Version(version.major() + 1, 0, 0);
    }
    else if (version.minor() != 0)
    {
        // ^0.2.3 → >=0.2.3 <0.3.0
        upperBound = Version(0, version.minor() + 1, 0);
    }
    else
    {
        // ^0.0.3 → >=0.0.3 <0.0.4
        upperBound = Version(0, 0, version.patch() + 1);
    }

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, version);
    comparators.emplace_back(ComparisonOperator::LessThan, upperBound);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

auto VersionConstraint::tilde(const Version& version) -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Tilde allows patch-level changes: ~1.2.3 → >=1.2.3 <1.3.0
    Version upperBound(version.major(), version.minor() + 1, 0);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, version);
    comparators.emplace_back(ComparisonOperator::LessThan, upperBound);

    RangeList ranges;
    ranges.emplace_back(std::move(comparators));

    return VersionConstraint(std::move(ranges));
}

// ============================================
// Constraint Checking
// ============================================

auto VersionConstraint::isSatisfiedBy(const Version& version) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Empty constraints match all versions
    if (m_pImpl->ranges.empty())
    {
        return true;
    }

    // OR operation: at least one range must be satisfied
    return std::any_of(m_pImpl->ranges.begin(), m_pImpl->ranges.end(),
                       [&version](const VersionRange& range)
                       {
                           return range.isSatisfiedBy(version);
                       });
}

auto VersionConstraint::allSatisfy(const std::vector<Version>& versions) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    return std::all_of(versions.begin(), versions.end(),
                       [this](const Version& v)
                       {
                           return isSatisfiedBy(v);
                       });
}

auto VersionConstraint::anySatisfy(const std::vector<Version>& versions) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    return std::any_of(versions.begin(), versions.end(),
                       [this](const Version& v)
                       {
                           return isSatisfiedBy(v);
                       });
}

auto VersionConstraint::filter(const std::vector<Version>& versions) const -> std::vector<Version>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::vector<Version> result;
    result.reserve(versions.size());

    std::copy_if(versions.begin(), versions.end(), std::back_inserter(result),
                 [this](const Version& v)
                 {
                     return isSatisfiedBy(v);
                 });

    return result;
}

auto VersionConstraint::maxSatisfying(const std::vector<Version>& versions) const
    -> std::optional<Version>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto filtered = filter(versions);

    if (filtered.empty())
    {
        return std::nullopt;
    }

    return *std::max_element(filtered.begin(), filtered.end());
}

auto VersionConstraint::minSatisfying(const std::vector<Version>& versions) const
    -> std::optional<Version>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto filtered = filter(versions);

    if (filtered.empty())
    {
        return std::nullopt;
    }

    return *std::min_element(filtered.begin(), filtered.end());
}

// ============================================
// Constraint Operations
// ============================================

auto VersionConstraint::intersect(const VersionConstraint& other) const -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Intersection: combine all ranges from both constraints
    // Each combination is a new range with all comparators
    RangeList newRanges;

    for (const auto& thisRange : m_pImpl->ranges)
    {
        for (const auto& otherRange : other.m_pImpl->ranges)
        {
            VersionRange::ComparatorList combined;

            // Add all comparators from both ranges
            for (const auto& comp : thisRange.getComparators())
            {
                combined.push_back(comp);
            }
            for (const auto& comp : otherRange.getComparators())
            {
                combined.push_back(comp);
            }

            newRanges.emplace_back(std::move(combined));
        }
    }

    return VersionConstraint(std::move(newRanges));
}

auto VersionConstraint::unite(const VersionConstraint& other) const -> VersionConstraint
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Union: add all ranges from both constraints
    RangeList newRanges = m_pImpl->ranges;
    newRanges.insert(newRanges.end(),
                     other.m_pImpl->ranges.begin(),
                     other.m_pImpl->ranges.end());

    return VersionConstraint(std::move(newRanges));
}

auto VersionConstraint::isEmpty() const noexcept -> bool
{
    return m_pImpl->ranges.empty();
}

auto VersionConstraint::isAny() const noexcept -> bool
{
    if (m_pImpl->ranges.empty())
    {
        return true;
    }

    // Check if any range matches all (empty comparators)
    return std::any_of(m_pImpl->ranges.begin(), m_pImpl->ranges.end(),
                       [](const VersionRange& range)
                       {
                           return range.isEmpty();
                       });
}

// ============================================
// String Conversion
// ============================================

auto VersionConstraint::toString() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (m_pImpl->ranges.empty() || isAny())
    {
        return "*";
    }

    std::ostringstream oss;
    bool first = true;

    for (const auto& range : m_pImpl->ranges)
    {
        if (!first)
        {
            oss << " || ";
        }
        oss << range.toString();
        first = false;
    }

    return oss.str();
}

auto operator<<(std::ostream& os, const VersionConstraint& constraint) -> std::ostream&
{
    os << constraint.toString();
    return os;
}

// ============================================
// Private Helper Methods
// ============================================

auto VersionConstraint::parseRange(std::string_view rangeString) -> VersionRange
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing range: " << rangeString);

    auto trimmed = trim(rangeString);

    if (trimmed.empty() || trimmed == "*")
    {
        return VersionRange(VersionRange::ComparatorList{});
    }

    // Check for caret range: ^1.2.3
    if (!trimmed.empty() && trimmed[0] == '^')
    {
        return parseCaretRange(trimmed.substr(1));
    }

    // Check for tilde range: ~1.2.3
    if (!trimmed.empty() && trimmed[0] == '~')
    {
        return parseTildeRange(trimmed.substr(1));
    }

    // Check for wildcard: 1.2.*, 1.*, *
    if (trimmed.find('*') != std::string_view::npos)
    {
        return parseWildcard(trimmed);
    }

    // Parse space-separated comparators (AND operation)
    VersionRange::ComparatorList comparators;

    // Split by spaces for multiple comparators
    std::string_view remaining = trimmed;
    while (!remaining.empty())
    {
        // Skip whitespace
        while (!remaining.empty() && std::isspace(static_cast<unsigned char>(remaining[0])))
        {
            remaining.remove_prefix(1);
        }

        if (remaining.empty())
        {
            break;
        }

        // Find the end of this comparator
        std::size_t end = 0;
        bool inVersion = false;

        for (std::size_t i = 0; i < remaining.size(); ++i)
        {
            char c = remaining[i];

            if (c == '>' || c == '<' || c == '=' || c == '!')
            {
                if (inVersion)
                {
                    break;
                }
            }
            else if (std::isspace(static_cast<unsigned char>(c)))
            {
                if (inVersion)
                {
                    break;
                }
            }
            else
            {
                inVersion = true;
            }

            end = i + 1;
        }

        auto comparatorStr = remaining.substr(0, end);
        remaining.remove_prefix(end);

        if (!comparatorStr.empty())
        {
            comparators.push_back(parseComparator(comparatorStr));
        }
    }

    return VersionRange(std::move(comparators));
}

auto VersionConstraint::parseComparator(std::string_view comparatorString) -> VersionComparator
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing comparator: " << comparatorString);

    auto trimmed = trim(comparatorString);

    if (trimmed.empty())
    {
        throw ConstraintException("Empty comparator");
    }

    ComparisonOperator op = ComparisonOperator::Equal;
    std::string_view versionStr = trimmed;

    // Parse operator
    if (trimmed.size() >= 2 && trimmed.substr(0, 2) == ">=")
    {
        op = ComparisonOperator::GreaterOrEqual;
        versionStr = trimmed.substr(2);
    }
    else if (trimmed.size() >= 2 && trimmed.substr(0, 2) == "<=")
    {
        op = ComparisonOperator::LessOrEqual;
        versionStr = trimmed.substr(2);
    }
    else if (trimmed.size() >= 2 && trimmed.substr(0, 2) == "!=")
    {
        op = ComparisonOperator::NotEqual;
        versionStr = trimmed.substr(2);
    }
    else if (trimmed.size() >= 2 && trimmed.substr(0, 2) == "==")
    {
        op = ComparisonOperator::Equal;
        versionStr = trimmed.substr(2);
    }
    else if (!trimmed.empty() && trimmed[0] == '>')
    {
        op = ComparisonOperator::GreaterThan;
        versionStr = trimmed.substr(1);
    }
    else if (!trimmed.empty() && trimmed[0] == '<')
    {
        op = ComparisonOperator::LessThan;
        versionStr = trimmed.substr(1);
    }
    else if (!trimmed.empty() && trimmed[0] == '=')
    {
        op = ComparisonOperator::Equal;
        versionStr = trimmed.substr(1);
    }

    // Trim whitespace from version string
    versionStr = trim(versionStr);

    if (versionStr.empty())
    {
        throw ConstraintException("Missing version in comparator: " + std::string(comparatorString));
    }

    Version version = Version::parse(versionStr);

    return VersionComparator(op, version);
}

auto VersionConstraint::parseCaretRange(std::string_view versionString) -> VersionRange
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing caret range: " << versionString);

    auto trimmed = trim(versionString);
    Version version = Version::parse(trimmed);

    // Determine upper bound based on caret semantics
    Version upperBound(0, 0, 0);

    if (version.major() != 0)
    {
        upperBound = Version(version.major() + 1, 0, 0);
    }
    else if (version.minor() != 0)
    {
        upperBound = Version(0, version.minor() + 1, 0);
    }
    else
    {
        upperBound = Version(0, 0, version.patch() + 1);
    }

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, version);
    comparators.emplace_back(ComparisonOperator::LessThan, upperBound);

    return VersionRange(std::move(comparators));
}

auto VersionConstraint::parseTildeRange(std::string_view versionString) -> VersionRange
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing tilde range: " << versionString);

    auto trimmed = trim(versionString);

    // Handle partial versions: ~1.2 means >=1.2.0 <1.3.0
    std::string versionStr(trimmed);

    // Count dots to determine if we have partial version
    auto dotCount = std::count(trimmed.begin(), trimmed.end(), '.');

    if (dotCount == 0)
    {
        // ~1 means >=1.0.0 <2.0.0
        versionStr += ".0.0";
    }
    else if (dotCount == 1)
    {
        // ~1.2 means >=1.2.0 <1.3.0
        versionStr += ".0";
    }

    Version version = Version::parse(versionStr);
    Version upperBound(version.major(), version.minor() + 1, 0);

    VersionRange::ComparatorList comparators;
    comparators.emplace_back(ComparisonOperator::GreaterOrEqual, version);
    comparators.emplace_back(ComparisonOperator::LessThan, upperBound);

    return VersionRange(std::move(comparators));
}

auto VersionConstraint::parseWildcard(std::string_view wildcardString) -> VersionRange
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing wildcard: " << wildcardString);

    auto trimmed = trim(wildcardString);

    // Handle different wildcard patterns
    if (trimmed == "*" || trimmed == "*.*.*")
    {
        // Match all versions
        return VersionRange(VersionRange::ComparatorList{});
    }

    // Parse patterns like "1.*", "1.2.*"
    std::string pattern(trimmed);

    // Find the wildcard position
    auto wildcardPos = pattern.find('*');
    if (wildcardPos == std::string::npos)
    {
        throw ConstraintException("Invalid wildcard pattern: " + pattern);
    }

    // Get the prefix before the wildcard
    std::string prefix = pattern.substr(0, wildcardPos);

    // Remove trailing dot if present
    if (!prefix.empty() && prefix.back() == '.')
    {
        prefix.pop_back();
    }

    if (prefix.empty())
    {
        // Just "*" - match all
        return VersionRange(VersionRange::ComparatorList{});
    }

    // Count dots in prefix
    auto dotCount = std::count(prefix.begin(), prefix.end(), '.');

    VersionRange::ComparatorList comparators;

    if (dotCount == 0)
    {
        // "1.*" → >=1.0.0 <2.0.0
        std::uint32_t major = std::stoul(prefix);
        comparators.emplace_back(ComparisonOperator::GreaterOrEqual,
                                 Version(major, 0, 0));
        comparators.emplace_back(ComparisonOperator::LessThan,
                                 Version(major + 1, 0, 0));
    }
    else if (dotCount == 1)
    {
        // "1.2.*" → >=1.2.0 <1.3.0
        auto dotPos = prefix.find('.');
        std::uint32_t major = std::stoul(prefix.substr(0, dotPos));
        std::uint32_t minor = std::stoul(prefix.substr(dotPos + 1));

        comparators.emplace_back(ComparisonOperator::GreaterOrEqual,
                                 Version(major, minor, 0));
        comparators.emplace_back(ComparisonOperator::LessThan,
                                 Version(major, minor + 1, 0));
    }
    else
    {
        throw ConstraintException("Invalid wildcard pattern: " + pattern);
    }

    return VersionRange(std::move(comparators));
}

auto VersionConstraint::trim(std::string_view str) -> std::string_view
{
    const auto start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string_view::npos)
    {
        return {};
    }

    const auto end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}

auto VersionConstraint::split(std::string_view str, std::string_view delimiter)
    -> std::vector<std::string>
{
    std::vector<std::string> result;

    std::size_t start = 0;
    std::size_t end = str.find(delimiter);

    while (end != std::string_view::npos)
    {
        result.emplace_back(str.substr(start, end - start));
        start = end + delimiter.size();
        end = str.find(delimiter, start);
    }

    result.emplace_back(str.substr(start));

    return result;
}

}  // namespace com::github::doevelopper::atlassians::semver
