/*******************************************************************
 * @file   ConfigurationFileOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option for specifying configuration file path
 *
 * This class represents an option that specifies a configuration file
 * from which additional options can be loaded.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_CONFIGURATIONFILEOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_CONFIGURATIONFILEOPTION_HPP

#include <filesystem>
#include <fstream>

#include <boost/program_options/parsers.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Configuration file option
     *
     * Represents an option that points to a configuration file.
     * The configuration file can contain additional options in
     * INI format (name=value pairs).
     *
     * @example
     * ```cpp
     * ConfigurationFileOption configOpt("config", "c", "Path to configuration file");
     * configOpt.withDefaultPath("/etc/myapp/config.ini");
     *
     * // After parsing, load config file options
     * if (configOpt.isSet(vm)) {
     *     auto configPath = configOpt.getFilePath(vm);
     *     auto fileVm = configOpt.parseConfigFile(configPath, optionsDesc);
     * }
     * ```
     */
    class ConfigurationFileOption : public AbstractOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct a configuration file option
         * @param longName Long option name
         * @param description Option description
         */
        explicit ConfigurationFileOption(std::string longName, std::string description);

        /**
         * @brief Construct a configuration file option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        ConfigurationFileOption(std::string longName, std::string shortName, std::string description);

        ConfigurationFileOption(const ConfigurationFileOption&) = delete;
        ConfigurationFileOption(ConfigurationFileOption&&) noexcept = default;
        ConfigurationFileOption& operator=(const ConfigurationFileOption&) = delete;
        ConfigurationFileOption& operator=(ConfigurationFileOption&&) noexcept = default;
        ~ConfigurationFileOption() noexcept override;

        // ============================================
        // Configuration File Specific Methods
        // ============================================

        /**
         * @brief Set the default configuration file path
         * @param path Default path to config file
         * @return Reference to this for chaining
         */
        auto withDefaultPath(std::filesystem::path path) -> ConfigurationFileOption&;

        /**
         * @brief Mark the configuration file as required to exist
         * @return Reference to this for chaining
         */
        auto mustExist() -> ConfigurationFileOption&;

        /**
         * @brief Allow unknown options in the config file
         * @return Reference to this for chaining
         */
        auto allowUnknownOptions() -> ConfigurationFileOption&;

        /**
         * @brief Get the configuration file path from variables map
         * @param vm The variables map
         * @return The file path, or std::nullopt if not specified
         */
        [[nodiscard]] auto getFilePath(const boost::program_options::variables_map& vm) const
            -> std::optional<std::filesystem::path>;

        /**
         * @brief Parse options from the configuration file
         * @param filePath Path to the configuration file
         * @param desc Options description for parsing
         * @param allowUnregistered Whether to allow unregistered options
         * @return Variables map with parsed options
         */
        [[nodiscard]] auto parseConfigFile(
            const std::filesystem::path& filePath,
            const boost::program_options::options_description& desc,
            bool allowUnregistered = false) const -> boost::program_options::variables_map;

        /**
         * @brief Check if the configuration file exists
         * @param vm The variables map
         * @return true if file exists or not specified, false otherwise
         */
        [[nodiscard]] auto fileExists(const boost::program_options::variables_map& vm) const -> bool;

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override;

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override;

        [[nodiscard]] auto validate(const boost::program_options::variables_map& vm) const -> bool override;

    protected:

        [[nodiscard]] auto doConfigureValue() const
            -> const boost::program_options::value_semantic* override;

    private:

        std::filesystem::path m_defaultPath;
        bool m_mustExist = false;
        bool m_allowUnknown = false;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_CONFIGURATIONFILEOPTION_HPP
