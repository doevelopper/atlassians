/*******************************************************************
 * @file   IConfiguration.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Configuration interface for command-line parsing
 *
 * Defines the contract for configuration providers that manage
 * command-line arguments, environment variables, and config files.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ICONFIGURATION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ICONFIGURATION_HPP

#include <any>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/CompilerDetection.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Interface for application configuration
     *
     * Provides a contract for classes that manage configuration from
     * multiple sources with proper precedence.
     *
     * Configuration sources (in order of precedence):
     * 1. Command-line arguments (highest)
     * 2. Environment variables
     * 3. Configuration file
     * 4. Default values (lowest)
     */
    class IConfiguration
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using Ptr = std::shared_ptr<IConfiguration>;

        /**
         * @brief Parse command-line arguments
         * @param argc Argument count
         * @param argv Argument values
         * @return true if parsing succeeded
         */
        [[nodiscard]] virtual auto parse(int argc, const char* argv[]) -> bool = 0;

        /**
         * @brief Check if help was requested
         */
        [[nodiscard]] virtual auto isHelpRequested() const noexcept -> bool = 0;

        /**
         * @brief Check if version was requested
         */
        [[nodiscard]] virtual auto isVersionRequested() const noexcept -> bool = 0;

        /**
         * @brief Get the help message
         */
        [[nodiscard]] virtual auto getHelpMessage() const -> std::string = 0;

        /**
         * @brief Get a configuration value
         * @tparam T The value type
         * @param name The option name
         * @param defaultValue Default if not set
         * @return The value
         */
        template<typename T>
        [[nodiscard]] auto get(const std::string& name, const T& defaultValue) const -> T
        {
            if (auto value = getValue(name))
            {
                try
                {
                    return std::any_cast<T>(*value);
                }
                catch (const std::bad_any_cast&)
                {
                    return defaultValue;
                }
            }
            return defaultValue;
        }

        /**
         * @brief Check if an option is set
         * @param name The option name
         */
        [[nodiscard]] virtual auto isSet(const std::string& name) const -> bool = 0;

        /**
         * @brief Get unrecognized arguments
         */
        [[nodiscard]] virtual auto getUnrecognizedArgs() const -> std::vector<std::string> = 0;

    protected:

        Q_DISABLE_COPY_MOVE(IConfiguration)
        IConfiguration() noexcept;
        virtual ~IConfiguration() noexcept;

        /**
         * @brief Get a raw value (for template get implementation)
         */
        [[nodiscard]] virtual auto getValue(const std::string& name) const -> std::optional<std::any> = 0;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ICONFIGURATION_HPP
