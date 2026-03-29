/*******************************************************************
 * @file   CommandLineParser.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Main command-line argument parser (Facade pattern)
 *
 * This class provides a high-level interface for parsing command-line
 * arguments. It acts as a Facade, coordinating the various option types
 * and providing a simple API for clients.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_COMMANDLINEPARSER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_COMMANDLINEPARSER_HPP

#include <functional>
#include <memory>
#include <vector>

#include <boost/program_options.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/BooleanOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/ConfigurationFileOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/Grouped.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/HiddenOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/MultiValueOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/PositionalOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/Required.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/TypedOption.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Result of command-line parsing
     */
    struct ParseResult
    {
        bool success = false;
        bool helpRequested = false;
        bool versionRequested = false;
        std::string errorMessage;
        std::vector<std::string> unrecognizedOptions;
        boost::program_options::variables_map variablesMap;
    };

    /**
     * @brief High-level command-line argument parser
     *
     * Provides a unified interface for configuring and parsing command-line
     * arguments. Supports all option types from the framework.
     *
     * @example
     * ```cpp
     * CommandLineParser parser("MyApp", "My Application v1.0");
     *
     * // Add options
     * parser.addOption<BooleanOption>("help", "h", "Show help message");
     * parser.addOption<BooleanOption>("version", "V", "Show version");
     * parser.addOption<StringOption>("config", "c", "Config file path")
     *       .withDefaultValue("/etc/myapp.conf");
     *
     * // Parse
     * auto result = parser.parse(argc, argv);
     *
     * if (result.helpRequested) {
     *     parser.printHelp(std::cout);
     *     return 0;
     * }
     *
     * // Access values
     * auto config = parser.getValue<std::string>("config");
     * ```
     */
    class CommandLineParser
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using OptionList = std::vector<IOption::Ptr>;
        using GroupList = std::vector<std::unique_ptr<OptionGroup>>;
        using ErrorHandler = std::function<void(const std::string&)>;
        using UnrecognizedHandler = std::function<void(const std::vector<std::string>&)>;

        /**
         * @brief Construct a command-line parser
         * @param programName The program name for help display
         * @param description Program description for help display
         */
        explicit CommandLineParser(std::string programName = "", std::string description = "") noexcept;

        CommandLineParser(const CommandLineParser&) = delete;
        CommandLineParser(CommandLineParser&&) noexcept = default;
        CommandLineParser& operator=(const CommandLineParser&) = delete;
        CommandLineParser& operator=(CommandLineParser&&) noexcept = default;
        ~CommandLineParser() noexcept;

        // ============================================
        // Option Registration
        // ============================================

        /**
         * @brief Add an existing option
         * @param option The option to add
         * @return Reference to this for chaining
         */
        auto addOption(IOption::Ptr option) -> CommandLineParser&;

        /**
         * @brief Create and add an option
         * @tparam OptionT The option type to create
         * @tparam Args Constructor argument types
         * @param args Arguments forwarded to the option constructor
         * @return Reference to the created option
         */
        template<typename OptionT, typename... Args>
        auto addOption(Args&&... args) -> OptionT&
        {
            auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
            auto& ref = *option;
            m_options.push_back(std::move(option));
            return ref;
        }

        /**
         * @brief Add an option group
         * @param name Group name
         * @param description Group description
         * @return Reference to the created group
         */
        auto addGroup(std::string name, std::string description = "") -> OptionGroup&;

        /**
         * @brief Add standard help and version options
         * @param version The version string to display
         * @return Reference to this for chaining
         */
        auto addStandardOptions(const std::string& version = "") -> CommandLineParser&;

        // ============================================
        // Parsing
        // ============================================

        /**
         * @brief Parse command-line arguments
         * @param argc Argument count
         * @param argv Argument values
         * @return Parse result containing success status and parsed values
         */
        [[nodiscard]] auto parse(int argc, const char* argv[]) -> ParseResult;

        /**
         * @brief Parse command-line arguments (non-const version)
         * @param argc Argument count
         * @param argv Argument values
         * @return Parse result
         */
        [[nodiscard]] auto parse(int argc, char* argv[]) -> ParseResult;

        /**
         * @brief Parse and merge configuration file options
         * @param filePath Path to configuration file
         * @param result Existing parse result to merge into
         * @param allowUnknown Whether to allow unknown options in config file
         */
        auto parseConfigFile(
            const std::filesystem::path& filePath,
            ParseResult& result,
            bool allowUnknown = false) -> void;

        /**
         * @brief Parse and merge environment variables
         * @param prefix Environment variable prefix to match
         * @param result Existing parse result to merge into
         */
        auto parseEnvironment(const std::string& prefix, ParseResult& result) -> void;

        // ============================================
        // Value Access
        // ============================================

        /**
         * @brief Check if an option was specified
         * @param name The option name
         * @return true if the option was specified
         */
        [[nodiscard]] auto isSet(const std::string& name) const -> bool;

        /**
         * @brief Get a typed value from the parsed results
         * @tparam T The expected value type
         * @param name The option name
         * @return The value, or std::nullopt if not set or wrong type
         */
        template<typename T>
        [[nodiscard]] auto getValue(const std::string& name) const -> std::optional<T>
        {
            if (!m_parsed)
            {
                return std::nullopt;
            }

            // Special handling for boolean options (flags)
            if constexpr (std::is_same_v<T, bool>)
            {
                // For boolean flags, we check if option is present (flag set) or absent (flag not set)
                return m_variablesMap.count(name) > 0;
            }
            else
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
        }

        /**
         * @brief Get a typed value with default
         * @tparam T The expected value type
         * @param name The option name
         * @param defaultValue Default value if not set
         * @return The value, or default if not set
         */
        template<typename T>
        [[nodiscard]] auto getValue(const std::string& name, const T& defaultValue) const -> T
        {
            auto value = getValue<T>(name);
            return value.value_or(defaultValue);
        }

        /**
         * @brief Get the variables map directly
         */
        [[nodiscard]] auto getVariablesMap() const -> const boost::program_options::variables_map&;

        // ============================================
        // Help and Display
        // ============================================

        /**
         * @brief Print help message
         * @param os Output stream
         */
        auto printHelp(std::ostream& os) const -> void;

        /**
         * @brief Print version message
         * @param os Output stream
         */
        auto printVersion(std::ostream& os) const -> void;

        /**
         * @brief Get the help text as string
         */
        [[nodiscard]] auto getHelpText() const -> std::string;

        // ============================================
        // Configuration
        // ============================================

        /**
         * @brief Set whether to allow unrecognized options
         * @param allow true to allow, false to fail on unrecognized
         * @return Reference to this for chaining
         */
        auto allowUnrecognized(bool allow = true) -> CommandLineParser&;

        /**
         * @brief Set error handler
         * @param handler Function called on parse errors
         * @return Reference to this for chaining
         */
        auto onError(ErrorHandler handler) -> CommandLineParser&;

        /**
         * @brief Set handler for unrecognized options
         * @param handler Function called with list of unrecognized options
         * @return Reference to this for chaining
         */
        auto onUnrecognized(UnrecognizedHandler handler) -> CommandLineParser&;

        /**
         * @brief Set the program name
         * @param name The program name
         * @return Reference to this for chaining
         */
        auto setProgramName(std::string name) -> CommandLineParser&;

        /**
         * @brief Set the program description
         * @param description The description
         * @return Reference to this for chaining
         */
        auto setDescription(std::string description) -> CommandLineParser&;

        /**
         * @brief Set the version string
         * @param version The version string
         * @return Reference to this for chaining
         */
        auto setVersion(std::string version) -> CommandLineParser&;

    private:

        /**
         * @brief Build the options description from registered options
         */
        auto buildOptionsDescription() const -> boost::program_options::options_description;

        /**
         * @brief Build the positional options description
         */
        auto buildPositionalDescription() const -> boost::program_options::positional_options_description;

        /**
         * @brief Validate all required options are present
         */
        [[nodiscard]] auto validateRequired() const -> bool;

        std::string m_programName;
        std::string m_description;
        std::string m_version;

        OptionList m_options;
        GroupList m_groups;
        OptionList m_hiddenOptions;

        boost::program_options::variables_map m_variablesMap;

        bool m_parsed = false;
        bool m_allowUnrecognized = false;

        ErrorHandler m_errorHandler;
        UnrecognizedHandler m_unrecognizedHandler;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_COMMANDLINEPARSER_HPP
