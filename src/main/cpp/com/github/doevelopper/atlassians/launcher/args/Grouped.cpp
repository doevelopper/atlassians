/*******************************************************************
 * @file   Grouped.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of OptionGroup
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/Grouped.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr OptionGroup::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.OptionGroup"));

OptionGroup::OptionGroup(std::string name, std::string description)
    : m_name(std::move(name))
    , m_description(std::move(description))
    , m_options()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Creating group: " << m_name);
}

OptionGroup::~OptionGroup() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto OptionGroup::getName() const noexcept -> std::string_view
{
    return m_name;
}

auto OptionGroup::getDescription() const noexcept -> std::string_view
{
    return m_description;
}

auto OptionGroup::addOption(IOption::Ptr option) -> OptionGroup&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Adding option: " << option->getLongName());
    m_options.push_back(std::move(option));
    return *this;
}

auto OptionGroup::getOptions() const noexcept -> const OptionList&
{
    return m_options;
}

auto OptionGroup::size() const noexcept -> std::size_t
{
    return m_options.size();
}

auto OptionGroup::empty() const noexcept -> bool
{
    return m_options.empty();
}

auto OptionGroup::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Configuring group: " << m_name);

    for (const auto& option : m_options)
    {
        if (!option->getTraits().isHidden)
        {
            option->configure(desc);
        }
    }
}

auto OptionGroup::configurePositional(
    boost::program_options::positional_options_description& positional) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    for (const auto& option : m_options)
    {
        if (option->getTraits().isPositional)
        {
            option->configurePositional(positional);
        }
    }
}

auto OptionGroup::createOptionsDescription() const -> boost::program_options::options_description
{
    boost::program_options::options_description desc(m_name);
    configure(desc);
    return desc;
}
