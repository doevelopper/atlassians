/**
 * @file PluginMetadata.cpp
 * @brief Implementation of PluginMetadata class
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/plugin/metadata/PluginMetadata.hpp>

using namespace com::github::doevelopper::atlassians::plugin::metadata;

log4cxx::LoggerPtr PluginMetadata::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.metadata.PluginMetadata"));

namespace
{
    /**
     * @brief Get the current platform identifier.
     */
    [[nodiscard]] auto getCurrentPlatform() noexcept -> std::string_view
    {
#if defined(_WIN32) || defined(_WIN64)
        return "windows";
#elif defined(__APPLE__) && defined(__MACH__)
        return "macos";
#elif defined(__linux__)
        return "linux";
#elif defined(__FreeBSD__)
        return "freebsd";
#else
        return "unknown";
#endif
    }
}

// ============================================
// Constructors and Destructor
// ============================================

PluginMetadata::PluginMetadata() noexcept
    : buildDate_(std::chrono::system_clock::now())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PluginMetadata::PluginMetadata(std::string name, std::string version) noexcept
    : name_(std::move(name))
    , version_(std::move(version))
    , buildDate_(std::chrono::system_clock::now())
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PluginMetadata::~PluginMetadata() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PluginMetadata::PluginMetadata(const PluginMetadata& other)
    : name_(other.name_)
    , version_(other.version_)
    , apiVersion_(other.apiVersion_)
    , description_(other.description_)
    , author_(other.author_)
    , license_(other.license_)
    , homepage_(other.homepage_)
    , providedServices_(other.providedServices_)
    , requiredServices_(other.requiredServices_)
    , capabilities_(other.capabilities_)
    , dependencies_(other.dependencies_)
    , properties_(other.properties_)
    , entryPoint_(other.entryPoint_)
    , icon_(other.icon_)
    , supportedPlatforms_(other.supportedPlatforms_)
    , buildDate_(other.buildDate_)
    , checksum_(other.checksum_)
    , priority_(other.priority_)
    , enabled_(other.enabled_)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

PluginMetadata::PluginMetadata(PluginMetadata&& other) noexcept
    : name_(std::move(other.name_))
    , version_(std::move(other.version_))
    , apiVersion_(std::move(other.apiVersion_))
    , description_(std::move(other.description_))
    , author_(std::move(other.author_))
    , license_(std::move(other.license_))
    , homepage_(std::move(other.homepage_))
    , providedServices_(std::move(other.providedServices_))
    , requiredServices_(std::move(other.requiredServices_))
    , capabilities_(std::move(other.capabilities_))
    , dependencies_(std::move(other.dependencies_))
    , properties_(std::move(other.properties_))
    , entryPoint_(std::move(other.entryPoint_))
    , icon_(std::move(other.icon_))
    , supportedPlatforms_(std::move(other.supportedPlatforms_))
    , buildDate_(other.buildDate_)
    , checksum_(std::move(other.checksum_))
    , priority_(other.priority_)
    , enabled_(other.enabled_)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto PluginMetadata::operator=(const PluginMetadata& other) -> PluginMetadata&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        name_ = other.name_;
        version_ = other.version_;
        apiVersion_ = other.apiVersion_;
        description_ = other.description_;
        author_ = other.author_;
        license_ = other.license_;
        homepage_ = other.homepage_;
        providedServices_ = other.providedServices_;
        requiredServices_ = other.requiredServices_;
        capabilities_ = other.capabilities_;
        dependencies_ = other.dependencies_;
        properties_ = other.properties_;
        entryPoint_ = other.entryPoint_;
        icon_ = other.icon_;
        supportedPlatforms_ = other.supportedPlatforms_;
        buildDate_ = other.buildDate_;
        checksum_ = other.checksum_;
        priority_ = other.priority_;
        enabled_ = other.enabled_;
    }
    return *this;
}

