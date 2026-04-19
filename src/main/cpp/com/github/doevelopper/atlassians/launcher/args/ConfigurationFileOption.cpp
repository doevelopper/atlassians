/*******************************************************************
 * @file   ConfigurationFileOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of ConfigurationFileOption
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/ConfigurationFileOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr ConfigurationFileOption::logger = log4cxx::Logger::getLogger(
    std::string("com.github.doevelopper.premisses.launcher.args.ConfigurationFileOption"));

ConfigurationFileOption::ConfigurationFileOption(std::string longName, std::string description)
    : AbstractOption(std::move(longName), std::move(description))
    , m_defaultPath()
    , m_mustExist(false)
    , m_allowUnknown(false)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ConfigurationFileOption::ConfigurationFileOption(
    std::string longName, std::string shortName, std::string description)
    : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
    , m_defaultPath()
    , m_mustExist(false)
    , m_allowUnknown(false)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ConfigurationFileOption::~ConfigurationFileOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ConfigurationFileOption::withDefaultPath(std::filesystem::path path) -> ConfigurationFileOption&
{
    m_defaultPath = std::move(path);
    mutableTraits().hasDefault = true;
    return *this;
}

auto ConfigurationFileOption::mustExist() -> ConfigurationFileOption&
{
    m_mustExist = true;
    return *this;
}

auto ConfigurationFileOption::allowUnknownOptions() -> ConfigurationFileOption&
{
    m_allowUnknown = true;
    return *this;
}

auto ConfigurationFileOption::getFilePath(const boost::program_options::variables_map& vm) const
    -> std::optional<std::filesystem::path>
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (isSet(vm))
    {
        try
        {
            auto pathStr = vm[std::string(getLongName())].as<std::string>();
            return std::filesystem::path(pathStr);
        }
        catch (const boost::bad_any_cast&)
        {
            LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Failed to get config file path");
        }
    }

    if (!m_defaultPath.empty())
    {
        return m_defaultPath;
    }

    return std::nullopt;
}

auto ConfigurationFileOption::parseConfigFile(
    const std::filesystem::path& filePath,
    const boost::program_options::options_description& desc,
    bool allowUnregistered) const -> boost::program_options::variables_map
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing config file: " << filePath);

    boost::program_options::variables_map vm;

    if (!std::filesystem::exists(filePath))
    {
        LOG4CXX_WARN(logger, __LOG4CXX_FUNC__ << " - Config file does not exist: " << filePath);
        return vm;
    }

    std::ifstream configFile(filePath);
    if (!configFile.is_open())
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Failed to open config file: " << filePath);
        return vm;
    }

    try
    {
        auto parsed = boost::program_options::parse_config_file(configFile, desc, allowUnregistered);
        boost::program_options::store(parsed, vm);
        boost::program_options::notify(vm);

        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ << " - Successfully parsed config file: " << filePath);
    }
    catch (const boost::program_options::error& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Error parsing config file: " << e.what());
    }

    return vm;
}

auto ConfigurationFileOption::fileExists(const boost::program_options::variables_map& vm) const -> bool
{
    auto path = getFilePath(vm);
    if (!path.has_value())
    {
        return true;  // No file specified, considered OK
    }
    return std::filesystem::exists(path.value());
}

auto ConfigurationFileOption::getKind() const noexcept -> OptionKind
{
    return OptionKind::ConfigFile;
}

auto ConfigurationFileOption::getValue(const boost::program_options::variables_map& vm) const -> Value
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto path = getFilePath(vm);
    if (path.has_value())
    {
        return path.value().string();
    }
    return Value{};
}

auto ConfigurationFileOption::validate(const boost::program_options::variables_map& vm) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (m_mustExist)
    {
        auto path = getFilePath(vm);
        if (path.has_value() && !std::filesystem::exists(path.value()))
        {
            LOG4CXX_ERROR(
                logger, __LOG4CXX_FUNC__ << " - Required config file does not exist: " << path.value());
            return false;
        }
    }

    return true;
}

auto ConfigurationFileOption::doConfigureValue() const -> const boost::program_options::value_semantic*
{
    auto* value = boost::program_options::value<std::string>();

    if (!m_defaultPath.empty())
    {
        value->default_value(m_defaultPath.string());
    }

    return value;
}
