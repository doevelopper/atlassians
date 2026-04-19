/**
 * @file   License.cpp
 * @brief  License entity class implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/License.hpp>

#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr License::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.License"));

// ============================================
// License Implementation Structure
// ============================================

struct License::Impl
{
    std::string licenseKey;
    std::string productId;
    TimePoint expirationDate{};
    TimePoint issueDate{};
    LicenseType type{LicenseType::Unknown};
    FeatureList enabledFeatures;
    std::optional<int> maxConcurrentUsers;
    std::optional<std::string> hardwareId;
    std::string digitalSignature;
    std::string licenseeName;
    std::string licenseeEmail;
    std::string version{"1.0"};
    std::vector<std::pair<std::string, std::string>> metadata;

    Impl() = default;

    Impl(const Impl& other) = default;

    Impl(Impl&& other) noexcept = default;

    auto operator=(const Impl& other) -> Impl& = default;

    auto operator=(Impl&& other) noexcept -> Impl& = default;
};

// ============================================
// License Implementation
// ============================================

License::License() noexcept
    : m_pImpl(std::make_unique<Impl>())
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

License::~License() noexcept
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

License::License(const License& other)
    : m_pImpl(std::make_unique<Impl>(*other.m_pImpl))
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto License::operator=(const License& other) -> License&
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        m_pImpl = std::make_unique<Impl>(*other.m_pImpl);
    }
    return *this;
}

License::License(License&& other) noexcept = default;

auto License::operator=(License&& other) noexcept -> License& = default;

License::License(std::unique_ptr<Impl> impl) noexcept
    : m_pImpl(std::move(impl))
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto License::builder() -> LicenseBuilder
{
    return LicenseBuilder();
}

auto License::createEmpty() -> License
{
    return License();
}

auto License::getLicenseKey() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->licenseKey) : std::string_view{};
}

auto License::getProductId() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->productId) : std::string_view{};
}

auto License::getExpirationDate() const noexcept -> TimePoint
{
    return m_pImpl ? m_pImpl->expirationDate : TimePoint{};
}

auto License::getIssueDate() const noexcept -> TimePoint
{
    return m_pImpl ? m_pImpl->issueDate : TimePoint{};
}

auto License::getType() const noexcept -> LicenseType
{
    return m_pImpl ? m_pImpl->type : LicenseType::Unknown;
}

auto License::getEnabledFeatures() const noexcept -> const FeatureList&
{
    static const FeatureList empty;
    return m_pImpl ? m_pImpl->enabledFeatures : empty;
}

auto License::getMaxConcurrentUsers() const noexcept -> std::optional<int>
{
    return m_pImpl ? m_pImpl->maxConcurrentUsers : std::nullopt;
}

auto License::getHardwareId() const noexcept -> std::optional<std::string_view>
{
    if (m_pImpl && m_pImpl->hardwareId.has_value())
    {
        return std::string_view(m_pImpl->hardwareId.value());
    }
    return std::nullopt;
}

auto License::getDigitalSignature() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->digitalSignature) : std::string_view{};
}

auto License::getLicenseeName() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->licenseeName) : std::string_view{};
}

auto License::getLicenseeEmail() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->licenseeEmail) : std::string_view{};
}

auto License::getVersion() const noexcept -> std::string_view
{
    return m_pImpl ? std::string_view(m_pImpl->version) : std::string_view{};
}

auto License::getMetadata() const noexcept -> const std::vector<std::pair<std::string, std::string>>&
{
    static const std::vector<std::pair<std::string, std::string>> empty;
    return m_pImpl ? m_pImpl->metadata : empty;
}

auto License::isValid() const noexcept -> bool
{
    if (!m_pImpl)
    {
        return false;
    }

    // Check required fields
    if (m_pImpl->licenseKey.empty() || m_pImpl->productId.empty())
    {
        return false;
    }

    // Check type
    if (m_pImpl->type == LicenseType::Unknown)
    {
        return false;
    }

    // Check not expired
    return !isExpired();
}

auto License::isExpired() const noexcept -> bool
{
    if (!m_pImpl)
    {
        return true;
    }

    // Check if expiration date is set
    if (m_pImpl->expirationDate == TimePoint{})
    {
        return false; // No expiration date means perpetual
    }

    return std::chrono::system_clock::now() > m_pImpl->expirationDate;
}

auto License::hasFeature(std::string_view featureName) const noexcept -> bool
{
    if (!m_pImpl)
    {
        return false;
    }

    return std::find(m_pImpl->enabledFeatures.begin(),
                     m_pImpl->enabledFeatures.end(),
                     featureName) != m_pImpl->enabledFeatures.end();
}

auto License::isHardwareBound() const noexcept -> bool
{
    return m_pImpl && m_pImpl->hardwareId.has_value();
}

auto License::hasConcurrencyLimit() const noexcept -> bool
{
    return m_pImpl && m_pImpl->maxConcurrentUsers.has_value();
}

auto License::getDaysUntilExpiration() const noexcept -> int
{
    if (!m_pImpl || m_pImpl->expirationDate == TimePoint{})
    {
        return std::numeric_limits<int>::max(); // Perpetual
    }

    auto now      = std::chrono::system_clock::now();
    auto duration = m_pImpl->expirationDate - now;
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(duration).count() / 24);
}

auto License::toString() const -> std::string
{
    if (!m_pImpl)
    {
        return "License[invalid]";
    }

    std::ostringstream oss;
    oss << "License[";
    oss << "key=" << m_pImpl->licenseKey.substr(0, 8) << "...";
    oss << ", product=" << m_pImpl->productId;
    oss << ", type=" << licenseTypeToString(m_pImpl->type);
    oss << ", features=" << m_pImpl->enabledFeatures.size();

    if (m_pImpl->maxConcurrentUsers.has_value())
    {
        oss << ", maxUsers=" << m_pImpl->maxConcurrentUsers.value();
    }

    if (m_pImpl->hardwareId.has_value())
    {
        oss << ", hwBound=true";
    }

    oss << ", daysRemaining=" << getDaysUntilExpiration();
    oss << "]";

    return oss.str();
}

auto License::operator==(const License& other) const noexcept -> bool
{
    if (!m_pImpl || !other.m_pImpl)
    {
        return !m_pImpl && !other.m_pImpl;
    }
    return m_pImpl->licenseKey == other.m_pImpl->licenseKey;
}

auto License::operator!=(const License& other) const noexcept -> bool
{
    return !(*this == other);
}

// ============================================
// LicenseBuilder Implementation
// ============================================

struct LicenseBuilder::BuilderImpl
{
    std::unique_ptr<License::Impl> licenseImpl;

    BuilderImpl()
        : licenseImpl(std::make_unique<License::Impl>())
    {
    }
};

LicenseBuilder::LicenseBuilder()
    : m_pImpl(std::make_unique<BuilderImpl>())
{
}

LicenseBuilder::~LicenseBuilder() noexcept = default;

LicenseBuilder::LicenseBuilder(LicenseBuilder&&) noexcept = default;

LicenseBuilder& LicenseBuilder::operator=(LicenseBuilder&&) noexcept = default;

auto LicenseBuilder::withLicenseKey(std::string_view key) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->licenseKey = std::string(key);
    return *this;
}

auto LicenseBuilder::withProductId(std::string_view productId) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->productId = std::string(productId);
    return *this;
}

auto LicenseBuilder::withExpirationDate(License::TimePoint expirationDate) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->expirationDate = expirationDate;
    return *this;
}

auto LicenseBuilder::expiresInDays(int days) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->expirationDate = std::chrono::system_clock::now() + std::chrono::hours(24 * days);
    return *this;
}

auto LicenseBuilder::withIssueDate(License::TimePoint issueDate) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->issueDate = issueDate;
    return *this;
}

auto LicenseBuilder::issuedNow() -> LicenseBuilder&
{
    m_pImpl->licenseImpl->issueDate = std::chrono::system_clock::now();
    return *this;
}

auto LicenseBuilder::withType(LicenseType type) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->type = type;
    return *this;
}

auto LicenseBuilder::withFeature(std::string_view feature) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->enabledFeatures.emplace_back(feature);
    return *this;
}

auto LicenseBuilder::withFeatures(const std::vector<std::string>& features) -> LicenseBuilder&
{
    for (const auto& feature : features)
    {
        m_pImpl->licenseImpl->enabledFeatures.push_back(feature);
    }
    return *this;
}

auto LicenseBuilder::withMaxConcurrentUsers(int maxUsers) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->maxConcurrentUsers = maxUsers;
    return *this;
}

auto LicenseBuilder::withHardwareId(std::string_view hardwareId) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->hardwareId = std::string(hardwareId);
    return *this;
}

auto LicenseBuilder::withDigitalSignature(std::string_view signature) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->digitalSignature = std::string(signature);
    return *this;
}

auto LicenseBuilder::withLicenseeName(std::string_view name) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->licenseeName = std::string(name);
    return *this;
}

auto LicenseBuilder::withLicenseeEmail(std::string_view email) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->licenseeEmail = std::string(email);
    return *this;
}

auto LicenseBuilder::withVersion(std::string_view version) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->version = std::string(version);
    return *this;
}

auto LicenseBuilder::withMetadata(std::string_view key, std::string_view value) -> LicenseBuilder&
{
    m_pImpl->licenseImpl->metadata.emplace_back(std::string(key), std::string(value));
    return *this;
}

auto LicenseBuilder::build() -> License
{
    // Validate required fields
    if (m_pImpl->licenseImpl->licenseKey.empty())
    {
        throw LicenseInvalidException("License key is required");
    }

    if (m_pImpl->licenseImpl->productId.empty())
    {
        throw LicenseInvalidException("Product ID is required");
    }

    if (m_pImpl->licenseImpl->type == LicenseType::Unknown)
    {
        throw LicenseInvalidException("License type is required");
    }

    // Set default issue date if not set
    if (m_pImpl->licenseImpl->issueDate == License::TimePoint{})
    {
        m_pImpl->licenseImpl->issueDate = std::chrono::system_clock::now();
    }

    return License(std::move(m_pImpl->licenseImpl));
}

auto LicenseBuilder::tryBuild() noexcept -> std::optional<License>
{
    try
    {
        return build();
    }
    catch (const LicenseException&)
    {
        return std::nullopt;
    }
}
