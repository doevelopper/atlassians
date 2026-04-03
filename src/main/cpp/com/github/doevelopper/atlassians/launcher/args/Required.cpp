/*******************************************************************
 * @file   Required.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of RequiredOption decorator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/Required.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr RequiredOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.RequiredOption"));

RequiredOption::RequiredOption(IOption::Ptr wrapped)
    : m_wrapped(std::move(wrapped))
    , m_traits()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_traits = m_wrapped->getTraits();
        m_traits.isRequired = true;
    }
}

RequiredOption::~RequiredOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto RequiredOption::getLongName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getLongName() : std::string_view{};
}

auto RequiredOption::getShortName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getShortName() : std::string_view{};
}

auto RequiredOption::getDescription() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getDescription() : std::string_view{};
}

auto RequiredOption::getKind() const noexcept -> OptionKind
{
    return m_wrapped ? m_wrapped->getKind() : OptionKind::Named;
}

auto RequiredOption::getTraits() const noexcept -> const OptionTraits&
{
    return m_traits;
}

auto RequiredOption::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_wrapped->configure(desc);
    }
}

auto RequiredOption::configurePositional(
    boost::program_options::positional_options_description& positional) const -> void
{
    if (m_wrapped)
    {
        m_wrapped->configurePositional(positional);
    }
}

auto RequiredOption::isSet(const boost::program_options::variables_map& vm) const noexcept -> bool
{
    return m_wrapped ? m_wrapped->isSet(vm) : false;
}

auto RequiredOption::getValue(const boost::program_options::variables_map& vm) const -> Value
{
    return m_wrapped ? m_wrapped->getValue(vm) : Value{};
}

auto RequiredOption::validate(const boost::program_options::variables_map& vm) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!isSet(vm))
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Required option not set: " << getLongName());
        return false;
    }

    if (m_wrapped)
    {
        return m_wrapped->validate(vm);
    }

    return true;
}

auto RequiredOption::process() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_wrapped->process();
    }
}

auto RequiredOption::checkRequired(const boost::program_options::variables_map& vm) const -> void
{
    if (!isSet(vm))
    {
        throw RequiredOptionMissingException(std::string(getLongName()));
    }
}

auto RequiredOption::getWrapped() const noexcept -> const IOption::Ptr&
{
    return m_wrapped;
}
