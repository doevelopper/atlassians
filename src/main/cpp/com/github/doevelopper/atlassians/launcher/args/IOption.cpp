/*******************************************************************
 * @file   IOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of IOption interface
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr IOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.IOption"));

IOption::IOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

IOption::~IOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto IOption::configurePositional(
    [[maybe_unused]] boost::program_options::positional_options_description& positional) const -> void
{
    // Default: do nothing (not a positional option)
}

auto IOption::isSet(const boost::program_options::variables_map& vm) const noexcept -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::string name{getLongName()};
    return vm.count(name) > 0;
}

auto IOption::validate([[maybe_unused]] const boost::program_options::variables_map& vm) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // Default: always valid
    return true;
}

auto IOption::getFormattedName() const -> std::string
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::string result;
    auto shortName = getShortName();
    auto longName = getLongName();

    // Boost format: long-name,short-name (e.g., "help,h")
    if (!shortName.empty())
    {
        result = std::string(longName) + "," + std::string(shortName);
    }
    else
    {
        result = std::string(longName);
    }
    return result;
}
