/**
 * @file ErrorCode.cpp
 * @brief Implementation of 64-bit structured error code
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorCode.hpp>

#include <sstream>
#include <iomanip>
#include <array>

using namespace com::github::doevelopper::atlassians::triada;

log4cxx::LoggerPtr ErrorCode::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.ErrorCode"));

namespace
{
    // String lookup tables for severity levels
    constexpr std::array<std::string_view, 8> severityStrings = {
        "TRACE",
        "DEBUG",
        "INFORMATIONAL",
        "WARNING",
        "RECOVERABLE",
        "ERROR",
        "CRITICAL",
        "FATAL"
    };

    // String lookup tables for service identifiers
    constexpr std::array<std::string_view, 32> serviceStrings = {
        "Core",
        "Memory",
        "IO",
        "Network",
        "Storage",
        "Security",
        "Configuration",
        "Logging",
        "Communication",
        "Hardware",
        "Scheduler",
        "Timer",
        "Watchdog",
        "Power",
        "Diagnostic",
        "Protocol",
        "Application",
        "User",
        "External",
        "Test",
        "Reserved20",
        "Reserved21",
        "Reserved22",
        "Reserved23",
        "Reserved24",
        "Reserved25",
        "Reserved26",
        "Reserved27",
        "Reserved28",
        "Reserved29",
        "Reserved30",
        "Vendor"
    };
}

auto ErrorCode::severityString() const -> std::string_view
{
    auto sev = static_cast<std::size_t>(severity());
    if (sev < severityStrings.size())
    {
        return severityStrings[sev];
    }
    return "UNKNOWN";
}

auto ErrorCode::serviceString() const -> std::string_view
{
    auto svc = static_cast<std::size_t>(service());
    if (svc < serviceStrings.size())
    {
        return serviceStrings[svc];
    }
    return "Unknown";
}

auto ErrorCode::toHexString() const -> std::string
{
    std::ostringstream oss;
    oss << "0x" << std::hex << std::uppercase
        << std::setfill('0') << std::setw(16) << m_value;
    return oss.str();
}

auto ErrorCode::toString() const -> std::string
{
    std::ostringstream oss;
    oss << "ErrorCode{"
        << "severity=" << severityString()
        << ", service=" << serviceString()
        << ", mission=0x" << std::hex << std::uppercase << std::setfill('0')
        << std::setw(3) << mission()
        << ", code=0x" << std::setw(10) << code()
        << ", raw=" << toHexString()
        << "}";
    return oss.str();
}
