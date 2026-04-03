/*******************************************************************
 * @file   AbstractOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Abstract base class for command-line options using Template Method pattern
 *
 * This class provides common functionality for all option types and
 * implements the Template Method pattern for option configuration.
 * It follows the Open/Closed Principle (OCP) - open for extension,
 * closed for modification.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTOPTION_HPP

#include <any>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Abstract base class for all option implementations
     *
     * Provides common storage and behavior for options while allowing
     * subclasses to customize specific behaviors through virtual methods.
     *
     * Uses Template Method pattern for configure() and Builder pattern
     * support through fluent setters.
     *
     * @note Subclasses should override:
     *   - getKind() to return their specific OptionKind
     *   - getValue() to extract typed values from variables_map
     *   - doConfigureValue() to set up boost value semantics
     */
    class AbstractOption : public IOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct an option with required parameters
         * @param longName The long option name (without --)
         * @param description Help text description
         */
        explicit AbstractOption(std::string longName, std::string description) noexcept;

        /**
         * @brief Construct an option with short and long names
         * @param longName The long option name (without --)
         * @param shortName The short option name (without -)
         * @param description Help text description
         */
        AbstractOption(std::string longName, std::string shortName, std::string description) noexcept;

        AbstractOption(const AbstractOption&) = delete;
        AbstractOption(AbstractOption&&) noexcept = default;
        AbstractOption& operator=(const AbstractOption&) = delete;
        AbstractOption& operator=(AbstractOption&&) noexcept = default;
        ~AbstractOption() noexcept override;

        // ============================================
        // IOption Interface Implementation
        // ============================================

        [[nodiscard]] auto getLongName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getShortName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getDescription() const noexcept -> std::string_view override;
        [[nodiscard]] auto getTraits() const noexcept -> const OptionTraits& override;

        /**
         * @brief Template Method for configuring the option
         *
         * Calls the following hooks in order:
         * 1. getFormattedName() - get option string
         * 2. doConfigureValue() - subclass configures value semantics
         * 3. Adds to options_description
         */
        auto configure(boost::program_options::options_description& desc) const -> void override;

        auto process() -> void override;

        // ============================================
        // Fluent Builder Pattern Methods
        // ============================================

        /**
         * @brief Mark this option as required
         * @return Reference to this for chaining
         */
        auto required() -> AbstractOption&;

        /**
         * @brief Mark this option as hidden from help
         * @return Reference to this for chaining
         */
        auto hidden() -> AbstractOption&;

        /**
         * @brief Set a validator function
         * @param validator The validation function
         * @return Reference to this for chaining
         */
        auto withValidator(Validator validator) -> AbstractOption&;

        /**
         * @brief Set a notifier function called when option is parsed
         * @param notifier The notification function
         * @return Reference to this for chaining
         */
        auto withNotifier(Notifier notifier) -> AbstractOption&;

        // ============================================
        // Additional Accessors
        // ============================================

        /**
         * @brief Get the validator function
         */
        [[nodiscard]] auto getValidator() const noexcept -> const std::optional<Validator>&;

        /**
         * @brief Get the notifier function
         */
        [[nodiscard]] auto getNotifier() const noexcept -> const std::optional<Notifier>&;

    protected:

        /**
         * @brief Hook for subclasses to configure value semantics
         *
         * Override this to specify the boost::program_options value type,
         * default value, implicit value, etc.
         *
         * @return The value semantic to use, or nullptr for switch/flag options
         */
        [[nodiscard]] virtual auto doConfigureValue() const
            -> const boost::program_options::value_semantic*;

        /**
         * @brief Mutable access to traits for subclass modification
         */
        auto mutableTraits() -> OptionTraits&;

        /// @brief The long option name
        std::string m_longName;

        /// @brief The short option name (may be empty)
        std::string m_shortName;

        /// @brief The description for help text
        std::string m_description;

        /// @brief Option traits/properties
        OptionTraits m_traits;

        /// @brief Optional validator function
        std::optional<Validator> m_validator;

        /// @brief Optional notifier function
        std::optional<Notifier> m_notifier;

    private:
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_ABSTRACTOPTION_HPP
