/*******************************************************************
 * @file   AbstractConfiguration.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of AbstractConfiguration
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <algorithm>
#include <cctype>
#include <fstream>
#include <locale>
#include <sstream>

#include <boost/program_options/parsers.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractConfiguration.hpp>

namespace po = boost::program_options;

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr AbstractConfiguration::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.AbstractConfiguration"));

AbstractConfiguration::AbstractConfiguration() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

AbstractConfiguration::~AbstractConfiguration() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto AbstractConfiguration::parse(int argc, const char* argv[]) -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // Let subclass define options
        addStandardOptions();
        defineOptions();

        // Parse in order of precedence (lowest to highest)
        // Later sources override earlier ones

        // 1. Parse config file (lowest priority)
        if (!m_configFilePath.empty())
        {
            parseConfigFile();
        }

        // 2. Parse environment variables
        if (!m_envPrefix.empty())
        {
            parseEnvironment();
        }

        // 3. Parse command line (highest priority)
        if (!parseCommandLine(argc, argv))
        {
            return false;
        }

        // Check for help/version
        m_helpRequested = m_variablesMap.count("help") > 0;
        m_versionRequested = m_variablesMap.count("version") > 0;

        // Validate if not just asking for help
        if (!m_helpRequested && !m_versionRequested)
        {
            if (!validateOptions())
            {
                return false;
            }
        }

        return true;
    }
    catch (const po::error& e)
    {
        reportError(e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        reportError(std::string("Configuration error: ") + e.what());
        return false;
    }
}

auto AbstractConfiguration::parseCommandLine(int argc, const char* argv[]) -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        std::vector<std::string> args(argv + 1, argv + std::max(argc, 1));

        auto parser = po::command_line_parser(args)
            .options(m_optionsDesc)
            .positional(m_positionalDesc);

        if (m_allowUnrecognized)
        {
            parser.allow_unregistered();
        }

        auto parsed = parser.run();
        po::store(parsed, m_variablesMap);
        po::notify(m_variablesMap);

        // Collect unrecognized
        m_unrecognizedArgs = po::collect_unrecognized(parsed.options, po::include_positional);

        LOG4CXX_DEBUG(logger, "Parsed " << m_variablesMap.size() << " options from command line");
        return true;
    }
    catch (const po::error& e)
    {
        reportError(e.what());
        return false;
    }
}

auto AbstractConfiguration::parseEnvironment() -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        auto envMapper = [this](const std::string& envName) -> std::string
        {
            if (envName.length() < m_envPrefix.length() ||
                envName.substr(0, m_envPrefix.length()) != m_envPrefix)
            {
                return {};
            }

            std::string optionName = envName.substr(m_envPrefix.length());

            std::transform(optionName.begin(), optionName.end(), optionName.begin(),
                [](unsigned char c) -> unsigned char
                {
                    if (c == '_') return '-';
                    return std::tolower(c, std::locale::classic());
                });

            return optionName;
        };

        auto parsed = po::parse_environment(m_optionsDesc, envMapper);
        po::store(parsed, m_variablesMap);
        po::notify(m_variablesMap);

        LOG4CXX_DEBUG(logger, "Parsed environment with prefix: " << m_envPrefix);
        return true;
    }
    catch (const po::error& e)
    {
        reportError(e.what());
        return false;
    }
}

auto AbstractConfiguration::parseConfigFile() -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!std::filesystem::exists(m_configFilePath))
    {
        LOG4CXX_DEBUG(logger, "Config file not found: " << m_configFilePath.string());
        return false;
    }

    try
    {
        std::ifstream configFile(m_configFilePath);
        if (!configFile)
        {
            LOG4CXX_WARN(logger, "Cannot open config file: " << m_configFilePath.string());
            return false;
        }

        auto parsed = po::parse_config_file(configFile, m_optionsDesc, m_allowUnrecognized);
        po::store(parsed, m_variablesMap);
        po::notify(m_variablesMap);

        LOG4CXX_DEBUG(logger, "Parsed config file: " << m_configFilePath.string());
        return true;
    }
    catch (const po::error& e)
    {
        reportError(std::string("Config file error: ") + e.what());
        return false;
    }
}

auto AbstractConfiguration::isHelpRequested() const noexcept -> bool
{
    return m_helpRequested;
}

auto AbstractConfiguration::isVersionRequested() const noexcept -> bool
{
    return m_versionRequested;
}

auto AbstractConfiguration::getHelpMessage() const -> std::string
{
    std::ostringstream oss;

    if (!m_programName.empty())
    {
        oss << m_programName;
        if (!m_programDescription.empty())
        {
            oss << " - " << m_programDescription;
        }
        oss << "\n\n";
    }

    oss << m_optionsDesc;
    return oss.str();
}

auto AbstractConfiguration::isSet(const std::string& name) const -> bool
{
    return m_variablesMap.count(name) > 0;
}

auto AbstractConfiguration::getUnrecognizedArgs() const -> std::vector<std::string>
{
    return m_unrecognizedArgs;
}

auto AbstractConfiguration::getValue(const std::string& name) const -> std::optional<std::any>
{
    if (m_variablesMap.count(name) == 0)
    {
        return std::nullopt;
    }

    try
    {
        // The variables_map contains boost::any, but we need to convert to std::any
        // We need to try known types
        const auto& value = m_variablesMap[name];

        if (value.empty())
        {
            return std::nullopt;
        }

        // Try common types
        try { return std::any(value.as<bool>()); } catch (...) {}
        try { return std::any(value.as<int>()); } catch (...) {}
        try { return std::any(value.as<long>()); } catch (...) {}
        try { return std::any(value.as<unsigned int>()); } catch (...) {}
        try { return std::any(value.as<double>()); } catch (...) {}
        try { return std::any(value.as<std::string>()); } catch (...) {}
        try { return std::any(value.as<std::vector<std::string>>()); } catch (...) {}

        // If none of the known types match, return empty
        return std::nullopt;
    }
    catch (...)
    {
        return std::nullopt;
    }
}

auto AbstractConfiguration::getVariablesMap() const -> const po::variables_map&
{
    return m_variablesMap;
}

auto AbstractConfiguration::setProgramInfo(const std::string& name, const std::string& description)
    -> AbstractConfiguration&
{
    m_programName = name;
    m_programDescription = description;
    return *this;
}

auto AbstractConfiguration::setConfigFile(const std::filesystem::path& path) -> AbstractConfiguration&
{
    m_configFilePath = path;
    return *this;
}

auto AbstractConfiguration::setEnvPrefix(const std::string& prefix) -> AbstractConfiguration&
{
    m_envPrefix = prefix;
    return *this;
}

auto AbstractConfiguration::allowUnrecognized(bool allow) -> AbstractConfiguration&
{
    m_allowUnrecognized = allow;
    return *this;
}

auto AbstractConfiguration::onError(ErrorHandler handler) -> AbstractConfiguration&
{
    m_errorHandler = std::move(handler);
    return *this;
}

auto AbstractConfiguration::getOptionsDescription() -> po::options_description&
{
    return m_optionsDesc;
}

auto AbstractConfiguration::getPositionalDescription() -> po::positional_options_description&
{
    return m_positionalDesc;
}

void AbstractConfiguration::defineOptions()
{
    // Default implementation does nothing
    // Subclasses override to add their options
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto AbstractConfiguration::validateOptions() -> bool
{
    // Default implementation accepts all configurations
    // Subclasses override to add validation
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return true;
}

void AbstractConfiguration::addStandardOptions()
{
    m_optionsDesc.add_options()
        ("help,h", "Display this help message")
        ("version,v", "Display version information");
}

void AbstractConfiguration::reportError(const std::string& error)
{
    LOG4CXX_ERROR(logger, error);
    if (m_errorHandler)
    {
        m_errorHandler(error);
    }
}