auto PluginMetadata::operator=(PluginMetadata&& other) noexcept -> PluginMetadata&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (this != &other)
    {
        name_ = std::move(other.name_);
        version_ = std::move(other.version_);
        apiVersion_ = std::move(other.apiVersion_);
        description_ = std::move(other.description_);
        author_ = std::move(other.author_);
        license_ = std::move(other.license_);
        homepage_ = std::move(other.homepage_);
        providedServices_ = std::move(other.providedServices_);
        requiredServices_ = std::move(other.requiredServices_);
        capabilities_ = std::move(other.capabilities_);
        dependencies_ = std::move(other.dependencies_);
        properties_ = std::move(other.properties_);
        entryPoint_ = std::move(other.entryPoint_);
        icon_ = std::move(other.icon_);
        supportedPlatforms_ = std::move(other.supportedPlatforms_);
        buildDate_ = other.buildDate_;
        checksum_ = std::move(other.checksum_);
        priority_ = other.priority_;
        enabled_ = other.enabled_;
    }
    return *this;
}

// ============================================
// IPluginMetadata Implementation
// ============================================

auto PluginMetadata::getName() const noexcept -> std::string_view
{
    return name_;
}

auto PluginMetadata::getVersion() const noexcept -> std::string_view
{
    return version_;
}

auto PluginMetadata::getApiVersion() const noexcept -> std::string_view
{
    return apiVersion_;
}

auto PluginMetadata::getDescription() const noexcept -> std::string_view
{
    return description_;
}

auto PluginMetadata::getAuthor() const noexcept -> std::string_view
{
    return author_;
}

auto PluginMetadata::getLicense() const noexcept -> std::string_view
{
    return license_;
}

auto PluginMetadata::getHomepage() const noexcept -> std::optional<std::string_view>
{
    if (homepage_.has_value())
    {
        return std::string_view(*homepage_);
    }
    return std::nullopt;
}

auto PluginMetadata::getProvidedServices() const noexcept -> const std::vector<std::string>&
{
    return providedServices_;
}

auto PluginMetadata::getRequiredServices() const noexcept -> const std::vector<std::string>&
{
    return requiredServices_;
}

auto PluginMetadata::getCapabilities() const noexcept -> const std::vector<std::string>&
{
    return capabilities_;
}

auto PluginMetadata::hasCapability(std::string_view capability) const noexcept -> bool
{
    return std::find(capabilities_.begin(), capabilities_.end(), capability) != capabilities_.end();
}

auto PluginMetadata::getDependencies() const noexcept -> const std::vector<PluginDependency>&
{
    return dependencies_;
}

auto PluginMetadata::dependsOn(std::string_view pluginName) const noexcept -> bool
{
    return std::any_of(dependencies_.begin(), dependencies_.end(),
        [pluginName](const PluginDependency& dep)
        {
            return dep.pluginName == pluginName;
        });
}

auto PluginMetadata::getProperty(std::string_view key) const noexcept
    -> std::optional<std::string_view>
{
    auto it = properties_.find(std::string(key));
    if (it != properties_.end())
    {
        return std::string_view(it->second);
    }
    return std::nullopt;
}

auto PluginMetadata::getProperties() const noexcept -> const std::map<std::string, std::string>&
{
    return properties_;
}

auto PluginMetadata::getEntryPoint() const noexcept -> std::optional<std::string_view>
{
    if (entryPoint_.has_value())
    {
        return std::string_view(*entryPoint_);
    }
    return std::nullopt;
}

auto PluginMetadata::getIcon() const noexcept -> std::optional<std::string_view>
{
    if (icon_.has_value())
    {
        return std::string_view(*icon_);
    }
    return std::nullopt;
}

auto PluginMetadata::getSupportedPlatforms() const noexcept -> const std::vector<std::string>&
{
    return supportedPlatforms_;
}

auto PluginMetadata::supportsCurrentPlatform() const noexcept -> bool
{
    if (supportedPlatforms_.empty())
    {
        return true; // If no platforms specified, assume all are supported
    }

    auto currentPlatform = getCurrentPlatform();
    return std::find(supportedPlatforms_.begin(), supportedPlatforms_.end(),
                     currentPlatform) != supportedPlatforms_.end();
}

auto PluginMetadata::getBuildDate() const noexcept -> std::chrono::system_clock::time_point
{
    return buildDate_;
}

auto PluginMetadata::getChecksum() const noexcept -> std::optional<std::string_view>
{
    if (checksum_.has_value())
    {
        return std::string_view(*checksum_);
    }
    return std::nullopt;
}

auto PluginMetadata::getPriority() const noexcept -> PluginPriority
{
    return priority_;
}

auto PluginMetadata::isEnabled() const noexcept -> bool
{
    return enabled_;
}

// ============================================
// Setters
// ============================================

