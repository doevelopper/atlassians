/*******************************************************************
 * @file   DefaultConfiguration.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of DefaultConfiguration
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/value_semantic.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/DefaultConfiguration.hpp>

namespace po = boost::program_options;

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr DefaultConfiguration::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.DefaultConfiguration"));

DefaultConfiguration::DefaultConfiguration()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

DefaultConfiguration::DefaultConfiguration(const std::string& programName, const std::string& description)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    setProgramInfo(programName, description);
}

DefaultConfiguration::DefaultConfiguration(int argc, const char* argv[])
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parsed = parse(argc, argv);
}

DefaultConfiguration::DefaultConfiguration(int argc, const char* argv[], const std::string& configFile)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    setConfigFile(configFile);
    m_parsed = parse(argc, argv);
}

DefaultConfiguration::DefaultConfiguration(int argc, const char* argv[], UnparsedHandler handler)
    : m_unparsedHandler(std::move(handler))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parsed = parse(argc, argv);
}

DefaultConfiguration::DefaultConfiguration(int argc, const char* argv[],
                                           UnparsedHandler handler,
                                           const std::string& configFile)
    : m_unparsedHandler(std::move(handler))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    setConfigFile(configFile);
    m_parsed = parse(argc, argv);
}

DefaultConfiguration::~DefaultConfiguration() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void DefaultConfiguration::defineOptions()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    auto& opts = getOptionsDescription();

    // Standard options
    opts.add_options()
        ("config,c", po::value<std::string>(),
            "Path to configuration file")
        ("verbose", po::value<int>()->implicit_value(1)->default_value(0),
            "Verbosity level (0-3)")
        ("quiet,q", po::bool_switch()->default_value(false),
            "Suppress non-essential output")
        ("debug,d", po::bool_switch()->default_value(false),
            "Enable debug mode");

    // Add platform-specific options
    addPlatformOptions();
}

void DefaultConfiguration::addPlatformOptions()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

#ifdef __linux__
    // Linux-specific options
    auto& opts = getOptionsDescription();
    opts.add_options()
        ("syslog", po::bool_switch()->default_value(false),
            "Log to syslog instead of stderr");
#endif

#ifdef _WIN32
    // Windows-specific options
    auto& opts = getOptionsDescription();
    opts.add_options()
        ("eventlog", po::bool_switch()->default_value(false),
            "Log to Windows Event Log");
#endif
}

auto DefaultConfiguration::validateOptions() -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Handle unparsed arguments
    auto unrecognized = getUnrecognizedArgs();
    if (!unrecognized.empty())
    {
        if (m_unparsedHandler)
        {
            m_unparsedHandler(unrecognized);
        }
        else
        {
            LOG4CXX_WARN(logger, "Unrecognized arguments: " << unrecognized.size());
            for (const auto& arg : unrecognized)
            {
                LOG4CXX_WARN(logger, "  " << arg);
            }
        }
    }

    return true;
}

auto DefaultConfiguration::getVerbosity() const -> int
{
    return get<int>("verbose", 0);
}

auto DefaultConfiguration::isQuiet() const -> bool
{
    return get<bool>("quiet", false);
}

auto DefaultConfiguration::isDebugMode() const -> bool
{
    return get<bool>("debug", false);
}

auto DefaultConfiguration::getConfigFilePath() const -> std::string
{
    return get<std::string>("config", "");
}

auto DefaultConfiguration::setUnparsedHandler(UnparsedHandler handler) -> DefaultConfiguration&
{
    m_unparsedHandler = std::move(handler);
    return *this;
}
