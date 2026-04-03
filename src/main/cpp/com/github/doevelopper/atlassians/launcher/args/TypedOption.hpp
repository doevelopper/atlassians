/*******************************************************************
 * @file   TypedOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Type-safe option template using CRTP pattern
 *
 * This template provides type-safe access to option values and
 * implements the Curiously Recurring Template Pattern (CRTP) for
 * static polymorphism where performance is critical.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TYPEDOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TYPEDOPTION_HPP

#include <optional>
#include <type_traits>

#include <boost/program_options.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Type-safe option template
     *
     * Provides compile-time type safety for option values. Supports
     * default values, implicit values, and type conversion.
     *
     * @tparam T The value type this option holds
     *
     * @example
     * ```cpp
     * // String option with default value
     * TypedOption<std::string> configFile("config", "c", "Configuration file path");
     * configFile.withDefaultValue("/etc/app.conf");
     *
     * // Integer option with implicit value
     * TypedOption<int> verbosity("verbose", "v", "Verbosity level");
     * verbosity.withImplicitValue(1).withDefaultValue(0);
     * ```
     */
    template<typename T>
    class TypedOption : public AbstractOption
    {
    public:

        using ValueType = T;

        /**
         * @brief Construct a typed option
         * @param longName Long option name
         * @param description Option description
         */
        explicit TypedOption(std::string longName, std::string description)
            : AbstractOption(std::move(longName), std::move(description))
            , m_defaultValue()
            , m_implicitValue()
        {
        }

        /**
         * @brief Construct a typed option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        TypedOption(std::string longName, std::string shortName, std::string description)
            : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
            , m_defaultValue()
            , m_implicitValue()
        {
        }

        ~TypedOption() noexcept override = default;

        // ============================================
        // Type-specific Methods
        // ============================================

        /**
         * @brief Set a default value for the option
         * @param value The default value
         * @return Reference to this for chaining
         */
        auto withDefaultValue(T value) -> TypedOption&
        {
            m_defaultValue = std::move(value);
            mutableTraits().hasDefault = true;
            return *this;
        }

        /**
         * @brief Set an implicit value (used when option is specified without value)
         * @param value The implicit value
         * @return Reference to this for chaining
         */
        auto withImplicitValue(T value) -> TypedOption&
        {
            m_implicitValue = std::move(value);
            mutableTraits().hasImplicit = true;
            return *this;
        }

        /**
         * @brief Get the typed value from the variables map
         * @param vm The variables map
         * @return The typed value, or std::nullopt if not set
         */
        [[nodiscard]] auto getTypedValue(const boost::program_options::variables_map& vm) const
            -> std::optional<T>
        {
            if (!isSet(vm))
            {
                if (m_defaultValue.has_value())
                {
                    return m_defaultValue;
                }
                return std::nullopt;
            }

            try
            {
                return vm[std::string(getLongName())].template as<T>();
            }
            catch (const boost::bad_any_cast&)
            {
                return std::nullopt;
            }
        }

        /**
         * @brief Get the default value if set
         */
        [[nodiscard]] auto getDefaultValue() const noexcept -> const std::optional<T>&
        {
            return m_defaultValue;
        }

        /**
         * @brief Check if a default value is set
         */
        [[nodiscard]] auto hasDefaultValue() const noexcept -> bool
        {
            return m_defaultValue.has_value();
        }

        /**
         * @brief Get the implicit value if set
         */
        [[nodiscard]] auto getImplicitValue() const noexcept -> const std::optional<T>&
        {
            return m_implicitValue;
        }

        /**
         * @brief Check if an implicit value is set
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
            return OptionKind::Named;
        }

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override
        {
            auto typed = getTypedValue(vm);
            if (typed.has_value())
            {
                return typed.value();
            }
            return Value{};  // Empty any
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

            if (m_notifier.has_value())
            {
                value->notifier([notifier = m_notifier.value()](const T& val) {
                    notifier(val);
                });
            }

            return value;
        }

    private:

        std::optional<T> m_defaultValue;
        std::optional<T> m_implicitValue;
    };

    // ============================================
    // Common Type Aliases
    // ============================================

    using StringOption = TypedOption<std::string>;
    using IntOption = TypedOption<int>;
    using DoubleOption = TypedOption<double>;
    using UnsignedOption = TypedOption<unsigned int>;
    using LongOption = TypedOption<long>;

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TYPEDOPTION_HPP
