/*******************************************************************
 * @file   ProgramOption.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of unified program options handler
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <locale>
#include <sstream>

#include <boost/program_options/parsers.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/ProgramOption.hpp>

namespace po = boost::program_options;

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr ProgramOption::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.ProgramOption"));

ProgramOption::ProgramOption()
    : AbstractOption("program", "", "Program options")
    , m_optionsDesc("Available options")
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ProgramOption::ProgramOption(std::string name, std::string description)
    : AbstractOption(std::move(name), "", std::move(description))
    , m_optionsDesc("Available options")
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ProgramOption::~ProgramOption() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ProgramOption::parseCommandLine(int argc, const char* argv[]) -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // Convert to vector of strings (skip program name)
        std::vector<std::string> args(argv + 1, argv + std::max(argc, 1));

        // Parse command line with positional options
        auto parsedOptions = po::command_line_parser(args)
            .options(m_optionsDesc)
            .positional(m_positionalDesc)
            .allow_unregistered()
            .run();

        // Store parsed options
        po::store(parsedOptions, m_variablesMap);
        po::notify(m_variablesMap);

        // Collect unrecognized options
        m_unparsedTokens = po::collect_unrecognized(
            parsedOptions.options,
            po::include_positional);

        LOG4CXX_DEBUG(logger, "Parsed " << m_variablesMap.size() << " options from command line");
        return true;
    }
    catch (const po::error& e)
    {
        LOG4CXX_ERROR(logger, "Command line parsing error: " << e.what());
        return false;
    }
}

auto ProgramOption::parseEnvironment(const std::string& prefix) -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // Environment variable name mapper
        auto envMapper = [&prefix](const std::string& envName) -> std::string
        {
            // Check if env var starts with prefix
            if (envName.length() < prefix.length() ||
                envName.substr(0, prefix.length()) != prefix)
            {
                return {}; // Ignore env vars without the prefix
            }

            // Extract the option name (after prefix)
            std::string optionName = envName.substr(prefix.length());

            // Convert: OPTION_NAME -> option-name
            std::transform(optionName.begin(), optionName.end(), optionName.begin(),
                [](unsigned char c)
                {
                    if (c == '_')
                    {
                        return static_cast<unsigned char>('-');
                    }
                    return static_cast<unsigned char>(std::tolower(c, std::locale::classic()));
                });

            return optionName;
        };

        auto parsedOptions = po::parse_environment(m_optionsDesc, envMapper);
        po::store(parsedOptions, m_variablesMap);
        po::notify(m_variablesMap);

        LOG4CXX_DEBUG(logger, "Parsed environment variables with prefix '" << prefix << "'");
        return true;
    }
    catch (const po::error& e)
    {
        LOG4CXX_ERROR(logger, "Environment parsing error: " << e.what());
        return false;
    }
}

auto ProgramOption::parseConfigFile(const std::filesystem::path& filePath, bool allowUnknown) -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // First check the specified path
        if (std::filesystem::exists(filePath))
        {
            std::ifstream configFile(filePath);
            if (configFile)
            {
                auto parsedOptions = po::parse_config_file(configFile, m_optionsDesc, allowUnknown);
                po::store(parsedOptions, m_variablesMap);
                po::notify(m_variablesMap);

                LOG4CXX_DEBUG(logger, "Parsed config file: " << filePath.string());
                return true;
            }
        }

        // Try XDG configuration directories
        std::string configRoots;

        if (const auto* configHome = std::getenv("XDG_CONFIG_HOME"))
        {
            configRoots = std::string(configHome) + ":";
        }
        else if (const auto* home = std::getenv("HOME"))
        {
            configRoots = std::string(home) + "/.config:";
        }

        if (const auto* configDirs = std::getenv("XDG_CONFIG_DIRS"))
        {
            configRoots += configDirs;
        }
        else
        {
            configRoots += "/etc/xdg";
        }

        LOG4CXX_DEBUG(logger, "Searching config directories: " << configRoots);

        std::istringstream configStream(configRoots);
        std::string configDir;

        while (std::getline(configStream, configDir, ':'))
        {
            std::filesystem::path searchPath = std::filesystem::path(configDir) / filePath.filename();

            if (std::filesystem::exists(searchPath))
            {
                std::ifstream configFile(searchPath);
                if (configFile)
                {
                    auto parsedOptions = po::parse_config_file(configFile, m_optionsDesc, allowUnknown);
                    po::store(parsedOptions, m_variablesMap);
                    po::notify(m_variablesMap);

                    LOG4CXX_INFO(logger, "Parsed config file from XDG path: " << searchPath.string());
                    return true;
                }
            }
        }

        LOG4CXX_WARN(logger, "Config file not found: " << filePath.string());
        return false;
    }
    catch (const po::error& e)
    {
        LOG4CXX_ERROR(logger, "Config file parsing error in " << filePath.string() << ": " << e.what());
        return false;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, "Error reading config file " << filePath.string() << ": " << e.what());
        return false;
    }
}

auto ProgramOption::isOptionSet(const std::string& name) const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return m_variablesMap.count(name) > 0;
}

auto ProgramOption::getVariablesMap() const -> const po::variables_map&
{
    return m_variablesMap;
}

auto ProgramOption::getOptionsDescription() -> po::options_description&
{
    return m_optionsDesc;
}

auto ProgramOption::getPositionalDescription() -> po::positional_options_description&
{
    return m_positionalDesc;
}

auto ProgramOption::getUnparsedTokens() const -> const std::vector<std::string>&
{
    return m_unparsedTokens;
}

auto ProgramOption::getKind() const noexcept -> OptionKind
{
    return OptionKind::Named;
}

auto ProgramOption::getValue(const po::variables_map& /*vm*/) const -> Value
{
    // ProgramOption is a container, not a single value
    return std::nullopt;
}
