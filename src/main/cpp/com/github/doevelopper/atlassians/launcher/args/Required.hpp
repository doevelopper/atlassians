/*******************************************************************
 * @file   Required.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Required option decorator
 *
 * This decorator marks an option as required and provides validation.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_REQUIRED_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_REQUIRED_HPP

#include <memory>
#include <stdexcept>

#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Exception thrown when a required option is missing
     */
    class RequiredOptionMissingException : public std::runtime_error
    {
    public:
        explicit RequiredOptionMissingException(const std::string& optionName)
            : std::runtime_error("Required option missing: " + optionName)
            , m_optionName(optionName)
        {
        }

        [[nodiscard]] auto getOptionName() const noexcept -> const std::string&
        {
            return m_optionName;
        }

    private:
        std::string m_optionName;
    };

    /**
     * @brief Decorator that marks an option as required
     *
     * Uses the Decorator pattern to wrap any IOption and enforce
     * that it must be specified on the command line.
     *
     * @example
     * ```cpp
     * // Make an option required
     * auto inputOpt = std::make_shared<StringOption>("input", "i", "Input file");
     * RequiredOption required(inputOpt);
     *
     * // Or using the factory function
     * auto requiredInput = makeRequired<StringOption>("input", "i", "Input file");
     * ```
     */
    class RequiredOption : public IOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct a required option decorator
         * @param wrapped The option to make required
         */
        explicit RequiredOption(IOption::Ptr wrapped);

        RequiredOption(const RequiredOption&) = delete;
        RequiredOption(RequiredOption&&) noexcept = default;
        RequiredOption& operator=(const RequiredOption&) = delete;
        RequiredOption& operator=(RequiredOption&&) noexcept = default;
        ~RequiredOption() noexcept override;

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

        /**
         * @brief Validate that the required option is set
         * @param vm The variables map
         * @return true if option is set, false otherwise
         */
        [[nodiscard]] auto validate(const boost::program_options::variables_map& vm) const -> bool override;

        auto process() -> void override;

        /**
         * @brief Check if required option is present, throw if not
         * @param vm The variables map
         * @throws RequiredOptionMissingException if option is not set
         */
        auto checkRequired(const boost::program_options::variables_map& vm) const -> void;

        /**
         * @brief Get the wrapped option
         */
        [[nodiscard]] auto getWrapped() const noexcept -> const IOption::Ptr&;

    private:

        IOption::Ptr m_wrapped;
        mutable OptionTraits m_traits;
    };

    /**
     * @brief Factory function to create a required option
     * @tparam OptionT The option type to create and make required
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to the option constructor
     * @return Shared pointer to the required option decorator
     */
    template<typename OptionT, typename... Args>
    auto makeRequired(Args&&... args) -> std::shared_ptr<RequiredOption>
    {
        auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
        return std::make_shared<RequiredOption>(std::move(option));
    }

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_REQUIRED_HPP
