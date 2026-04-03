/*******************************************************************
 * @file   VersionConstraint.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Version constraint/range specification for dependency management
 *
 * Supports npm/Cargo style version constraint expressions:
 * - Exact: 1.2.3, =1.2.3
 * - Comparison: >1.2.3, >=1.2.3, <2.0.0, <=1.9.9
 * - Caret range: ^1.2.3 (>=1.2.3 <2.0.0)
 * - Tilde range: ~1.2.3 (>=1.2.3 <1.3.0)
 * - Wildcard: 1.2.*, 1.*, *
 * - Compound: >=1.2.0 <2.0.0, >=1.0.0 || >=2.0.0
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINT_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINT_HPP

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/semver/Version.hpp>

namespace com::github::doevelopper::atlassians::semver
{

/**
 * @brief Exception class for constraint parsing errors
 */
class ConstraintException : public std::runtime_error
{
public:
    explicit ConstraintException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    explicit ConstraintException(const char* message)
        : std::runtime_error(message)
    {
    }
};

/**
 * @brief Comparison operator for version constraints
 */
enum class ComparisonOperator : std::uint8_t
{
    Equal,          ///< = or == (exact match)
    NotEqual,       ///< != (not equal)
    GreaterThan,    ///< >
    GreaterOrEqual, ///< >=
    LessThan,       ///< <
    LessOrEqual     ///< <=
};

/**
 * @brief A single constraint condition (operator + version)
 */
class VersionComparator
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    /**
     * @brief Default constructor
     */
    VersionComparator() noexcept;

    /**
     * @brief Construct with operator and version
     * @param op Comparison operator
     * @param version Target version
     */
    VersionComparator(ComparisonOperator op, const Version& version);

    /**
     * @brief Copy constructor
     */
    VersionComparator(const VersionComparator& other);

    /**
     * @brief Copy assignment operator
     */
    auto operator=(const VersionComparator& other) -> VersionComparator&;

    /**
     * @brief Move constructor
     */
    VersionComparator(VersionComparator&& other) noexcept;

    /**
     * @brief Move assignment operator
     */
    auto operator=(VersionComparator&& other) noexcept -> VersionComparator&;

    /**
     * @brief Destructor
     */
    ~VersionComparator() noexcept;

    /**
     * @brief Check if a version satisfies this comparator
     * @param version Version to check
     * @return true if version satisfies the comparator
     */
    [[nodiscard]] auto isSatisfiedBy(const Version& version) const -> bool;

    /**
     * @brief Get the comparison operator
     * @return Comparison operator
     */
    [[nodiscard]] auto getOperator() const noexcept -> ComparisonOperator;

    /**
     * @brief Get the target version
     * @return Target version
     */
    [[nodiscard]] auto getVersion() const noexcept -> const Version&;

    /**
     * @brief Convert to string representation
     * @return String representation (e.g., ">=1.2.3")
     */
    [[nodiscard]] auto toString() const -> std::string;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};

/**
 * @brief A version range representing a conjunction of comparators (AND)
 *
 * All comparators in the range must be satisfied.
 */
class VersionRange
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using ComparatorList = std::vector<VersionComparator>;

    /**
     * @brief Default constructor (matches all versions)
     */
    VersionRange() noexcept;

    /**
     * @brief Construct with a list of comparators
     * @param comparators List of comparators (all must match)
     */
    explicit VersionRange(ComparatorList comparators);

    /**
     * @brief Copy constructor
     */
    VersionRange(const VersionRange& other);

    /**
     * @brief Copy assignment
     */
    auto operator=(const VersionRange& other) -> VersionRange&;

    /**
     * @brief Move constructor
     */
    VersionRange(VersionRange&& other) noexcept;

    /**
     * @brief Move assignment
     */
    auto operator=(VersionRange&& other) noexcept -> VersionRange&;

    /**
     * @brief Destructor
     */
    ~VersionRange() noexcept;

    /**
     * @brief Check if a version satisfies all comparators in this range
     * @param version Version to check
     * @return true if version satisfies all comparators
     */
    [[nodiscard]] auto isSatisfiedBy(const Version& version) const -> bool;

    /**
     * @brief Get the comparators
     * @return List of comparators
     */
    [[nodiscard]] auto getComparators() const noexcept -> const ComparatorList&;

    /**
     * @brief Check if this range is empty (matches all)
     * @return true if empty
     */
    [[nodiscard]] auto isEmpty() const noexcept -> bool;

    /**
     * @brief Convert to string representation
     * @return String representation
     */
    [[nodiscard]] auto toString() const -> std::string;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};

