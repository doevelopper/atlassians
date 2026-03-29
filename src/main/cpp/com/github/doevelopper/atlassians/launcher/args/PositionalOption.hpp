/*******************************************************************
 * @file   PositionalOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Positional argument option (arguments by position, not name)
 *
 * This class represents positional arguments that are identified by
 * their position on the command line rather than by a flag.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_POSITIONALOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_POSITIONALOPTION_HPP

#include <vector>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Positional argument option
     *
     * Represents arguments identified by position rather than by name.
     * Can accept a fixed number of arguments or unlimited arguments.
     *
     * @tparam T The value type for the positional argument
     *
     * @example
     * ```cpp
     * // Single input file (first positional argument)
     * PositionalOption<std::string> inputFile("input", "Input file", 1);
     *
     * // Multiple source files (unlimited)
     * PositionalOption<std::string> sources("source", "Source files", -1);
     * ```
     */
    template<typename T>
    class PositionalOption : public AbstractOption
    {
    public:

        using ValueType = T;
        using ContainerType = std::vector<T>;

        /// @brief Unlimited positional arguments
        static constexpr int Unlimited = -1;

        /**
         * @brief Construct a positional option
         * @param name Internal name for the option
         * @param description Option description
         * @param maxCount Maximum number of positional args to consume (-1 for unlimited)
         */
        explicit PositionalOption(std::string name, std::string description, int maxCount = 1)
            : AbstractOption(std::move(name), std::move(description))
            , m_maxCount(maxCount)
            , m_position(0)
        {
            mutableTraits().isPositional = true;
            if (maxCount == Unlimited || maxCount > 1)
            {
                mutableTraits().allowsMultiple = true;
            }
        }

        ~PositionalOption() noexcept override = default;

        // ============================================
        // Positional Option Specific Methods
        // ============================================

        /**
         * @brief Set the starting position for this argument
         * @param position The starting position (0-based)
         * @return Reference to this for chaining
         */
        auto atPosition(std::size_t position) -> PositionalOption&
        {
            m_position = position;
            return *this;
        }

        /**
         * @brief Get the maximum count of arguments
         */
        [[nodiscard]] auto getMaxCount() const noexcept -> int
        {
            return m_maxCount;
        }

        /**
         * @brief Get a single value (for single-value positional args)
         * @param vm The variables map
         * @return The value, or std::nullopt if not set
         */
        [[nodiscard]] auto getSingleValue(const boost::program_options::variables_map& vm) const
            -> std::optional<T>
        {
            if (!isSet(vm))
            {
                return std::nullopt;
            }

            try
            {
                if (m_maxCount == 1)
                {
                    return vm[std::string(getLongName())].template as<T>();
                }
                else
                {
                    auto values = vm[std::string(getLongName())].template as<ContainerType>();
                    if (!values.empty())
                    {
                        return values.front();
                    }
                }
            }
            catch (const boost::bad_any_cast&)
            {
                // Ignore
            }
            return std::nullopt;
        }

        /**
         * @brief Get all values (for multi-value positional args)
         * @param vm The variables map
         * @return Vector of values
         */
        [[nodiscard]] auto getValues(const boost::program_options::variables_map& vm) const
            -> ContainerType
        {
            if (!isSet(vm))
            {
                return {};
            }

            try
            {
                if (m_maxCount == 1)
                {
                    ContainerType result;
                    result.push_back(vm[std::string(getLongName())].template as<T>());
                    return result;
                }
                else
                {
                    return vm[std::string(getLongName())].template as<ContainerType>();
                }
            }
            catch (const boost::bad_any_cast&)
            {
                return {};
            }
        }

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override
        {
            return OptionKind::Positional;
        }

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override
        {
            if (m_maxCount == 1)
            {
                auto val = getSingleValue(vm);
                if (val.has_value())
                {
                    return val.value();
                }
            }
            else
            {
                return getValues(vm);
            }
            return Value{};
        }

        auto configurePositional(
            boost::program_options::positional_options_description& positional) const -> void override
        {
            positional.add(std::string(getLongName()).c_str(), m_maxCount);
        }

        [[nodiscard]] auto getFormattedName() const -> std::string override
        {
            // Positional options only have a long name (internal identifier)
            return std::string(getLongName());
        }

    protected:

        [[nodiscard]] auto doConfigureValue() const
            -> const boost::program_options::value_semantic* override
        {
            if (m_maxCount == 1)
            {
                auto* value = boost::program_options::value<T>();
                if (getTraits().isRequired)
                {
                    value->required();
                }
                return value;
            }
            else
            {
                auto* value = boost::program_options::value<ContainerType>();
                value->multitoken();
                if (getTraits().isRequired)
                {
                    value->required();
                }
                return value;
            }
        }

    private:

        int m_maxCount;          ///< Maximum number of positional args (-1 for unlimited)
        std::size_t m_position;  ///< Starting position (for documentation/ordering)
    };

    // ============================================
    // Common Positional Option Type Aliases
    // ============================================

    using StringPositionalOption = PositionalOption<std::string>;
    using IntPositionalOption = PositionalOption<int>;

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_POSITIONALOPTION_HPP
