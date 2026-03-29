/*******************************************************************
 * @file   MultiValueOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option accepting multiple values
 *
 * This class represents an option that can be specified multiple times
 * or accepts multiple values. Useful for include paths, file lists, etc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_MULTIVALUEOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_MULTIVALUEOPTION_HPP

#include <string>
#include <vector>
#include <type_traits>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Option that accepts multiple values
     *
     * Represents a command-line option that can be specified multiple times
     * or accepts a list of values. Values are collected into a vector.
     *
     * @tparam T The element type for the value collection
     *
     * @example
     * ```cpp
     * MultiValueOption<std::string> includeOpt("include", "I", "Include directories");
     * // Usage: --include dir1 --include dir2 -I dir3
     *
     * MultiValueOption<int> portsOpt("port", "p", "Ports to listen on");
     * // Usage: --port 8080 --port 8443
     * ```
     */
    template<typename T>
    class MultiValueOption : public AbstractOption
    {
    public:

        using ValueType = T;
        using ContainerType = std::vector<T>;

        /**
         * @brief Construct a multi-value option
         * @param longName Long option name
         * @param description Option description
         */
        explicit MultiValueOption(std::string longName, std::string description)
            : AbstractOption(std::move(longName), std::move(description))
            , m_defaultValues()
            , m_minCount(0)
            , m_maxCount(std::numeric_limits<std::size_t>::max())
        {
            mutableTraits().allowsMultiple = true;
        }

        /**
         * @brief Construct a multi-value option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        MultiValueOption(std::string longName, std::string shortName, std::string description)
            : AbstractOption(std::move(longName), std::move(shortName), std::move(description))
            , m_defaultValues()
            , m_minCount(0)
            , m_maxCount(std::numeric_limits<std::size_t>::max())
        {
            mutableTraits().allowsMultiple = true;
        }

        ~MultiValueOption() noexcept override = default;

        // ============================================
        // Multi-value Specific Methods
        // ============================================

        /**
         * @brief Set default values
         * @param values Default value list
         * @return Reference to this for chaining
         */
        auto withDefaultValues(ContainerType values) -> MultiValueOption&
        {
            m_defaultValues = std::move(values);
            mutableTraits().hasDefault = true;
            return *this;
        }

        /**
         * @brief Set minimum number of values required
         * @param count Minimum count
         * @return Reference to this for chaining
         */
        auto withMinCount(std::size_t count) -> MultiValueOption&
        {
            m_minCount = count;
            return *this;
        }

        /**
         * @brief Set maximum number of values allowed
         * @param count Maximum count
         * @return Reference to this for chaining
         */
        auto withMaxCount(std::size_t count) -> MultiValueOption&
        {
            m_maxCount = count;
            return *this;
        }

        /**
         * @brief Enable composing behavior (values from multiple sources are combined)
         * @return Reference to this for chaining
         */
        auto composing() -> MultiValueOption&
        {
            mutableTraits().isComposing = true;
            return *this;
        }

        /**
         * @brief Check if composing is enabled
         */
        [[nodiscard]] auto isComposing() const noexcept -> bool
        {
            return getTraits().isComposing;
        }

        /**
         * @brief Get minimum count
         */
        [[nodiscard]] auto getMinCount() const noexcept -> std::size_t
        {
            return m_minCount;
        }

        /**
         * @brief Get maximum count
         */
        [[nodiscard]] auto getMaxCount() const noexcept -> std::size_t
        {
            return m_maxCount;
        }

        /**
         * @brief Check if default values are set
         */
        [[nodiscard]] auto hasDefaultValue() const noexcept -> bool
        {
            return !m_defaultValues.empty();
        }

        /**
         * @brief Get the values from the variables map
         * @param vm The variables map
         * @return Vector of values, or default values if not specified
         */
        [[nodiscard]] auto getValues(const boost::program_options::variables_map& vm) const
            -> ContainerType
        {
            if (!isSet(vm))
            {
                return m_defaultValues;
            }

            try
            {
                return vm[std::string(getLongName())].template as<ContainerType>();
            }
            catch (const boost::bad_any_cast&)
            {
                return m_defaultValues;
            }
        }

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override
        {
            return OptionKind::MultiValue;
        }

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override
        {
            return getValues(vm);
        }

        [[nodiscard]] auto validate(const boost::program_options::variables_map& vm) const -> bool override
        {
            if (isSet(vm))
            {
                auto values = getValues(vm);
                if (values.size() < m_minCount || values.size() > m_maxCount)
                {
                    return false;
                }
            }
            return true;
        }

    protected:

        [[nodiscard]] auto doConfigureValue() const
            -> const boost::program_options::value_semantic* override
        {
            auto* value = boost::program_options::value<ContainerType>();

            if (!m_defaultValues.empty())
            {
                // Convert default values to string representation for display
                std::string defaultText;
                for (std::size_t i = 0; i < m_defaultValues.size(); ++i)
                {
                    if (i > 0) defaultText += ", ";
                    if constexpr (std::is_same_v<T, std::string>)
                    {
                        defaultText += m_defaultValues[i];
                    }
                    else
                    {
                        defaultText += std::to_string(m_defaultValues[i]);
                    }
                }
                value->default_value(m_defaultValues, defaultText);
            }

            value->multitoken();

            if (getTraits().isComposing)
            {
                value->composing();
            }

            if (getTraits().isRequired)
            {
                value->required();
            }

            return value;
        }

    private:

        ContainerType m_defaultValues;
        std::size_t m_minCount;
        std::size_t m_maxCount;
    };

    // ============================================
    // Common Multi-value Type Aliases
    // ============================================

    using StringListOption = MultiValueOption<std::string>;
    using IntListOption = MultiValueOption<int>;

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_MULTIVALUEOPTION_HPP
