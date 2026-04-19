/**
 * @file   FileLicenseStorage.cpp
 * @brief  File-based license storage implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/FileLicenseStorage.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr FileLicenseStorage::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.FileLicenseStorage"));

FileLicenseStorage::FileLicenseStorage(
    const std::filesystem::path& storageDir,
    bool createIfMissing)
    : BaseLicenseStorage(storageDir.string())
    , m_storageDir(storageDir)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Storage directory: " << storageDir);

    if (createIfMissing && !std::filesystem::exists(m_storageDir))
    {
        LOG4CXX_INFO(logger, "Creating license storage directory: " << m_storageDir);
        std::filesystem::create_directories(m_storageDir);
    }
}

FileLicenseStorage::~FileLicenseStorage()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto FileLicenseStorage::doSave(const License& license) -> void
{
    auto filePath = getLicenseFilePath(license.getLicenseKey());

    LOG4CXX_DEBUG(logger, "Saving license to: " << filePath);

    std::ofstream file(filePath);
    if (!file.is_open())
    {
        throw LicenseStorageException("Failed to open license file for writing: " + filePath.string());
    }

    file << serializeLicense(license);

    if (file.fail())
    {
        throw LicenseStorageException("Failed to write license to file: " + filePath.string());
    }

    LOG4CXX_INFO(logger, "License saved successfully: " << license.getLicenseKey());
}

auto FileLicenseStorage::doLoad(std::string_view licenseKey) -> std::optional<License>
{
    auto filePath = getLicenseFilePath(licenseKey);

    LOG4CXX_DEBUG(logger, "Loading license from: " << filePath);

    if (!std::filesystem::exists(filePath))
    {
        LOG4CXX_DEBUG(logger, "License file not found: " << filePath);
        return std::nullopt;
    }

    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw LicenseStorageException("Failed to open license file for reading: " + filePath.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return deserializeLicense(buffer.str());
}

auto FileLicenseStorage::doExists(std::string_view licenseKey) const -> bool
{
    auto filePath = getLicenseFilePath(licenseKey);
    return std::filesystem::exists(filePath);
}

auto FileLicenseStorage::loadPrimary() -> std::optional<License>
{
    std::lock_guard<std::mutex> lock(getMutex());

    LOG4CXX_DEBUG(logger, "Loading primary license from: " << m_storageDir);

    // First check for dedicated primary license file
    auto primaryPath = m_storageDir / m_primaryLicenseFile;
    if (std::filesystem::exists(primaryPath))
    {
        std::ifstream file(primaryPath);
        if (file.is_open())
        {
            std::stringstream buffer;
            buffer << file.rdbuf();
            return deserializeLicense(buffer.str());
        }
    }

    // Otherwise, load the first available license
    if (!std::filesystem::exists(m_storageDir))
    {
        return std::nullopt;
    }

    for (const auto& entry : std::filesystem::directory_iterator(m_storageDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lic")
        {
            std::ifstream file(entry.path());
            if (file.is_open())
            {
                std::stringstream buffer;
                buffer << file.rdbuf();
                auto license = deserializeLicense(buffer.str());
                if (license.has_value())
                {
                    LOG4CXX_DEBUG(logger, "Loaded primary license: " << license.value().getLicenseKey());
                    return license;
                }
            }
        }
    }

    LOG4CXX_DEBUG(logger, "No primary license found");
    return std::nullopt;
}

auto FileLicenseStorage::remove(std::string_view licenseKey) -> bool
{
    std::lock_guard<std::mutex> lock(getMutex());

    auto filePath = getLicenseFilePath(licenseKey);

    LOG4CXX_DEBUG(logger, "Removing license: " << filePath);

    if (!std::filesystem::exists(filePath))
    {
        return false;
    }

    std::error_code ec;
    bool removed = std::filesystem::remove(filePath, ec);

    if (ec)
    {
        throw LicenseStorageException("Failed to remove license file: " + ec.message());
    }

    LOG4CXX_INFO(logger, "License removed: " << licenseKey);
    return removed;
}

auto FileLicenseStorage::listAll() const -> std::vector<std::string>
{
    std::lock_guard<std::mutex> lock(getMutex());
    std::vector<std::string> licenses;

    if (!std::filesystem::exists(m_storageDir))
    {
        return licenses;
    }

    for (const auto& entry : std::filesystem::directory_iterator(m_storageDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lic")
        {
            auto stem = entry.path().stem().string();
            if (stem != "primary")
            {
                licenses.push_back(stem);
            }
        }
    }

    return licenses;
}

auto FileLicenseStorage::clear() -> void
{
    std::lock_guard<std::mutex> lock(getMutex());

    LOG4CXX_INFO(logger, "Clearing all licenses from: " << m_storageDir);

    if (!std::filesystem::exists(m_storageDir))
    {
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator(m_storageDir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lic")
        {
            std::filesystem::remove(entry.path());
        }
    }
}

auto FileLicenseStorage::isAvailable() const noexcept -> bool
{
    try
    {
        return std::filesystem::exists(m_storageDir) &&
               std::filesystem::is_directory(m_storageDir);
    }
    catch (...)
    {
        return false;
    }
}

auto FileLicenseStorage::getLicenseFilePath(std::string_view licenseKey) const -> std::filesystem::path
{
    return m_storageDir / (sanitizeFilename(licenseKey) + ".lic");
}

auto FileLicenseStorage::sanitizeFilename(std::string_view licenseKey) const -> std::string
{
    std::string result(licenseKey);

    // Replace invalid filename characters
    std::replace(result.begin(), result.end(), '/', '_');
    std::replace(result.begin(), result.end(), '\\', '_');
    std::replace(result.begin(), result.end(), ':', '_');
    std::replace(result.begin(), result.end(), '*', '_');
    std::replace(result.begin(), result.end(), '?', '_');
    std::replace(result.begin(), result.end(), '"', '_');
    std::replace(result.begin(), result.end(), '<', '_');
    std::replace(result.begin(), result.end(), '>', '_');
    std::replace(result.begin(), result.end(), '|', '_');

    return result;
}
