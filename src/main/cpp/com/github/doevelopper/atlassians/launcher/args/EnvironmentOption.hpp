/*******************************************************************
 * @file   EnvironmentOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option that can be set from environment variables
 *
 * This class represents an option that can read its value from
 * an environment variable as an alternative to command-line specification.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ENVIRONMENTOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ENVIRONMENTOPTION_HPP

#include <cstdlib>
#include <functional>

#include <boost/program_options/environment_iterator.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Option that can be set from environment variables
     *
     * This option type can read its value from an environment variable
     * when not specified on the command line. Supports prefix-based
     * environment variable mapping.
     *
     * @tparam T The value type
     *
     * @example
     * ```cpp
     * // Map --log-level to APP_LOG_LEVEL environment variable
     * EnvironmentOption<std::string> logLevel("log-level", "l", "Logging level");
     * logLevel.fromEnv("APP_LOG_LEVEL").withDefaultValue("info");
     *
     * // Or use automatic prefix mapping
     * EnvironmentOption<int> port("port", "p", "Server port");
     * port.withEnvPrefix("MYAPP_");  // Maps to MYAPP_PORT
     * ```
     */
    template<typename T>
    class EnvironmentOption : public AbstractOption
    {
    public:

        using ValueType = T;

        /**
         * @brief Environment variable name mapper function type
         *
         * Takes an option name and returns the corresponding environment
         * variable name, or empty string if not mapped.
         */
        using EnvMapper = std::function<std::string(const std::string&)>;

        /**
         * @brief Construct an environment option
         * @param longName Long option name
         * @param description Option description
         */
        explicit EnvironmentOption(std::string longName, std::string description)
            : AbstractOption(std::move(longName), std::move(description))
            , m_envVarName()
            , m_envPrefix()
            , m_defaultValue()
        {
        }

        /**
         * @brief Construct an environment option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        EnvironmentOption(std::string longName, std::string shortName, std::string description)
            : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
            , m_envVarName()
            , m_envPrefix()
            , m_defaultValue()
        {
        }

        ~EnvironmentOption() noexcept override = default;

        // ============================================
        // Environment Option Specific Methods
        // ============================================

        /**
         * @brief Set the explicit environment variable name
         * @param envVarName The environment variable name
         * @return Reference to this for chaining
         */
        auto fromEnv(std::string envVarName) -> EnvironmentOption&
        {
            m_envVarName = std::move(envVarName);
            return *this;
        }

        /**
         * @brief Set the environment variable prefix for automatic mapping
         *
         * The option name will be converted to uppercase and prefixed
         * with this string. For example, "log-level" with prefix "APP_"
         * becomes "APP_LOG_LEVEL".
         *
         * @param prefix The prefix to use
         * @return Reference to this for chaining
         */
        auto withEnvPrefix(std::string prefix) -> EnvironmentOption&
        {
            m_envPrefix = std::move(prefix);
            return *this;
        }

        /**
         * @brief Set a default value
         * @param value The default value
         * @return Reference to this for chaining
         */
        auto withDefaultValue(T value) -> EnvironmentOption&
        {
            m_defaultValue = std::move(value);
            mutableTraits().hasDefault = true;
            return *this;
        }

        /**
         * @brief Get the environment variable name for this option
         * @return The environment variable name
         */
        [[nodiscard]] auto getEnvVarName() const -> std::string
        {
            if (!m_envVarName.empty())
            {
                return m_envVarName;
            }

            // Auto-generate from option name
            std::string name = std::string(getLongName());
            std::string result = m_envPrefix;

            for (char c : name)
            {
                if (c == '-')
                {
                    result += '_';
                }
                else
                {
                    result += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
                }
            }

            return result;
        }

        /**
         * @brief Get the value from environment variable
         * @return The value from environment, or std::nullopt if not set
         */
        [[nodiscard]] auto getEnvValue() const -> std::optional<std::string>
        {
            std::string envName = getEnvVarName();
            const char* value = std::getenv(envName.c_str());
            if (value != nullptr)
            {
                return std::string(value);
            }
            return std::nullopt;
        }

        /**
         * @brief Get the typed value, checking environment if not in variables map
         * @param vm The variables map
         * @return The typed value from command line, environment, or default
         */
        [[nodiscard]] auto getTypedValue(const boost::program_options::variables_map& vm) const
            -> std::optional<T>
        {
            // First, check command line
            if (isSet(vm))
            {
                try
                {
                    return vm[std::string(getLongName())].template as<T>();
                }
                catch (const boost::bad_any_cast&)
                {
                    // Fall through to environment check
                }
            }

            // Second, check environment
            auto envValue = getEnvValue();
            if (envValue.has_value())
            {
                return convertFromString(envValue.value());
            }

            // Finally, return default
            return m_defaultValue;
        }

        /**
         * @brief Create an environment variable mapper for use with parse_environment
         * @param prefix The prefix to match and strip
         * @return A mapper function suitable for parse_environment
         */
        [[nodiscard]] static auto createEnvMapper(const std::string& prefix) -> EnvMapper
        {
            return [prefix](const std::string& envVar) -> std::string {
                if (envVar.find(prefix) == 0)
                {
                    std::string name = envVar.substr(prefix.length());
                    // Convert UPPER_CASE to lower-case with dashes
                    std::string result;
                    for (char c : name)
                    {
                        if (c == '_')
                        {
                            result += '-';
                        }
                        else
                        {
                            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                        }
                    }
                    return result;
                }
                return "";  // Not matched
            };
        }

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override
        {
            return OptionKind::Environment;
        }

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override
        {
            auto typed = getTypedValue(vm);
            if (typed.has_value())
            {
                return typed.value();
            }
            return Value{};
        }

    protected:

        [[nodiscard]] auto doConfigureValue() const
            -> const boost::program_options::value_semantic* override
        {
            auto* value = boost::program_options::value<T>();

            if (m_defaultValue.has_value())
            {
                value->default_value(m_defaultValue.value());
            }

            if (getTraits().isRequired)
            {
                value->required();
            }

            return value;
        }

    private:

        /**
         * @brief Convert string to target type
         * @param str String value to convert
         * @return Converted value, or std::nullopt on failure
         */
        [[nodiscard]] auto convertFromString(const std::string& str) const -> std::optional<T>
        {
            if constexpr (std::is_same_v<T, std::string>)
            {
                return str;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                try
                {
                    return std::stoi(str);
                }
                catch (...)
                {
                    return std::nullopt;
                }
            }
            else if constexpr (std::is_same_v<T, long>)
            {
                try
                {
                    return std::stol(str);
                }
                catch (...)
                {
                    return std::nullopt;
                }
            }
            else if constexpr (std::is_same_v<T, double>)
            {
                try
                {
                    return std::stod(str);
                }
                catch (...)
                {
                    return std::nullopt;
                }
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                std::string lower = str;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                return (lower == "true" || lower == "1" || lower == "yes" || lower == "on");
            }
            else
            {
                // For other types, try stream extraction
                std::istringstream iss(str);
                T value;
                if (iss >> value)
                {
                    return value;
                }
                return std::nullopt;
            }
        }

        std::string m_envVarName;     ///< Explicit environment variable name
        std::string m_envPrefix;      ///< Prefix for auto-mapping
        std::optional<T> m_defaultValue;
    };

    // ============================================
    // Common Environment Option Type Aliases
    // ============================================

    using StringEnvOption = EnvironmentOption<std::string>;
    using IntEnvOption = EnvironmentOption<int>;
    using BoolEnvOption = EnvironmentOption<bool>;

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ENVIRONMENTOPTION_HPP
