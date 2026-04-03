/**
 * @file   LicenseManager.cpp
 * @brief  Thread-safe License Manager facade implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/LicenseManager.hpp>

#include <fstream>
#include <sstream>

#include <com/github/doevelopper/atlassians/licensing/ExpirationValidator.hpp>
#include <com/github/doevelopper/atlassians/licensing/FeatureValidator.hpp>
#include <com/github/doevelopper/atlassians/licensing/HardwareIdValidator.hpp>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr LicenseManager::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.LicenseManager"));

// Static member initialization
std::shared_ptr<LicenseManager> LicenseManager::s_instance = nullptr;
std::mutex LicenseManager::s_instanceMutex;

LicenseManager::LicenseManager()
    : m_validationChain(std::make_unique<ValidationChain>())
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Creating LicenseManager instance");

    // Add default validators
    m_validationChain->addValidator(std::make_shared<ExpirationValidator>());
    m_validationChain->addValidator(std::make_shared<HardwareIdValidator>());
    m_validationChain->addValidator(std::make_shared<FeatureValidator>());

    // Sort validators by priority
    m_validationChain->sortByPriority();
}

LicenseManager::~LicenseManager()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto LicenseManager::getInstance() -> std::shared_ptr<LicenseManager>
{
    std::lock_guard<std::mutex> lock(s_instanceMutex);

    if (!s_instance)
    {
        // Use private constructor through shared_ptr with custom deleter
        s_instance = std::shared_ptr<LicenseManager>(new LicenseManager());
        LOG4CXX_INFO(logger, "LicenseManager singleton instance created");
    }

    return s_instance;
}

auto LicenseManager::resetInstance() -> void
{
    std::lock_guard<std::mutex> lock(s_instanceMutex);
    s_instance.reset();
    LOG4CXX_DEBUG(logger, "LicenseManager singleton instance reset");
}

auto LicenseManager::setStorage(ILicenseStoragePtr storage) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_storage = std::move(storage);
    LOG4CXX_DEBUG(logger, "License storage set");
}

auto LicenseManager::addValidator(ILicenseValidatorPtr validator) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (validator)
    {
        m_validationChain->addValidator(std::move(validator));
        m_validationChain->sortByPriority();
        LOG4CXX_DEBUG(logger, "Validator added to chain");
    }
}

auto LicenseManager::removeValidator(std::string_view name) -> bool
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_validationChain->removeValidator(name);
}

auto LicenseManager::clearValidators() -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_validationChain->clearValidators();
}

auto LicenseManager::registerObserver(ILicenseObserverPtr observer) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_observers.emplace_back(observer);
    LOG4CXX_DEBUG(logger, "Observer registered");
}

auto LicenseManager::unregisterObserver(const ILicenseObserverPtr& observer) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_observers.erase(
        std::remove_if(m_observers.begin(), m_observers.end(),
                       [&observer](const ILicenseObserverWPtr& wp) {
                           auto sp = wp.lock();
                           return !sp || sp == observer;
                       }),
        m_observers.end());

    LOG4CXX_DEBUG(logger, "Observer unregistered");
}

auto LicenseManager::loadLicense(std::string_view licenseKey) -> License
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG4CXX_INFO(logger, "Loading license: " << licenseKey);

    if (!m_storage)
    {
        throw LicenseStorageException("No license storage configured");
    }

    auto license = m_storage->load(licenseKey);
    if (!license.has_value())
    {
        throw LicenseStorageException("License not found: " + std::string(licenseKey));
    }

    // Clear cache
    m_cachedLicense.reset();
    m_cachedValidationResult.reset();

    // Notify observers
    notifyObservers(LicenseEvent::Loaded, license.value());

    return license.value();
}

auto LicenseManager::loadPrimaryLicense() -> License
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG4CXX_INFO(logger, "Loading primary license");

    if (!m_storage)
    {
        throw LicenseStorageException("No license storage configured");
    }

    auto license = m_storage->loadPrimary();
    if (!license.has_value())
    {
        throw LicenseStorageException("No primary license found");
    }

    // Cache the license
    m_cachedLicense = license.value();

    // Notify observers
    notifyObservers(LicenseEvent::Loaded, license.value());

    return license.value();
}

auto LicenseManager::loadLicenseFromFile(const std::filesystem::path& filePath) -> License
{
    LOG4CXX_INFO(logger, "Loading license from file: " << filePath);

    if (!std::filesystem::exists(filePath))
    {
        throw LicenseStorageException("License file not found: " + filePath.string());
    }

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw LicenseStorageException("Failed to open license file: " + filePath.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    // Parse the license data (simplified parsing)
    auto content = buffer.str();

    // This is a simplified implementation. In production,
    // use proper JSON parsing with nlohmann/json
    auto extractValue = [&content](const std::string& key) -> std::string {
        auto keyPos = content.find("\"" + key + "\"");
        if (keyPos == std::string::npos)
            return "";

        auto colonPos = content.find(':', keyPos);
        if (colonPos == std::string::npos)
            return "";

        auto valueStart = content.find_first_not_of(" \t\n", colonPos + 1);
        if (valueStart == std::string::npos)
            return "";

        if (content[valueStart] == '"')
        {
            auto valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos)
                return "";
            return content.substr(valueStart + 1, valueEnd - valueStart - 1);
        }
        else
        {
            auto valueEnd = content.find_first_of(",}\n", valueStart);
            if (valueEnd == std::string::npos)
                valueEnd = content.size();
            auto value = content.substr(valueStart, valueEnd - valueStart);
            while (!value.empty() && std::isspace(value.back()))
                value.pop_back();
            return value;
        }
    };

    auto builder = License::builder();
    builder.withLicenseKey(extractValue("licenseKey"))
        .withProductId(extractValue("productId"))
        .withType(stringToLicenseType(extractValue("type")))
        .withLicenseeName(extractValue("licenseeName"))
        .withLicenseeEmail(extractValue("licenseeEmail"))
        .withDigitalSignature(extractValue("digitalSignature"));

    // Parse dates
    auto expStr = extractValue("expirationDate");
    if (!expStr.empty())
    {
        auto expTime = static_cast<std::time_t>(std::stoll(expStr));
        builder.withExpirationDate(std::chrono::system_clock::from_time_t(expTime));
    }

    auto issueStr = extractValue("issueDate");
    if (!issueStr.empty())
    {
        auto issueTime = static_cast<std::time_t>(std::stoll(issueStr));
        builder.withIssueDate(std::chrono::system_clock::from_time_t(issueTime));
    }

    auto license = builder.build();

    // Cache and notify
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cachedLicense = license;
    }

    notifyObservers(LicenseEvent::Loaded, license);

    return license;
}

auto LicenseManager::saveLicense(const License& license) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG4CXX_INFO(logger, "Saving license: " << license.getLicenseKey());

    if (!m_storage)
    {
        throw LicenseStorageException("No license storage configured");
    }

    m_storage->save(license);

    // Clear cache
    m_cachedLicense.reset();
    m_cachedValidationResult.reset();

    notifyObservers(LicenseEvent::Updated, license);
}

auto LicenseManager::validate(const License& license) -> bool
{
    return validateWithDetails(license).isValid();
}

auto LicenseManager::validateWithDetails(const License& license) -> ValidationResult
{
    std::lock_guard<std::mutex> lock(m_mutex);

    LOG4CXX_INFO(logger, "Validating license: " << license.getLicenseKey());

    // Check cache
    if (m_cachedLicense.has_value() &&
        m_cachedLicense->getLicenseKey() == license.getLicenseKey() &&
        m_cachedValidationResult.has_value())
    {
        LOG4CXX_DEBUG(logger, "Returning cached validation result");
        return m_cachedValidationResult.value();
    }

    auto result = m_validationChain->validateWithDetails(license);

    // Cache the result
    m_cachedLicense          = license;
    m_cachedValidationResult = result;

    // Notify observers
    notifyValidationResult(license, result);

    if (result.isValid())
    {
        notifyObservers(LicenseEvent::Validated, license);
    }
    else
    {
        notifyObservers(LicenseEvent::ValidationFailed, license);

        if (result.code == ValidationCode::Expired)
        {
            notifyObservers(LicenseEvent::Expired, license);
        }
    }

    return result;
}

auto LicenseManager::isFeatureEnabled(const License& license, std::string_view featureName) const -> bool
{
    return license.hasFeature(featureName);
}

auto LicenseManager::getLicenseStatus(const License& license) const -> LicenseStatus
{
    // Check expiration first since isValid() includes expiration check
    if (license.isExpired())
    {
        return LicenseStatus::Expired;
    }

    if (!license.isValid())
    {
        return LicenseStatus::Invalid;
    }

    return LicenseStatus::Valid;
}

auto LicenseManager::isPrimaryLicenseValid() -> bool
{
    try
    {
        auto license = loadPrimaryLicense();
        return validate(license);
    }
    catch (const LicenseException& e)
    {
        LOG4CXX_WARN(logger, "Primary license validation failed: " << e.what());
        return false;
    }
}

auto LicenseManager::hasValidLicense() -> bool
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_storage)
    {
        return false;
    }

    try
    {
        auto licenses = m_storage->listAll();
        for (const auto& key : licenses)
        {
            auto license = m_storage->load(key);
            if (license.has_value())
            {
                auto result = m_validationChain->validateWithDetails(license.value());
                if (result.isValid())
                {
                    return true;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Error checking for valid license: " << e.what());
    }

    return false;
}

auto LicenseManager::getDaysUntilExpiration(const License& license) const -> int
{
    return license.getDaysUntilExpiration();
}

auto LicenseManager::getEnabledFeatures(const License& license) const -> std::vector<std::string>
{
    const auto& features = license.getEnabledFeatures();
    return std::vector<std::string>(features.begin(), features.end());
}

auto LicenseManager::notifyObservers(LicenseEvent event, const License& license) -> void
{
    // Clean up expired weak pointers and notify active observers
    auto it = m_observers.begin();
    while (it != m_observers.end())
    {
        if (auto observer = it->lock())
        {
            try
            {
                observer->onLicenseEvent(event, license);
            }
            catch (const std::exception& e)
            {
                LOG4CXX_WARN(logger, "Observer threw exception: " << e.what());
            }
            ++it;
        }
        else
        {
            it = m_observers.erase(it);
        }
    }
}

auto LicenseManager::notifyValidationResult(const License& license, const ValidationResult& result) -> void
{
    auto it = m_observers.begin();
    while (it != m_observers.end())
    {
        if (auto observer = it->lock())
        {
            try
            {
                observer->onValidationResult(license, result);
            }
            catch (const std::exception& e)
            {
                LOG4CXX_WARN(logger, "Observer threw exception: " << e.what());
            }
            ++it;
        }
        else
        {
            it = m_observers.erase(it);
        }
    }
}
