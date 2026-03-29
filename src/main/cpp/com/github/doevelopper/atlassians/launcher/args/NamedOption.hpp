/*******************************************************************
 * @file   NamedOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Named option with explicit value processing
 *
 * This class provides a concrete named option with explicit
 * processing capabilities for the parsed value.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_NAMEDOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_NAMEDOPTION_HPP

#include <functional>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Named option with value processing callback
     *
     * A concrete option implementation that stores a callback for
     * processing the option value after parsing.
     *
     * @tparam T The value type
     *
     * @example
     * ```cpp
     * NamedOption<std::string> fileOpt("file", "f", "Input file");
     * fileOpt.onProcess([](const std::string& path) {
     *     std::cout << "Processing file: " << path << std::endl;
     * });
     * ```
     */
    template<typename T>
    class NamedOption : public AbstractOption
    {
    public:

        using ValueType = T;
        using ProcessCallback = std::function<void(const T&)>;

        /**
         * @brief Construct a named option
         * @param longName Long option name
         * @param description Option description
         */
        explicit NamedOption(std::string longName, std::string description)
            : AbstractOption(std::move(longName), std::move(description))
            , m_defaultValue()
            , m_processCallback()
            , m_variablesMap(nullptr)
        {
        }

        /**
         * @brief Construct a named option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        NamedOption(std::string longName, std::string shortName, std::string description)
            : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
            , m_defaultValue()
            , m_processCallback()
            , m_variablesMap(nullptr)
        {
        }

        ~NamedOption() noexcept override = default;

        // ============================================
        // Named Option Specific Methods
        // ============================================

        /**
         * @brief Set default value
         * @param value The default value
         * @return Reference to this for chaining
         */
        auto withDefaultValue(T value) -> NamedOption&
        {
            m_defaultValue = std::move(value);
            mutableTraits().hasDefault = true;
            return *this;
        }

        /**
         * @brief Set processing callback
         * @param callback Function called during process() with the value
         * @return Reference to this for chaining
         */
        auto onProcess(ProcessCallback callback) -> NamedOption&
        {
            m_processCallback = std::move(callback);
            return *this;
        }

        /**
         * @brief Bind a variables map for value retrieval
         * @param vm The variables map to bind
         */
        auto bindVariablesMap(const boost::program_options::variables_map* vm) -> void
        {
            m_variablesMap = vm;
        }

        /**
         * @brief Get the typed value
         * @param vm The variables map
         * @return The value, or default if not set
         */
        [[nodiscard]] auto getTypedValue(const boost::program_options::variables_map& vm) const
            -> std::optional<T>
        {
            if (!isSet(vm))
            {
                return m_defaultValue;
            }

            try
            {
                return vm[std::string(getLongName())].template as<T>();
            }
            catch (const boost::bad_any_cast&)
            {
                return m_defaultValue;
            }
        }

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override
        {
            return OptionKind::Named;
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

        auto process() -> void override
        {
            if (m_processCallback && m_variablesMap != nullptr)
            {
                auto value = getTypedValue(*m_variablesMap);
                if (value.has_value())
                {
                    m_processCallback(value.value());
                }
            }
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

        std::optional<T> m_defaultValue;
        ProcessCallback m_processCallback;
        const boost::program_options::variables_map* m_variablesMap;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_NAMEDOPTION_HPP
