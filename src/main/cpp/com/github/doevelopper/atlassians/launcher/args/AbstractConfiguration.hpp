/*******************************************************************
 * @file   AbstractConfiguration.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Abstract base for configuration implementations
 *
 * Provides common functionality for configuration classes using
 * the Template Method pattern.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTCONFIGURATION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTCONFIGURATION_HPP

#include <filesystem>
#include <functional>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <com/github/doevelopper/atlassians/CompilerDetection.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/IConfiguration.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Abstract base class for configuration management
     *
     * Uses Template Method pattern to define the parsing algorithm
     * while allowing subclasses to customize option definitions.
     */
    class AbstractConfiguration : public IConfiguration
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Error callback signature
         */
        using ErrorHandler = std::function<void(const std::string& error)>;

        // ===========================================
        // IConfiguration Interface
        // ===========================================

        [[nodiscard]] auto parse(int argc, const char* argv[]) -> bool override;
        [[nodiscard]] auto isHelpRequested() const noexcept -> bool override;
        [[nodiscard]] auto isVersionRequested() const noexcept -> bool override;
        [[nodiscard]] auto getHelpMessage() const -> std::string override;
        [[nodiscard]] auto isSet(const std::string& name) const -> bool override;
        [[nodiscard]] auto getUnrecognizedArgs() const -> std::vector<std::string> override;

        // ===========================================
        // Configuration Methods
        // ===========================================

        /**
         * @brief Set the program name and description for help
         */
        auto setProgramInfo(const std::string& name, const std::string& description) -> AbstractConfiguration&;

        /**
         * @brief Set the config file path
         */
        auto setConfigFile(const std::filesystem::path& path) -> AbstractConfiguration&;

        /**
         * @brief Set environment variable prefix
         */
        auto setEnvPrefix(const std::string& prefix) -> AbstractConfiguration&;

        /**
         * @brief Allow unrecognized options
         */
        auto allowUnrecognized(bool allow = true) -> AbstractConfiguration&;

        /**
         * @brief Set error handler callback
         */
        auto onError(ErrorHandler handler) -> AbstractConfiguration&;

        /**
         * @brief Get access to options description for adding custom options
         */
        [[nodiscard]] auto getOptionsDescription() -> boost::program_options::options_description&;

        /**
         * @brief Get access to positional options description
         */
        [[nodiscard]] auto getPositionalDescription()
            -> boost::program_options::positional_options_description&;

    protected:

        Q_DISABLE_COPY_MOVE(AbstractConfiguration)
        AbstractConfiguration() noexcept;
        ~AbstractConfiguration() noexcept override;

        /**
         * @brief Hook for subclasses to define their options
         * Called before parsing begins
         */
        virtual void defineOptions();

        /**
         * @brief Hook for subclasses to validate parsed options
         * @return true if validation passes
         */
        virtual auto validateOptions() -> bool;

        /**
         * @brief Get the raw value implementation
         */
        [[nodiscard]] auto getValue(const std::string& name) const -> std::optional<std::any> override;

        /**
         * @brief Get the variables map
         */
        [[nodiscard]] auto getVariablesMap() const -> const boost::program_options::variables_map&;

        /**
         * @brief Add standard help/version options
         */
        void addStandardOptions();

    private:

        auto parseCommandLine(int argc, const char* argv[]) -> bool;
        auto parseEnvironment() -> bool;
        auto parseConfigFile() -> bool;
        void reportError(const std::string& error);

        boost::program_options::options_description m_optionsDesc{"Options"};
        boost::program_options::positional_options_description m_positionalDesc;
        boost::program_options::variables_map m_variablesMap;

        std::string m_programName;
        std::string m_programDescription;
        std::filesystem::path m_configFilePath;
        std::string m_envPrefix;
        std::vector<std::string> m_unrecognizedArgs;

        ErrorHandler m_errorHandler;
        bool m_allowUnrecognized{false};
        bool m_helpRequested{false};
        bool m_versionRequested{false};
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTCONFIGURATION_HPP
