/*******************************************************************
 * @file   BooleanOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of BooleanOption
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/BooleanOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr BooleanOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.BooleanOption"));

BooleanOption::BooleanOption(std::string longName, std::string description)
    : AbstractOption(std::move(longName), std::move(description))
    , m_negatable(false)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

BooleanOption::BooleanOption(std::string longName, std::string shortName, std::string description)
    : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
    , m_negatable(false)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

BooleanOption::~BooleanOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto BooleanOption::negatable() -> BooleanOption&
{
    m_negatable = true;
    return *this;
}

auto BooleanOption::isNegatable() const noexcept -> bool
{
    return m_negatable;
}

auto BooleanOption::getBoolValue(const boost::program_options::variables_map& vm) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Check if the option was specified
    if (isSet(vm))
    {
        return true;
    }

    // Check for negated form if negatable
    if (m_negatable)
    {
        std::string negatedName = "no-" + std::string(getLongName());
        if (vm.count(negatedName) > 0)
        {
            return false;
        }
    }

    return false;
}

auto BooleanOption::getKind() const noexcept -> OptionKind
{
    return OptionKind::Boolean;
}

auto BooleanOption::getValue(const boost::program_options::variables_map& vm) const -> Value
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return getBoolValue(vm);
}

auto BooleanOption::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Add the main option
    std::string formattedName = getFormattedName();
    desc.add_options()(formattedName.c_str(), m_description.c_str());

    // Add negated form if negatable
    if (m_negatable)
    {
        std::string negatedName = "no-" + std::string(getLongName());
        std::string negatedDesc = "Disable " + std::string(getLongName());
        desc.add_options()(negatedName.c_str(), negatedDesc.c_str());
    }

    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Configured boolean option: " << formattedName);
}

auto BooleanOption::doConfigureValue() const -> const boost::program_options::value_semantic*
{
    // Boolean switches don't have values
    return nullptr;
}
