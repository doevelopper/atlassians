/*******************************************************************
 * @file   Optional.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of OptionalOption decorator
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/Optional.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr OptionalOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.OptionalOption"));

OptionalOption::OptionalOption(IOption::Ptr wrapped)
    : m_wrapped(std::move(wrapped))
    , m_defaultValue()
    , m_traits()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_traits = m_wrapped->getTraits();
        m_traits.isRequired = false;
    }
}

OptionalOption::OptionalOption(IOption::Ptr wrapped, Value defaultValue)
    : m_wrapped(std::move(wrapped))
    , m_defaultValue(std::move(defaultValue))
    , m_traits()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_traits = m_wrapped->getTraits();
        m_traits.isRequired = false;
        m_traits.hasDefault = true;
    }
}

OptionalOption::~OptionalOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto OptionalOption::getDefaultValue() const noexcept -> const Value&
{
    return m_defaultValue;
}

auto OptionalOption::hasDefaultValue() const noexcept -> bool
{
    return m_defaultValue.has_value();
}

auto OptionalOption::getLongName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getLongName() : std::string_view{};
}

auto OptionalOption::getShortName() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getShortName() : std::string_view{};
}

auto OptionalOption::getDescription() const noexcept -> std::string_view
{
    return m_wrapped ? m_wrapped->getDescription() : std::string_view{};
}

auto OptionalOption::getKind() const noexcept -> OptionKind
{
    return m_wrapped ? m_wrapped->getKind() : OptionKind::Named;
}

auto OptionalOption::getTraits() const noexcept -> const OptionTraits&
{
    return m_traits;
}

auto OptionalOption::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_wrapped->configure(desc);
    }
}

auto OptionalOption::configurePositional(
    boost::program_options::positional_options_description& positional) const -> void
{
    if (m_wrapped)
    {
        m_wrapped->configurePositional(positional);
    }
}

auto OptionalOption::isSet(const boost::program_options::variables_map& vm) const noexcept -> bool
{
    return m_wrapped ? m_wrapped->isSet(vm) : false;
}

auto OptionalOption::getValue(const boost::program_options::variables_map& vm) const -> Value
{
    if (m_wrapped)
    {
        auto value = m_wrapped->getValue(vm);
        if (value.has_value())
        {
            return value;
        }
    }

    // Return default value if wrapped value is not set
    return m_defaultValue;
}

auto OptionalOption::validate(const boost::program_options::variables_map& vm) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Optional options are always valid if not set
    if (!isSet(vm))
    {
        return true;
    }

    if (m_wrapped)
    {
        return m_wrapped->validate(vm);
    }

    return true;
}

auto OptionalOption::process() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if (m_wrapped)
    {
        m_wrapped->process();
    }
}

auto OptionalOption::getWrapped() const noexcept -> const IOption::Ptr&
{
    return m_wrapped;
}
