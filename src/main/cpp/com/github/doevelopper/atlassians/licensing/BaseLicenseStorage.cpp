/**
 * @file   BaseLicenseStorage.cpp
 * @brief  Base class implementation for license storage
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/BaseLicenseStorage.hpp>

#include <iomanip>
#include <sstream>

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr BaseLicenseStorage::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.BaseLicenseStorage"));

BaseLicenseStorage::BaseLicenseStorage(std::string_view storageId)
    : m_storageId(storageId)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Storage ID: " << storageId);
}

BaseLicenseStorage::~BaseLicenseStorage()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto BaseLicenseStorage::save(const License& license) -> void
{
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " Saving license: " << license.getLicenseKey());
    doSave(license);
}

auto BaseLicenseStorage::load(std::string_view licenseKey) -> std::optional<License>
{
    std::lock_guard<std::mutex> lock(m_mutex);
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " Loading license: " << licenseKey);
    return doLoad(licenseKey);
}

auto BaseLicenseStorage::exists(std::string_view licenseKey) const -> bool
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return doExists(licenseKey);
}

auto BaseLicenseStorage::getStorageLocation() const -> std::string
{
    return m_storageId;
}

auto BaseLicenseStorage::serializeLicense(const License& license) const -> std::string
{
    // Helper to escape JSON special characters
    auto escapeJson = [](std::string_view input) -> std::string {
        std::string output;
        output.reserve(input.size() * 2);
        for (char c : input)
        {
            switch (c)
            {
                case '"':
                    output += "\\\"";
                    break;
                case '\\':
                    output += "\\\\";
                    break;
                case '\n':
                    output += "\\n";
                    break;
                case '\r':
                    output += "\\r";
                    break;
                case '\t':
                    output += "\\t";
                    break;
                default:
                    output += c;
            }
        }
        return output;
    };

    // Simple JSON-like serialization (in production, use a proper JSON library)
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"licenseKey\": \"" << escapeJson(license.getLicenseKey()) << "\",\n";
    oss << "  \"productId\": \"" << escapeJson(license.getProductId()) << "\",\n";
    oss << "  \"type\": \"" << licenseTypeToString(license.getType()) << "\",\n";
    oss << "  \"licenseeName\": \"" << escapeJson(license.getLicenseeName()) << "\",\n";
    oss << "  \"licenseeEmail\": \"" << escapeJson(license.getLicenseeEmail()) << "\",\n";

    // Serialize expiration date as ISO 8601
    auto expTime   = std::chrono::system_clock::to_time_t(license.getExpirationDate());
    auto issueTime = std::chrono::system_clock::to_time_t(license.getIssueDate());
    oss << "  \"expirationDate\": " << expTime << ",\n";
    oss << "  \"issueDate\": " << issueTime << ",\n";

    // Features array
    oss << "  \"features\": [";
    const auto& features = license.getEnabledFeatures();
    for (size_t i = 0; i < features.size(); ++i)
    {
        oss << "\"" << escapeJson(features[i]) << "\"";
        if (i < features.size() - 1)
            oss << ", ";
    }
    oss << "],\n";

    // Metadata array
    const auto& metadata = license.getMetadata();
    if (!metadata.empty())
    {
        oss << "  \"metadata\": [";
        for (size_t i = 0; i < metadata.size(); ++i)
        {
            oss << "{\"key\": \"" << escapeJson(metadata[i].first) << "\", \"value\": \""
                << escapeJson(metadata[i].second) << "\"}";
            if (i < metadata.size() - 1)
                oss << ", ";
        }
        oss << "],\n";
    }

    // Optional fields
    if (license.getMaxConcurrentUsers().has_value())
    {
        oss << "  \"maxConcurrentUsers\": " << license.getMaxConcurrentUsers().value() << ",\n";
    }

    if (license.getHardwareId().has_value())
    {
        oss << "  \"hardwareId\": \"" << escapeJson(license.getHardwareId().value()) << "\",\n";
    }

    oss << "  \"digitalSignature\": \"" << escapeJson(license.getDigitalSignature()) << "\",\n";
    oss << "  \"version\": \"" << escapeJson(license.getVersion()) << "\"\n";
    oss << "}";

    return oss.str();
}

auto BaseLicenseStorage::deserializeLicense(std::string_view data) const -> std::optional<License>
{
    // Simple parsing (in production, use a proper JSON library like nlohmann/json)
    // This is a basic implementation for demonstration purposes

    try
    {
        auto builder = License::builder();

        // Helper to unescape JSON strings
        auto unescapeJson = [](const std::string& input) -> std::string {
            std::string output;
            output.reserve(input.size());
            for (size_t i = 0; i < input.size(); ++i)
            {
                if (input[i] == '\\' && i + 1 < input.size())
                {
                    switch (input[i + 1])
                    {
                        case '"':
                            output += '"';
                            ++i;
                            break;
                        case '\\':
                            output += '\\';
                            ++i;
                            break;
                        case 'n':
                            output += '\n';
                            ++i;
                            break;
                        case 'r':
                            output += '\r';
                            ++i;
                            break;
                        case 't':
                            output += '\t';
                            ++i;
                            break;
                        default:
                            output += input[i];
                    }
                }
                else
                {
                    output += input[i];
                }
            }
            return output;
        };

        // Extract fields using simple string parsing
        // Note: This is a simplified implementation. Use nlohmann::json in production.

        auto extractValue = [&data, &unescapeJson](const std::string& key) -> std::string {
            auto keyPos = data.find("\"" + key + "\"");
            if (keyPos == std::string_view::npos)
                return "";

            auto colonPos = data.find(':', keyPos);
            if (colonPos == std::string_view::npos)
                return "";

            auto valueStart = data.find_first_not_of(" \t\n", colonPos + 1);
            if (valueStart == std::string_view::npos)
                return "";

            if (data[valueStart] == '"')
            {
                // String value - handle escaped quotes
                size_t valueEnd = valueStart + 1;
                while (valueEnd < data.size())
                {
                    if (data[valueEnd] == '"' && data[valueEnd - 1] != '\\')
                        break;
                    ++valueEnd;
                }
                if (valueEnd == data.size())
                    return "";
                return unescapeJson(std::string(data.substr(valueStart + 1, valueEnd - valueStart - 1)));
            }
            else
            {
                // Number or other value
                auto valueEnd = data.find_first_of(",}\n", valueStart);
                if (valueEnd == std::string_view::npos)
                    valueEnd = data.size();
                auto value = data.substr(valueStart, valueEnd - valueStart);
                // Trim whitespace
                while (!value.empty() && std::isspace(value.back()))
                    value.remove_suffix(1);
                return std::string(value);
            }
        };

        builder.withLicenseKey(extractValue("licenseKey"));
        builder.withProductId(extractValue("productId"));
        builder.withType(stringToLicenseType(extractValue("type")));
        builder.withLicenseeName(extractValue("licenseeName"));
        builder.withLicenseeEmail(extractValue("licenseeEmail"));
        builder.withDigitalSignature(extractValue("digitalSignature"));
        builder.withVersion(extractValue("version"));

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

        // Parse max concurrent users
        auto maxUsersStr = extractValue("maxConcurrentUsers");
        if (!maxUsersStr.empty())
        {
            builder.withMaxConcurrentUsers(std::stoi(maxUsersStr));
        }

        // Parse hardware ID
        auto hwId = extractValue("hardwareId");
        if (!hwId.empty())
        {
            builder.withHardwareId(hwId);
        }

        // Parse features array
        auto featuresPos = data.find("\"features\"");
        if (featuresPos != std::string_view::npos)
        {
            auto arrayStart = data.find('[', featuresPos);
            auto arrayEnd   = data.find(']', arrayStart);
            if (arrayStart != std::string_view::npos && arrayEnd != std::string_view::npos)
            {
                auto arrayContent = data.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                // Parse feature strings from array
                size_t pos = 0;
                while (pos < arrayContent.size())
                {
                    auto quoteStart = arrayContent.find('"', pos);
                    if (quoteStart == std::string_view::npos)
                        break;
                    auto quoteEnd = arrayContent.find('"', quoteStart + 1);
                    if (quoteEnd == std::string_view::npos)
                        break;
                    auto feature = arrayContent.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
                    builder.withFeature(unescapeJson(std::string(feature)));
                    pos = quoteEnd + 1;
                }
            }
        }

        // Parse metadata array
        auto metadataPos = data.find("\"metadata\"");
        if (metadataPos != std::string_view::npos)
        {
            auto arrayStart = data.find('[', metadataPos);
            auto arrayEnd   = data.find(']', arrayStart);
            if (arrayStart != std::string_view::npos && arrayEnd != std::string_view::npos)
            {
                auto arrayContent = data.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
                // Parse metadata objects from array: {"key": "...", "value": "..."}
                size_t pos = 0;
                while (pos < arrayContent.size())
                {
                    auto objStart = arrayContent.find('{', pos);
                    if (objStart == std::string_view::npos)
                        break;
                    auto objEnd = arrayContent.find('}', objStart);
                    if (objEnd == std::string_view::npos)
                        break;

                    auto objContent = arrayContent.substr(objStart, objEnd - objStart + 1);

                    // Extract key
                    auto keyPos   = objContent.find("\"key\"");
                    auto valuePos = objContent.find("\"value\"");

                    if (keyPos != std::string_view::npos && valuePos != std::string_view::npos)
                    {
                        auto colonPos1 = objContent.find(':', keyPos);
                        auto quoteStart1 = objContent.find('"', colonPos1 + 1);
                        auto quoteEnd1   = objContent.find('"', quoteStart1 + 1);

                        auto colonPos2 = objContent.find(':', valuePos);
                        auto quoteStart2 = objContent.find('"', colonPos2 + 1);
                        auto quoteEnd2   = objContent.find('"', quoteStart2 + 1);

                        if (quoteStart1 != std::string_view::npos && quoteEnd1 != std::string_view::npos &&
                            quoteStart2 != std::string_view::npos && quoteEnd2 != std::string_view::npos)
                        {
                            auto key = unescapeJson(std::string(objContent.substr(quoteStart1 + 1, quoteEnd1 - quoteStart1 - 1)));
                            auto value = unescapeJson(std::string(objContent.substr(quoteStart2 + 1, quoteEnd2 - quoteStart2 - 1)));
                            builder.withMetadata(key, value);
                        }
                    }
                    pos = objEnd + 1;
                }
            }
        }

        return builder.tryBuild();
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Failed to deserialize license: " << e.what());
        return std::nullopt;
    }
}

auto BaseLicenseStorage::getMutex() const -> std::mutex&
{
    return m_mutex;
}
