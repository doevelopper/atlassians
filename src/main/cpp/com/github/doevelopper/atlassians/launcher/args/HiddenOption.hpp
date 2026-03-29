/*******************************************************************
 * @file   HiddenOption.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option hidden from help display (Decorator pattern)
 *
 * This class decorates another option to make it hidden from the
 * help display while still being usable on the command line.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_HIDDENOPTION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_HIDDENOPTION_HPP

#include <memory>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Decorator to make any option hidden from help
     *
     * Uses the Decorator pattern to wrap any IOption and make it
     * hidden from the help display. The wrapped option retains all
     * its functionality.
     *
     * @example
     * ```cpp
     * // Create a hidden debug option
     * auto debugOpt = std::make_shared<BooleanOption>("debug", "d", "Debug mode");
     * HiddenOption hiddenDebug(debugOpt);
     *
     * // Or using the factory function
     * auto hidden = makeHidden<BooleanOption>("internal-flag", "Internal use only");
     * ```
     */
    class HiddenOption : public IOption
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Construct a hidden option decorator
         * @param wrapped The option to make hidden
         */
        explicit HiddenOption(IOption::Ptr wrapped);

        HiddenOption(const HiddenOption&) = delete;
        HiddenOption(HiddenOption&&) noexcept = default;
        HiddenOption& operator=(const HiddenOption&) = delete;
        HiddenOption& operator=(HiddenOption&&) noexcept = default;
        ~HiddenOption() noexcept override;

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
        mutable OptionTraits m_traits;  ///< Cached traits with hidden flag
    };

    /**
     * @brief Factory function to create a hidden option
     * @tparam OptionT The option type to create and hide
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to the option constructor
     * @return Shared pointer to the hidden option
     */
    template<typename OptionT, typename... Args>
    auto makeHidden(Args&&... args) -> std::shared_ptr<HiddenOption>
    {
        auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
        return std::make_shared<HiddenOption>(std::move(option));
    }

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_HIDDENOPTION_HPP
