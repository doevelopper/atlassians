/*******************************************************************
 * @file   Optional.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Optional option decorator (explicit optional marking)
 *
 * This decorator explicitly marks an option as optional with
 * optional default values.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_OPTIONAL_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_OPTIONAL_HPP

#include <memory>

#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Decorator that explicitly marks an option as optional
     *
     * Uses the Decorator pattern to wrap any IOption. While options
     * are optional by default, this provides explicit documentation
     * and can be used to add a default value to any option type.
     *
     * @example
     * ```cpp
     * // Make an option explicitly optional with a default
     * auto outputOpt = std::make_shared<StringOption>("output", "o", "Output file");
     * OptionalOption optional(outputOpt, std::string("output.txt"));
     * ```
     */
    class OptionalOption : public IOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct an optional option decorator
         * @param wrapped The option to wrap
         */
        explicit OptionalOption(IOption::Ptr wrapped);

        /**
         * @brief Construct an optional option decorator with default value
         * @param wrapped The option to wrap
         * @param defaultValue The default value as std::any
         */
        OptionalOption(IOption::Ptr wrapped, Value defaultValue);

        OptionalOption(const OptionalOption&) = delete;
        OptionalOption(OptionalOption&&) noexcept = default;
        OptionalOption& operator=(const OptionalOption&) = delete;
        OptionalOption& operator=(OptionalOption&&) noexcept = default;
        ~OptionalOption() noexcept override;

        // ============================================
        // Optional Specific Methods
        // ============================================

        /**
         * @brief Get the default value
         */
        [[nodiscard]] auto getDefaultValue() const noexcept -> const Value&;

        /**
         * @brief Check if a default value is set
         */
        [[nodiscard]] auto hasDefaultValue() const noexcept -> bool;

        // ============================================
        // IOption Implementation (Delegation)
        // ============================================

        [[nodiscard]] auto getLongName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getShortName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getDescription() const noexcept -> std::string_view override;
        [[nodiscard]] auto getKind() const noexcept -> OptionKind override;
        [[nodiscard]] auto getTraits() const noexcept -> const OptionTraits& override;

        auto configure(boost::program_options::options_description& desc) const -> void override;
        auto configurePositional(boost::program_options::positional_options_description& positional) const
            -> void override;

        [[nodiscard]] auto isSet(const boost::program_options::variables_map& vm) const noexcept -> bool override;
        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override;
        [[nodiscard]] auto validate(const boost::program_options::variables_map& vm) const -> bool override;

        auto process() -> void override;

        /**
         * @brief Get the wrapped option
         */
        [[nodiscard]] auto getWrapped() const noexcept -> const IOption::Ptr&;

    private:

        IOption::Ptr m_wrapped;
        Value m_defaultValue;
        mutable OptionTraits m_traits;
    };

    /**
     * @brief Factory function to create an optional option
     * @tparam OptionT The option type to create
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to the option constructor
     * @return Shared pointer to the optional option decorator
     */
    template<typename OptionT, typename... Args>
    auto makeOptional(Args&&... args) -> std::shared_ptr<OptionalOption>
    {
        auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
        return std::make_shared<OptionalOption>(std::move(option));
    }

    /**
     * @brief Factory function to create an optional option with default value
     * @tparam OptionT The option type to create
     * @tparam T The default value type
     * @tparam Args Constructor argument types
     * @param defaultValue The default value
     * @param args Arguments forwarded to the option constructor
     * @return Shared pointer to the optional option decorator
     */
    template<typename OptionT, typename T, typename... Args>
    auto makeOptionalWithDefault(T defaultValue, Args&&... args) -> std::shared_ptr<OptionalOption>
    {
        auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
        return std::make_shared<OptionalOption>(std::move(option), IOption::Value{std::move(defaultValue)});
    }

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_OPTIONAL_HPP
