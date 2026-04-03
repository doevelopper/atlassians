/*******************************************************************
 * @file   ProgramOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unified program options handler
 *
 * This class provides a unified interface for handling options from
 * multiple sources: command line, environment, and configuration files.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_PROGRAMOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_PROGRAMOPTION_HPP

#include <filesystem>
#include <string>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Source for option values
     */
    enum class OptionSource : std::uint8_t
    {
        CommandLine,    ///< From command-line arguments
        Environment,    ///< From environment variables
        ConfigFile,     ///< From configuration file
        Default         ///< Default value
    };

    /**
     * @brief Unified program options manager
     *
     * Coordinates parsing from multiple sources with proper precedence:
     * 1. Command line (highest priority)
     * 2. Environment variables
     * 3. Configuration file
     * 4. Default values (lowest priority)
     *
     * @example
     * ```cpp
     * ProgramOption progOpt;
     *
     * // Configure options
     * auto& desc = progOpt.getOptionsDescription();
     * desc.add_options()
     *     ("config,c", po::value<std::string>(), "Config file")
     *     ("port,p", po::value<int>()->default_value(8080), "Port");
     *
     * // Parse from all sources
     * progOpt.parseCommandLine(argc, argv);
     * progOpt.parseEnvironment("MYAPP_");
     * progOpt.parseConfigFile(configPath);
     *
     * // Access values
     * int port = progOpt.get<int>("port", 8080);
     * ```
     */
    class ProgramOption : public AbstractOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        ProgramOption();

        /**
         * @brief Construct with a name and description
         * @param name Program name
         * @param description Program description
         */
        ProgramOption(std::string name, std::string description);

        ProgramOption(const ProgramOption&) = delete;
        ProgramOption(ProgramOption&&) noexcept = default;
        ProgramOption& operator=(const ProgramOption&) = delete;
        ProgramOption& operator=(ProgramOption&&) noexcept = default;
        ~ProgramOption() noexcept override;

        // ============================================
        // Parsing Methods
        // ============================================

        /**
         * @brief Parse command-line arguments
         * @param argc Argument count
         * @param argv Argument values
         * @return true if successful, false on error
         */
        auto parseCommandLine(int argc, const char* argv[]) -> bool;

        /**
         * @brief Parse environment variables with prefix
         * @param prefix The prefix to match (e.g., "MYAPP_")
         * @return true if successful, false on error
         */
        auto parseEnvironment(const std::string& prefix) -> bool;

        /**
         * @brief Parse configuration file
         * @param filePath Path to the configuration file
         * @param allowUnknown Whether to allow unknown options
         * @return true if successful, false on error
         */
        auto parseConfigFile(const std::filesystem::path& filePath, bool allowUnknown = false) -> bool;

        // ============================================
        // Value Access
        // ============================================

        /**
         * @brief Check if an option is set
         * @param name The option name
         * @return true if the option is set
         */
        [[nodiscard]] auto isOptionSet(const std::string& name) const -> bool;

        /**
         * @brief Get a typed option value
         * @tparam T The value type
         * @param name The option name
         * @param defaultValue Default value if not set
         * @return The value
         */
        template<typename T>
        [[nodiscard]] auto get(const std::string& name, const T& defaultValue) const -> T
        {
            if (m_variablesMap.count(name) == 0)
            {
                return defaultValue;
            }

            try
            {
                return m_variablesMap[name].as<T>();
            }
            catch (const boost::bad_any_cast&)
            {
                return defaultValue;
            }
        }

        /**
         * @brief Get an optional typed value
         * @tparam T The value type
         * @param name The option name
         * @return The value or std::nullopt
         */
        template<typename T>
        [[nodiscard]] auto get(const std::string& name) const -> std::optional<T>
        {
            if (m_variablesMap.count(name) == 0)
            {
                return std::nullopt;
            }

            try
            {
                return m_variablesMap[name].as<T>();
            }
            catch (const boost::bad_any_cast&)
            {
                return std::nullopt;
            }
        }

        /**
         * @brief Get the underlying variables map
         */
        [[nodiscard]] auto getVariablesMap() const -> const boost::program_options::variables_map&;

        /**
         * @brief Get mutable access to options description for configuration
         */
        [[nodiscard]] auto getOptionsDescription() -> boost::program_options::options_description&;

        /**
         * @brief Get mutable positional options description
         */
        [[nodiscard]] auto getPositionalDescription()
            -> boost::program_options::positional_options_description&;

        /**
         * @brief Get unparsed tokens (unrecognized arguments)
         */
        [[nodiscard]] auto getUnparsedTokens() const -> const std::vector<std::string>&;

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override;

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override;

    private:

        boost::program_options::options_description m_optionsDesc;
        boost::program_options::positional_options_description m_positionalDesc;
        boost::program_options::variables_map m_variablesMap;
        std::vector<std::string> m_unparsedTokens;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_PROGRAMOPTION_HPP