/**
 * @brief Version constraint for dependency management
 *
 * Supports complex version constraint expressions including:
 * - Exact match: 1.2.3, =1.2.3
 * - Comparison: >1.2.3, >=1.2.3, <2.0.0, <=1.9.9
 * - Caret range: ^1.2.3 means >=1.2.3 <2.0.0
 *   - ^1.2.3 → >=1.2.3 <2.0.0
 *   - ^0.2.3 → >=0.2.3 <0.3.0
 *   - ^0.0.3 → >=0.0.3 <0.0.4
 * - Tilde range: ~1.2.3 means >=1.2.3 <1.3.0
 *   - ~1.2.3 → >=1.2.3 <1.3.0
 *   - ~1.2 → >=1.2.0 <1.3.0
 * - Wildcard: 1.2.*, 1.*, *
 * - Compound (AND): >=1.2.0 <2.0.0
 * - Compound (OR): >=1.0.0 <1.5.0 || >=2.0.0
 *
 * @example
 * @code
 * // Parse constraint expressions
 * auto c1 = VersionConstraint::parse("^1.2.3");
 * auto c2 = VersionConstraint::parse(">=1.0.0 <2.0.0");
 * auto c3 = VersionConstraint::parse("~1.2.3");
 * auto c4 = VersionConstraint::parse("1.2.*");
 *
 * // Check if versions satisfy constraints
 * Version v = Version::parse("1.5.0");
 * if (c1.isSatisfiedBy(v)) { ... }
 *
 * // Use factory methods
 * auto exact = VersionConstraint::exact(Version::parse("1.2.3"));
 * auto range = VersionConstraint::range(
 *     Version::parse("1.0.0"), Version::parse("2.0.0"));
 * @endcode
 */
class VersionConstraint
{
    LOG4CXX_DECLARE_STATIC_LOGGER

public:
    using RangeList = std::vector<VersionRange>;

    // ============================================
    // Rule of Five
    // ============================================

    /**
     * @brief Default constructor (matches all versions)
     */
    VersionConstraint() noexcept;

    /**
     * @brief Destructor
     */
    ~VersionConstraint() noexcept;

    /**
     * @brief Copy constructor
     */
    VersionConstraint(const VersionConstraint& other);

    /**
     * @brief Copy assignment
     */
    auto operator=(const VersionConstraint& other) -> VersionConstraint&;

    /**
     * @brief Move constructor
     */
    VersionConstraint(VersionConstraint&& other) noexcept;

    /**
     * @brief Move assignment
     */
    auto operator=(VersionConstraint&& other) noexcept -> VersionConstraint&;

    // ============================================
    // Static Factory Methods
    // ============================================

    /**
     * @brief Parse a constraint expression string
     * @param constraintString The constraint expression to parse
     * @return Parsed VersionConstraint
     * @throws ConstraintException if parsing fails
     *
     * Supported formats:
     * - "1.2.3" or "=1.2.3" (exact)
     * - ">1.2.3", ">=1.2.3", "<1.2.3", "<=1.2.3" (comparison)
     * - "^1.2.3" (caret range)
     * - "~1.2.3" (tilde range)
     * - "1.2.*", "1.*", "*" (wildcard)
     * - ">=1.0.0 <2.0.0" (AND compound)
     * - ">=1.0.0 || >=2.0.0" (OR compound)
     */
    [[nodiscard]] static auto parse(std::string_view constraintString) -> VersionConstraint;

    /**
     * @brief Try to parse a constraint expression, returning nullopt on failure
     * @param constraintString The constraint expression to parse
     * @return Optional VersionConstraint
     */
    [[nodiscard]] static auto tryParse(std::string_view constraintString) noexcept
        -> std::optional<VersionConstraint>;

