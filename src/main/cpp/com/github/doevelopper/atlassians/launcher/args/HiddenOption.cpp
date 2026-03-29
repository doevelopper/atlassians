/*******************************************************************
 * @file   HiddenOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of HiddenOption decorator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/HiddenOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr HiddenOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.HiddenOption"));

HiddenOption::HiddenOption(IOption::Ptr wrapped)
    : m_wrapped(std::move(wrapped))
    , m_traits()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_traits = m_wrapped->getTraits();
        m_traits.isHidden = true;
    }
}

HiddenOption::~HiddenOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto HiddenOption::getLongName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getLongName() : std::string_view{};
}

auto HiddenOption::getShortName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getShortName() : std::string_view{};
}

auto HiddenOption::getDescription() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getDescription() : std::string_view{};
}

auto HiddenOption::getKind() const noexcept -> OptionKind
{
    return OptionKind::Hidden;
}

auto HiddenOption::getTraits() const noexcept -> const OptionTraits&
{
    return m_traits;
}

auto HiddenOption::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // Hidden options are added to a separate hidden options description
    // For now, we still configure them but they won't appear in help
    if (m_wrapped)
    {
        m_wrapped->configure(desc);
    }
}

auto HiddenOption::configurePositional(
    boost::program_options::positional_options_description& positional) const -> void
{
    if (m_wrapped)
    {
        m_wrapped->configurePositional(positional);
    }
}

auto HiddenOption::isSet(const boost::program_options::variables_map& vm) const noexcept -> bool
{
    return m_wrapped ? m_wrapped->isSet(vm) : false;
}

auto HiddenOption::getValue(const boost::program_options::variables_map& vm) const -> Value
{
    return m_wrapped ? m_wrapped->getValue(vm) : Value{};
}

auto HiddenOption::validate(const boost::program_options::variables_map& vm) const -> bool
{
    return m_wrapped ? m_wrapped->validate(vm) : true;
}

auto HiddenOption::process() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_wrapped->process();
    }
}

auto HiddenOption::getWrapped() const noexcept -> const IOption::Ptr&
{
    return m_wrapped;
}
