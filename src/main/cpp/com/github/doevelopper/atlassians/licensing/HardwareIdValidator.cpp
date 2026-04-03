/**
 * @file   HardwareIdValidator.cpp
 * @brief  Hardware ID binding validator implementation
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/licensing/HardwareIdValidator.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <sstream>

#ifdef __linux__
    #include <net/if.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

using namespace com::github::doevelopper::atlassians::licensing;

log4cxx::LoggerPtr HardwareIdValidator::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.licensing.HardwareIdValidator"));

HardwareIdValidator::HardwareIdValidator()
    : BaseLicenseValidator("HardwareIdValidator", 20)
    , m_hardwareIdGenerator(&HardwareIdValidator::generateDefaultHardwareId)
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

HardwareIdValidator::HardwareIdValidator(HardwareIdGenerator generator)
    : BaseLicenseValidator("HardwareIdValidator", 20)
    , m_hardwareIdGenerator(std::move(generator))
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

HardwareIdValidator::~HardwareIdValidator()
{
    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__);
}

auto HardwareIdValidator::setHardwareIdGenerator(HardwareIdGenerator generator) -> void
{
    m_hardwareIdGenerator = std::move(generator);
}

auto HardwareIdValidator::getCurrentHardwareId() const -> std::string
{
    if (m_hardwareIdGenerator)
    {
        return m_hardwareIdGenerator();
    }
    return generateDefaultHardwareId();
}

auto HardwareIdValidator::generateDefaultHardwareId() -> std::string
{
    std::ostringstream fingerprint;

#ifdef __linux__
    // Read CPU info
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    while (std::getline(cpuinfo, line))
    {
        if (line.find("model name") != std::string::npos ||
            line.find("cpu family") != std::string::npos)
        {
            fingerprint << line;
        }
    }

    // Read machine ID
    std::ifstream machineId("/etc/machine-id");
    if (machineId.is_open())
    {
        std::string id;
        std::getline(machineId, id);
        fingerprint << id;
    }

    // Get MAC address of first interface
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock >= 0)
    {
        struct ifreq ifr;
        std::strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';

        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0)
        {
            for (int i = 0; i < 6; ++i)
            {
                fingerprint << std::hex << std::setfill('0') << std::setw(2)
                            << static_cast<unsigned int>(static_cast<unsigned char>(ifr.ifr_hwaddr.sa_data[i]));
            }
        }
        close(sock);
    }
#else
    // Fallback for other platforms
    fingerprint << "default-hardware-id";
#endif

    // Hash the fingerprint to create a fixed-length ID
    std::hash<std::string> hasher;
    auto hashValue = hasher(fingerprint.str());

    std::ostringstream result;
    result << std::hex << std::setfill('0') << std::setw(16) << hashValue;

    return result.str();
}

auto HardwareIdValidator::doValidate(const License& license) const -> ValidationResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Check if license is hardware-bound
    if (!license.isHardwareBound())
    {
        LOG4CXX_DEBUG(logger, "License is not hardware-bound, skipping validation");
        return ValidationResult::success("License is not hardware-bound");
    }

    auto expectedHardwareId = license.getHardwareId();
    if (!expectedHardwareId.has_value())
    {
        return ValidationResult::failure(
            ValidationCode::HardwareMismatch,
            "License is marked as hardware-bound but has no hardware ID");
    }

    auto currentHardwareId = getCurrentHardwareId();

    LOG4CXX_DEBUG(logger, "Expected hardware ID: " << expectedHardwareId.value());
    LOG4CXX_DEBUG(logger, "Current hardware ID: " << currentHardwareId);

    if (expectedHardwareId.value() != currentHardwareId)
    {
        std::string message = "Hardware ID mismatch";
        LOG4CXX_WARN(logger, message << " - expected: " << expectedHardwareId.value()
                                     << ", actual: " << currentHardwareId);

        return ValidationResult::failure(
            ValidationCode::HardwareMismatch,
            message,
            "This license is bound to different hardware");
    }

    return ValidationResult::success("Hardware ID validation passed");
}