    /**
     * @brief Create an exact version constraint
     * @param version The exact version to match
     * @return VersionConstraint matching only the exact version
     */
    [[nodiscard]] static auto exact(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a constraint matching all versions
     * @return VersionConstraint matching any version
     */
    [[nodiscard]] static auto any() -> VersionConstraint;

    /**
     * @brief Create a constraint for >= version
     * @param version Minimum version (inclusive)
     * @return VersionConstraint for >= version
     */
    [[nodiscard]] static auto atLeast(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a constraint for > version
     * @param version Minimum version (exclusive)
     * @return VersionConstraint for > version
     */
    [[nodiscard]] static auto greaterThan(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a constraint for <= version
     * @param version Maximum version (inclusive)
     * @return VersionConstraint for <= version
     */
    [[nodiscard]] static auto atMost(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a constraint for < version
     * @param version Maximum version (exclusive)
     * @return VersionConstraint for < version
     */
    [[nodiscard]] static auto lessThan(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a range constraint [minVersion, maxVersion)
     * @param minVersion Minimum version (inclusive)
     * @param maxVersion Maximum version (exclusive)
     * @return VersionConstraint for the range
     */
    [[nodiscard]] static auto range(const Version& minVersion,
                                    const Version& maxVersion) -> VersionConstraint;

    /**
     * @brief Create a caret range constraint
     *
     * Allows changes that do not modify the left-most non-zero element:
     * - ^1.2.3 → >=1.2.3 <2.0.0
     * - ^0.2.3 → >=0.2.3 <0.3.0
     * - ^0.0.3 → >=0.0.3 <0.0.4
     *
     * @param version Base version
     * @return VersionConstraint for the caret range
     */
    [[nodiscard]] static auto caret(const Version& version) -> VersionConstraint;

    /**
     * @brief Create a tilde range constraint
     *
     * Allows patch-level changes:
     * - ~1.2.3 → >=1.2.3 <1.3.0
     * - ~1.2 → >=1.2.0 <1.3.0
     *
     * @param version Base version
     * @return VersionConstraint for the tilde range
     */
    [[nodiscard]] static auto tilde(const Version& version) -> VersionConstraint;

    // ============================================
    // Constraint Checking
    // ============================================

    /**
     * @brief Check if a version satisfies this constraint
     * @param version Version to check
     * @return true if the version satisfies the constraint
     */
    [[nodiscard]] auto isSatisfiedBy(const Version& version) const -> bool;

    /**
     * @brief Check if all versions in a range satisfy this constraint
     * @param versions List of versions to check
     * @return true if all versions satisfy the constraint
     */
    [[nodiscard]] auto allSatisfy(const std::vector<Version>& versions) const -> bool;

    /**
     * @brief Check if any version in a range satisfies this constraint
     * @param versions List of versions to check
     * @return true if any version satisfies the constraint
     */
    [[nodiscard]] auto anySatisfy(const std::vector<Version>& versions) const -> bool;

    /**
     * @brief Filter versions that satisfy this constraint
     * @param versions List of versions to filter
     * @return Vector of versions that satisfy the constraint
     */
    [[nodiscard]] auto filter(const std::vector<Version>& versions) const -> std::vector<Version>;

    /**
     * @brief Find the maximum version that satisfies this constraint
     * @param versions List of versions to search
     * @return Optional maximum satisfying version
     */
    [[nodiscard]] auto maxSatisfying(const std::vector<Version>& versions) const
        -> std::optional<Version>;

    /**
     * @brief Find the minimum version that satisfies this constraint
     * @param versions List of versions to search
     * @return Optional minimum satisfying version
     */
    [[nodiscard]] auto minSatisfying(const std::vector<Version>& versions) const
        -> std::optional<Version>;

    // ============================================
    // Constraint Operations
    // ============================================

    /**
     * @brief Combine this constraint with another (AND operation)
     * @param other Other constraint
     * @return New constraint that is the intersection
     */
    [[nodiscard]] auto intersect(const VersionConstraint& other) const -> VersionConstraint;

    /**
     * @brief Combine this constraint with another (OR operation)
     * @param other Other constraint
     * @return New constraint that is the union
     */
    [[nodiscard]] auto unite(const VersionConstraint& other) const -> VersionConstraint;

    /**
     * @brief Check if this constraint is empty (matches nothing)
     * @return true if no version can satisfy this constraint
     */
    [[nodiscard]] auto isEmpty() const noexcept -> bool;

    /**
     * @brief Check if this constraint matches all versions
     * @return true if any version satisfies this constraint
     */
    [[nodiscard]] auto isAny() const noexcept -> bool;

    // ============================================
    // String Conversion
    // ============================================

    /**
     * @brief Convert to string representation
     * @return String representation of the constraint
     */
    [[nodiscard]] auto toString() const -> std::string;

    /**
     * @brief Stream output operator
     */
    friend auto operator<<(std::ostream& os, const VersionConstraint& constraint)
        -> std::ostream&;

private:
    // ============================================
    // PIMPL Implementation
    // ============================================

    struct Impl;
    std::unique_ptr<Impl> m_pImpl;

    // ============================================
    // Private Constructor
    // ============================================

    /**
     * @brief Construct with a list of ranges (OR of ranges)
     */
    explicit VersionConstraint(RangeList ranges);

    // ============================================
    // Private Helper Methods
    // ============================================

    /**
     * @brief Parse a single range expression (no OR operators)
     */
    [[nodiscard]] static auto parseRange(std::string_view rangeString) -> VersionRange;

    /**
     * @brief Parse a single comparator expression
     */
    [[nodiscard]] static auto parseComparator(std::string_view comparatorString)
        -> VersionComparator;

    /**
     * @brief Parse a caret range expression
     */
    [[nodiscard]] static auto parseCaretRange(std::string_view versionString) -> VersionRange;

    /**
     * @brief Parse a tilde range expression
     */
    [[nodiscard]] static auto parseTildeRange(std::string_view versionString) -> VersionRange;

    /**
     * @brief Parse a wildcard expression
     */
    [[nodiscard]] static auto parseWildcard(std::string_view wildcardString) -> VersionRange;

    /**
     * @brief Trim whitespace from string
     */
    [[nodiscard]] static auto trim(std::string_view str) -> std::string_view;

    /**
     * @brief Split string by delimiter
     */
    [[nodiscard]] static auto split(std::string_view str, std::string_view delimiter)
        -> std::vector<std::string>;
};

}  // namespace com::github::doevelopper::atlassians::semver

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_SEMVER_VERSIONCONSTRAINT_HPP