auto PluginMetadata::setName(std::string name) -> PluginMetadata&
{
    name_ = std::move(name);
    return *this;
}

auto PluginMetadata::setVersion(std::string version) -> PluginMetadata&
{
    version_ = std::move(version);
    return *this;
}

auto PluginMetadata::setApiVersion(std::string apiVersion) -> PluginMetadata&
{
    apiVersion_ = std::move(apiVersion);
    return *this;
}

auto PluginMetadata::setDescription(std::string description) -> PluginMetadata&
{
    description_ = std::move(description);
    return *this;
}

auto PluginMetadata::setAuthor(std::string author) -> PluginMetadata&
{
    author_ = std::move(author);
    return *this;
}

auto PluginMetadata::setLicense(std::string license) -> PluginMetadata&
{
    license_ = std::move(license);
    return *this;
}

auto PluginMetadata::setHomepage(std::string homepage) -> PluginMetadata&
{
    homepage_ = std::move(homepage);
    return *this;
}

auto PluginMetadata::addProvidedService(std::string service) -> PluginMetadata&
{
    providedServices_.push_back(std::move(service));
    return *this;
}

auto PluginMetadata::setProvidedServices(std::vector<std::string> services) -> PluginMetadata&
{
    providedServices_ = std::move(services);
    return *this;
}

auto PluginMetadata::addRequiredService(std::string service) -> PluginMetadata&
{
    requiredServices_.push_back(std::move(service));
    return *this;
}

auto PluginMetadata::setRequiredServices(std::vector<std::string> services) -> PluginMetadata&
{
    requiredServices_ = std::move(services);
    return *this;
}

auto PluginMetadata::addCapability(std::string capability) -> PluginMetadata&
{
    capabilities_.push_back(std::move(capability));
    return *this;
}

auto PluginMetadata::setCapabilities(std::vector<std::string> capabilities) -> PluginMetadata&
{
    capabilities_ = std::move(capabilities);
    return *this;
}

auto PluginMetadata::addDependency(PluginDependency dependency) -> PluginMetadata&
{
    dependencies_.push_back(std::move(dependency));
    return *this;
}

auto PluginMetadata::addDependency(std::string name, std::string versionConstraint, bool optional)
    -> PluginMetadata&
{
    dependencies_.push_back(PluginDependency{
        std::move(name),
        std::move(versionConstraint),
        optional
    });
    return *this;
}

auto PluginMetadata::setDependencies(std::vector<PluginDependency> dependencies) -> PluginMetadata&
{
    dependencies_ = std::move(dependencies);
    return *this;
}

auto PluginMetadata::setProperty(std::string key, std::string value) -> PluginMetadata&
{
    properties_[std::move(key)] = std::move(value);
    return *this;
}

auto PluginMetadata::setProperties(std::map<std::string, std::string> properties) -> PluginMetadata&
{
    properties_ = std::move(properties);
    return *this;
}

auto PluginMetadata::setEntryPoint(std::string entryPoint) -> PluginMetadata&
{
    entryPoint_ = std::move(entryPoint);
    return *this;
}

auto PluginMetadata::setIcon(std::string icon) -> PluginMetadata&
{
    icon_ = std::move(icon);
    return *this;
}

auto PluginMetadata::addSupportedPlatform(std::string platform) -> PluginMetadata&
{
    supportedPlatforms_.push_back(std::move(platform));
    return *this;
}

auto PluginMetadata::setSupportedPlatforms(std::vector<std::string> platforms) -> PluginMetadata&
{
    supportedPlatforms_ = std::move(platforms);
    return *this;
}

auto PluginMetadata::setBuildDate(std::chrono::system_clock::time_point buildDate) -> PluginMetadata&
{
    buildDate_ = buildDate;
    return *this;
}

auto PluginMetadata::setChecksum(std::string checksum) -> PluginMetadata&
{
    checksum_ = std::move(checksum);
    return *this;
}

auto PluginMetadata::setPriority(PluginPriority priority) -> PluginMetadata&
{
    priority_ = priority;
    return *this;
}

auto PluginMetadata::setEnabled(bool enabled) -> PluginMetadata&
{
    enabled_ = enabled;
    return *this;
}

// ============================================
// Factory Methods
// ============================================

auto PluginMetadata::create(std::string name, std::string version) -> PluginMetadata
{
    return PluginMetadata(std::move(name), std::move(version));
}
