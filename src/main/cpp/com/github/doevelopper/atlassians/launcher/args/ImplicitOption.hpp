/*******************************************************************
 * @file   ImplicitOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option with implicit value when specified without argument
 *
 * This class represents an option that has a default value when not
 * specified, and an implicit value when specified without an argument.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IMPLICITOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IMPLICITOPTION_HPP

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Option with implicit value support
     *
     * This option type supports three states:
     * 1. Not specified: uses default value
     * 2. Specified without value: uses implicit value
     * 3. Specified with value: uses the provided value
     *
     * @tparam T The value type
     *
     * @example
     * ```cpp
     * // Verbosity: 0 if not specified, 1 if -v, N if -v=N
     * ImplicitOption<int> verbosity("verbose", "v", "Verbosity level");
     * verbosity.withDefaultValue(0).withImplicitValue(1);
     *
     * // Config: empty if not specified, "default.cfg" if --config, path if --config=path
     * ImplicitOption<std::string> config("config", "c", "Configuration file");
     * config.withImplicitValue("default.cfg");
     * ```
     */
    template<typename T>
    class ImplicitOption : public AbstractOption
    {
    public:

        using ValueType = T;

        /**
         * @brief Construct an implicit option
         * @param longName Long option name
         * @param description Option description
         */
        explicit ImplicitOption(std::string longName, std::string description)
            : AbstractOption(std::move(longName), std::move(description))
            , m_defaultValue()
            , m_implicitValue()
        {
            mutableTraits().hasImplicit = true;
        }

        /**
         * @brief Construct an implicit option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        ImplicitOption(std::string longName, std::string shortName, std::string description)
            : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
            , m_defaultValue()
            , m_implicitValue()
        {
            mutableTraits().hasImplicit = true;
        }

        ~ImplicitOption() noexcept override = default;

        // ============================================
        // Implicit Option Specific Methods
        // ============================================

        /**
         * @brief Set the default value (used when option is not specified)
         * @param value The default value
         * @return Reference to this for chaining
         */
        auto withDefaultValue(T value) -> ImplicitOption&
        {
            m_defaultValue = std::move(value);
            mutableTraits().hasDefault = true;
            return *this;
        }

        /**
         * @brief Set the implicit value (used when option is specified without argument)
         * @param value The implicit value
         * @return Reference to this for chaining
         */
        auto withImplicitValue(T value) -> ImplicitOption&
        {
            m_implicitValue = std::move(value);
            return *this;
        }

        /**
         * @brief Get the typed value from the variables map
         * @param vm The variables map
         * @return The value (explicit, implicit, or default)
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
                return m_implicitValue;
            }
        }

        /**
         * @brief Check if default value is set
         */
        [[nodiscard]] auto hasDefaultValue() const noexcept -> bool
        {
            return m_defaultValue.has_value();
        }

        /**
         * @brief Check if implicit value is set
         */
        [[nodiscard]] auto hasImplicitValue() const noexcept -> bool
        {
            return m_implicitValue.has_value();
        }

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override
        {
            return OptionKind::Implicit;
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

            if (m_implicitValue.has_value())
            {
                value->implicit_value(m_implicitValue.value());
            }

            if (getTraits().isRequired)
            {
                value->required();
            }

            return value;
        }

    private:

        std::optional<T> m_defaultValue;
        std::optional<T> m_implicitValue;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IMPLICITOPTION_HPP
