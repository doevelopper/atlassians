/*******************************************************************
 * @file   CommandLineParser.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of CommandLineParser
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <sstream>

#include <boost/program_options/parsers.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/CommandLineParser.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/EnvironmentOption.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr CommandLineParser::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.CommandLineParser"));

CommandLineParser::CommandLineParser(std::string programName, std::string description) noexcept
    : m_programName(std::move(programName))
    , m_description(std::move(description))
    , m_version()
    , m_options()
    , m_groups()
    , m_hiddenOptions()
    , m_variablesMap()
    , m_parsed(false)
    , m_allowUnrecognized(false)
    , m_errorHandler()
    , m_unrecognizedHandler()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CommandLineParser::~CommandLineParser() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto CommandLineParser::addOption(IOption::Ptr option) -> CommandLineParser&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Adding option: " << option->getLongName());

    if (option->getTraits().isHidden)
    {
        m_hiddenOptions.push_back(std::move(option));
    }
    else
    {
        m_options.push_back(std::move(option));
    }
    return *this;
}

auto CommandLineParser::addGroup(std::string name, std::string description) -> OptionGroup&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Adding group: " << name);
    m_groups.push_back(std::make_unique<OptionGroup>(std::move(name), std::move(description)));
    return *m_groups.back();
}

auto CommandLineParser::addStandardOptions(const std::string& version) -> CommandLineParser&
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!version.empty())
    {
        m_version = version;
    }

    // Add help option
    addOption<BooleanOption>("help", "h", "Display this help message and exit");

    // Always add version option
    addOption<BooleanOption>("version", "V", "Display version information and exit");

    return *this;
}

auto CommandLineParser::parse(int argc, const char* argv[]) -> ParseResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return parse(argc, const_cast<char**>(argv));
}

auto CommandLineParser::parse(int argc, char* argv[]) -> ParseResult
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    ParseResult result;
    result.success = false;

    try
    {
        // Build options descriptions
        auto visibleDesc = buildOptionsDescription();
        auto positionalDesc = buildPositionalDescription();

        // Create a combined description including hidden options
        boost::program_options::options_description allDesc;
        allDesc.add(visibleDesc);

        // Add hidden options to the combined description
        for (const auto& option : m_hiddenOptions)
        {
            option->configure(allDesc);
        }

        // Create the parser
        auto parser = boost::program_options::command_line_parser(argc, argv)
                          .options(allDesc)
                          .positional(positionalDesc);

        if (m_allowUnrecognized)
        {
            parser.allow_unregistered();
        }

        // Parse
        auto parsed = parser.run();

        // Collect unrecognized options
        result.unrecognizedOptions =
            boost::program_options::collect_unrecognized(parsed.options, boost::program_options::include_positional);

        // Filter out recognized positional options from unrecognized list
        // (boost includes them in unrecognized when allow_unregistered is set)

        // Store parsed options
        boost::program_options::store(parsed, m_variablesMap);
        boost::program_options::notify(m_variablesMap);

        result.variablesMap = m_variablesMap;
        m_parsed = true;

        // Check for help/version
        if (m_variablesMap.count("help") > 0)
        {
            result.helpRequested = true;
            result.success = true;
            return result;
        }

        if (m_variablesMap.count("version") > 0)
        {
            result.versionRequested = true;
            result.success = true;
            return result;
        }

        // Handle unrecognized options
        if (!result.unrecognizedOptions.empty() && !m_allowUnrecognized)
        {
            std::ostringstream oss;
            oss << "Unrecognized options: ";
            for (const auto& opt : result.unrecognizedOptions)
            {
                oss << opt << " ";
            }
            result.errorMessage = oss.str();

            if (m_unrecognizedHandler)
            {
                m_unrecognizedHandler(result.unrecognizedOptions);
            }
            if (m_errorHandler)
            {
                m_errorHandler(result.errorMessage);
            }

            return result;
        }

        // Validate required options
        if (!validateRequired())
        {
            result.errorMessage = "Missing required options";
            if (m_errorHandler)
            {
                m_errorHandler(result.errorMessage);
            }
            return result;
        }

        // Validate all options
        for (const auto& option : m_options)
        {
            if (!option->validate(m_variablesMap))
            {
                result.errorMessage = "Validation failed for option: " + std::string(option->getLongName());
                if (m_errorHandler)
                {
                    m_errorHandler(result.errorMessage);
                }
                return result;
            }
        }

        result.success = true;
    }
    catch (const boost::program_options::error& e)
    {
        result.errorMessage = e.what();
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Parse error: " << e.what());

        if (m_errorHandler)
        {
            m_errorHandler(result.errorMessage);
        }
    }
    catch (const std::exception& e)
    {
        result.errorMessage = e.what();
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Unexpected error: " << e.what());

        if (m_errorHandler)
        {
            m_errorHandler(result.errorMessage);
        }
    }

    return result;
}

auto CommandLineParser::parseConfigFile(
    const std::filesystem::path& filePath,
    ParseResult& result,
    bool allowUnknown) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing config file: " << filePath);

    if (!std::filesystem::exists(filePath))
    {
        LOG4CXX_WARN(logger, __LOG4CXX_FUNC__ << " - Config file not found: " << filePath);
        return;
    }

    try
    {
        auto desc = buildOptionsDescription();
        std::ifstream configFile(filePath);

        if (!configFile.is_open())
        {
            LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Failed to open config file: " << filePath);
            return;
        }

        auto parsed = boost::program_options::parse_config_file(configFile, desc, allowUnknown);
        boost::program_options::store(parsed, m_variablesMap);
        boost::program_options::notify(m_variablesMap);

        result.variablesMap = m_variablesMap;
    }
    catch (const boost::program_options::error& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Error parsing config file: " << e.what());
        result.errorMessage += "; Config file error: " + std::string(e.what());
    }
}

auto CommandLineParser::parseEnvironment(const std::string& prefix, ParseResult& result) -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ << " - Parsing environment with prefix: " << prefix);

    try
    {
        auto desc = buildOptionsDescription();
        auto mapper = EnvironmentOption<std::string>::createEnvMapper(prefix);

        auto parsed = boost::program_options::parse_environment(desc, mapper);
        boost::program_options::store(parsed, m_variablesMap);
        boost::program_options::notify(m_variablesMap);

        result.variablesMap = m_variablesMap;
    }
    catch (const boost::program_options::error& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " - Error parsing environment: " << e.what());
        result.errorMessage += "; Environment error: " + std::string(e.what());
    }
}

auto CommandLineParser::isSet(const std::string& name) const -> bool
{
    return m_parsed && m_variablesMap.count(name) > 0;
}

auto CommandLineParser::getVariablesMap() const -> const boost::program_options::variables_map&
{
    return m_variablesMap;
}

auto CommandLineParser::printHelp(std::ostream& os) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!m_programName.empty())
    {
        os << "Usage: " << m_programName << " [options]";

        // Add positional arguments to usage line
        for (const auto& option : m_options)
        {
            if (option->getTraits().isPositional)
            {
                os << " <" << option->getLongName() << ">";
            }
        }
        os << "\n\n";
    }

    if (!m_description.empty())
    {
        os << m_description << "\n\n";
    }

    // Print grouped options
    for (const auto& group : m_groups)
    {
        os << group->createOptionsDescription() << "\n";
    }

    // Print ungrouped options
    auto desc = buildOptionsDescription();
    os << desc << "\n";
}

auto CommandLineParser::printVersion(std::ostream& os) const -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (!m_programName.empty())
    {
        os << m_programName;
    }

    if (!m_version.empty())
    {
        os << " version " << m_version;
    }

    os << "\n";
}

auto CommandLineParser::getHelpText() const -> std::string
{
    std::ostringstream oss;
    printHelp(oss);
    return oss.str();
}

auto CommandLineParser::allowUnrecognized(bool allow) -> CommandLineParser&
{
    m_allowUnrecognized = allow;
    return *this;
}

auto CommandLineParser::onError(ErrorHandler handler) -> CommandLineParser&
{
    m_errorHandler = std::move(handler);
    return *this;
}

auto CommandLineParser::onUnrecognized(UnrecognizedHandler handler) -> CommandLineParser&
{
    m_unrecognizedHandler = std::move(handler);
    return *this;
}

auto CommandLineParser::setProgramName(std::string name) -> CommandLineParser&
{
    m_programName = std::move(name);
    return *this;
}

auto CommandLineParser::setDescription(std::string description) -> CommandLineParser&
{
    m_description = std::move(description);
    return *this;
}

auto CommandLineParser::setVersion(std::string version) -> CommandLineParser&
{
    m_version = std::move(version);
    return *this;
}

auto CommandLineParser::buildOptionsDescription() const -> boost::program_options::options_description
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    boost::program_options::options_description desc("Options");

    for (const auto& option : m_options)
    {
        if (!option->getTraits().isHidden)
        {
            option->configure(desc);
        }
    }

    return desc;
}

auto CommandLineParser::buildPositionalDescription() const
    -> boost::program_options::positional_options_description
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    boost::program_options::positional_options_description positional;

    for (const auto& option : m_options)
    {
        if (option->getTraits().isPositional)
        {
            option->configurePositional(positional);
        }
    }

    // Also check groups
    for (const auto& group : m_groups)
    {
        group->configurePositional(positional);
    }

    return positional;
}

auto CommandLineParser::validateRequired() const -> bool
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    for (const auto& option : m_options)
    {
        if (option->getTraits().isRequired && !option->isSet(m_variablesMap))
        {
            LOG4CXX_ERROR(
                logger, __LOG4CXX_FUNC__ << " - Required option missing: " << option->getLongName());
            return false;
        }
    }

    return true;
}
