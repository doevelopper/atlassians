/*******************************************************************
 * @file   IOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Base interface for command-line options using Strategy pattern
 *
 * This interface provides the contract for all option types in the
 * command-line parsing framework. It follows the Interface Segregation
 * Principle (ISP) and supports the Strategy pattern for different
 * option behaviors.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IOPTION_HPP

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Strongly typed enum for option types
     */
    enum class OptionKind : std::uint8_t
    {
        Named,           ///< Standard named option (--option=value)
        Boolean,         ///< Boolean switch (--flag)
        Positional,      ///< Positional argument
        MultiValue,      ///< Option accepting multiple values
        Implicit,        ///< Option with implicit value when specified without value
        Hidden,          ///< Hidden from help display
        ConfigFile,      ///< Configuration file option
        Environment      ///< Environment variable option
    };

    /**
     * @brief Option traits for composable behavior (Decorator pattern support)
     */
    struct OptionTraits
    {
        bool isRequired = false;      ///< Option must be specified
        bool isHidden = false;        ///< Not shown in help
        bool isPositional = false;    ///< Positional argument
        bool allowsMultiple = false;  ///< Can be specified multiple times
        bool hasImplicit = false;     ///< Has implicit value
        bool hasDefault = false;      ///< Has default value
        bool isComposing = false;     ///< Values are composed from multiple sources
    };

    /**
     * @brief Interface for all command-line options
     *
     * Implements the Strategy pattern - each option type provides its own
     * implementation of how to configure and validate itself.
     *
     * @note Follows Interface Segregation Principle (ISP)
     */
    class IOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /// @brief Type alias for option pointer
        using Ptr = std::shared_ptr<IOption>;

        /// @brief Type alias for option value
        using Value = std::any;

        /// @brief Type alias for validation callback
        using Validator = std::function<bool(const Value&)>;

        /// @brief Type alias for notification callback
        using Notifier = std::function<void(const Value&)>;

        IOption() noexcept;
        IOption(const IOption&) = delete;
        IOption(IOption&&) = delete;
        IOption& operator=(const IOption&) = delete;
        IOption& operator=(IOption&&) = delete;
        virtual ~IOption() noexcept;

        // ============================================
        // Core Interface Methods
        // ============================================

        /**
         * @brief Get the long name of the option (e.g., "help")
         */
        [[nodiscard]] virtual auto getLongName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the short name of the option (e.g., "h")
         */
        [[nodiscard]] virtual auto getShortName() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the description for help text
         */
        [[nodiscard]] virtual auto getDescription() const noexcept -> std::string_view = 0;

        /**
         * @brief Get the option kind
         */
        [[nodiscard]] virtual auto getKind() const noexcept -> OptionKind = 0;

        /**
         * @brief Get the option traits
         */
        [[nodiscard]] virtual auto getTraits() const noexcept -> const OptionTraits& = 0;

        /**
         * @brief Configure this option into a boost options_description
         * @param desc The options description to configure
         */
        virtual auto configure(boost::program_options::options_description& desc) const -> void = 0;

        /**
         * @brief Configure positional options if applicable
         * @param positional The positional options description
         */
        virtual auto configurePositional(boost::program_options::positional_options_description& positional) const -> void;

        /**
         * @brief Check if the option was specified in the parsed arguments
         * @param vm The variables map from parsing
         */
        [[nodiscard]] virtual auto isSet(const boost::program_options::variables_map& vm) const noexcept -> bool;

        /**
         * @brief Get the value of this option from the variables map
         * @param vm The variables map from parsing
         * @return The value wrapped in std::any, or empty if not set
         */
        [[nodiscard]] virtual auto getValue(const boost::program_options::variables_map& vm) const -> Value = 0;

        /**
         * @brief Validate the option value
         * @param vm The variables map from parsing
         * @return true if valid, false otherwise
         */
        [[nodiscard]] virtual auto validate(const boost::program_options::variables_map& vm) const -> bool;

        /**
         * @brief Get the formatted option string for help (e.g., "-h,--help")
         */
        [[nodiscard]] virtual auto getFormattedName() const -> std::string;

        /**
         * @brief Process the option after parsing (Template Method pattern hook)
         */
        virtual auto process() -> void = 0;

    protected:

    private:
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_IOPTION_HPP
