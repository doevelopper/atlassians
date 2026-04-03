/*******************************************************************
 * @file   AbstractOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of AbstractOption base class
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr AbstractOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.AbstractOption"));

AbstractOption::AbstractOption(std::string longName, std::string description) noexcept
    : m_longName(std::move(longName))
    , m_shortName()
    , m_description(std::move(description))
    , m_traits()
    , m_validator()
    , m_notifier()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AbstractOption::AbstractOption(std::string longName, std::string shortName, std::string description) noexcept
    : m_longName(std::move(longName))
    , m_shortName(std::move(shortName))
    , m_description(std::move(description))
    , m_traits()
    , m_validator()
    , m_notifier()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AbstractOption::~AbstractOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto AbstractOption::getLongName() const noexcept -> std::string_view
{
    return m_longName;
}

auto AbstractOption::getShortName() const noexcept -> std::string_view
{
    return m_shortName;
}

auto AbstractOption::getDescription() const noexcept -> std::string_view
{
    return m_description;
}

auto AbstractOption::getTraits() const noexcept -> const OptionTraits&
{
    return m_traits;
}

auto AbstractOption::configure(boost::program_options::options_description& desc) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::string formattedName = getFormattedName();
    auto* valueSemantic = doConfigureValue();

    if (valueSemantic != nullptr)
    {
        desc.add_options()(formattedName.c_str(), valueSemantic, m_description.c_str());
    }
    else
    {
        // Boolean switch with no value
        desc.add_options()(formattedName.c_str(), m_description.c_str());
    }

    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " - Configured option: " << formattedName);
}

auto AbstractOption::process() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // Default implementation: nothing to process
}

auto AbstractOption::required() -> AbstractOption&
{
    m_traits.isRequired = true;
    return *this;
}

auto AbstractOption::hidden() -> AbstractOption&
{
    m_traits.isHidden = true;
    return *this;
}

auto AbstractOption::withValidator(Validator validator) -> AbstractOption&
{
    m_validator = std::move(validator);
    return *this;
}

auto AbstractOption::withNotifier(Notifier notifier) -> AbstractOption&
{
    m_notifier = std::move(notifier);
    return *this;
}

auto AbstractOption::getValidator() const noexcept -> const std::optional<Validator>&
{
    return m_validator;
}

auto AbstractOption::getNotifier() const noexcept -> const std::optional<Notifier>&
{
    return m_notifier;
}

auto AbstractOption::doConfigureValue() const -> const boost::program_options::value_semantic*
{
    // Default: no value semantic (boolean switch)
    return nullptr;
}

auto AbstractOption::mutableTraits() -> OptionTraits&
{
    return m_traits;
}
