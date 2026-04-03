/*******************************************************************
 * @file   BooleanOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Boolean switch option (flag without value)
 *
 * This class represents a boolean flag option that acts as a switch.
 * When specified on the command line, it sets the value to true.
 * Supports negatable options (--no-option) for explicit false.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_BOOLEANOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_BOOLEANOPTION_HPP

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Boolean flag option (switch)
     *
     * Represents a command-line switch that sets a boolean value.
     * When specified, the value is true; when absent, the value is false.
     *
     * @example
     * ```cpp
     * BooleanOption verboseOpt("verbose", "v", "Enable verbose output");
     * // --verbose or -v sets to true
     *
     * BooleanOption debugOpt("debug", "d", "Enable debug mode");
     * debugOpt.negatable();  // Allows --no-debug
     * ```
     */
    class BooleanOption : public AbstractOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct a boolean option
         * @param longName Long option name
         * @param description Option description
         */
        explicit BooleanOption(std::string longName, std::string description);

        /**
         * @brief Construct a boolean option with short name
         * @param longName Long option name
         * @param shortName Short option name
         * @param description Option description
         */
        BooleanOption(std::string longName, std::string shortName, std::string description);

        BooleanOption(const BooleanOption&) = delete;
        BooleanOption(BooleanOption&&) noexcept = default;
        BooleanOption& operator=(const BooleanOption&) = delete;
        BooleanOption& operator=(BooleanOption&&) noexcept = default;
        ~BooleanOption() noexcept override;

        // ============================================
        // Boolean-specific Methods
        // ============================================

        /**
         * @brief Make this option negatable (allows --no-option)
         * @return Reference to this for chaining
         */
        auto negatable() -> BooleanOption&;

        /**
         * @brief Check if the option is negatable
         */
        [[nodiscard]] auto isNegatable() const noexcept -> bool;

        /**
         * @brief Get the boolean value from the variables map
         * @param vm The variables map
         * @return true if the option was specified, false otherwise
         */
        [[nodiscard]] auto getBoolValue(const boost::program_options::variables_map& vm) const -> bool;

        // ============================================
        // IOption Implementation
        // ============================================

        [[nodiscard]] auto getKind() const noexcept -> OptionKind override;

        [[nodiscard]] auto getValue(const boost::program_options::variables_map& vm) const -> Value override;

        auto configure(boost::program_options::options_description& desc) const -> void override;

    protected:

        [[nodiscard]] auto doConfigureValue() const
            -> const boost::program_options::value_semantic* override;

    private:

        bool m_negatable = false;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_BOOLEANOPTION_HPP
