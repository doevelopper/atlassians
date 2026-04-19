/*******************************************************************
 * @file   Grouped.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Option grouping for organized help display
 *
 * This class provides functionality for grouping related options
 * together in the help display.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_GROUPED_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_GROUPED_HPP

#include <memory>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Container for grouping related options
     *
     * Groups options together for organized help display.
     * Each group has a name that appears as a section header in help.
     *
     * @example
     * ```cpp
     * OptionGroup generalGroup("General Options");
     * generalGroup.add<BooleanOption>("help", "h", "Show help");
     * generalGroup.add<BooleanOption>("version", "V", "Show version");
     *
     * OptionGroup ioGroup("I/O Options");
     * ioGroup.add<StringOption>("input", "i", "Input file");
     * ioGroup.add<StringOption>("output", "o", "Output file");
     * ```
     */
    class OptionGroup
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using OptionList = std::vector<IOption::Ptr>;

        /**
         * @brief Construct an option group
         * @param name The group name (displayed as section header)
         * @param description Optional group description
         */
        explicit OptionGroup(std::string name, std::string description = "");

        OptionGroup(const OptionGroup&) = delete;
        OptionGroup(OptionGroup&&) noexcept = default;
        OptionGroup& operator=(const OptionGroup&) = delete;
        OptionGroup& operator=(OptionGroup&&) noexcept = default;
        ~OptionGroup() noexcept;

        // ============================================
        // Group Management
        // ============================================

        /**
         * @brief Get the group name
         */
        [[nodiscard]] auto getName() const noexcept -> std::string_view;

        /**
         * @brief Get the group description
         */
        [[nodiscard]] auto getDescription() const noexcept -> std::string_view;

        /**
         * @brief Add an existing option to the group
         * @param option The option to add
         * @return Reference to this for chaining
         */
        auto addOption(IOption::Ptr option) -> OptionGroup&;

        /**
         * @brief Create and add an option to the group
         * @tparam OptionT The option type to create
         * @tparam Args Constructor argument types
         * @param args Arguments forwarded to the option constructor
         * @return Reference to the created option
         */
        template<typename OptionT, typename... Args>
        auto add(Args&&... args) -> OptionT&
        {
            auto option = std::make_shared<OptionT>(std::forward<Args>(args)...);
            auto& ref = *option;
            m_options.push_back(std::move(option));
            return ref;
        }

        /**
         * @brief Get all options in this group
         */
        [[nodiscard]] auto getOptions() const noexcept -> const OptionList&;

        /**
         * @brief Get the number of options in this group
         */
        [[nodiscard]] auto size() const noexcept -> std::size_t;

        /**
         * @brief Check if the group is empty
         */
        [[nodiscard]] auto empty() const noexcept -> bool;

        /**
         * @brief Configure all options into an options_description
         * @param desc The options description to configure
         */
        auto configure(boost::program_options::options_description& desc) const -> void;

        /**
         * @brief Configure positional options
         * @param positional The positional options description
         */
        auto configurePositional(boost::program_options::positional_options_description& positional) const
            -> void;

        /**
         * @brief Create a separate options_description for this group
         * @return An options_description with just this group's options
         */
        [[nodiscard]] auto createOptionsDescription() const -> boost::program_options::options_description;

        // ============================================
        // Iteration Support
        // ============================================

        auto begin() noexcept { return m_options.begin(); }
        auto end() noexcept { return m_options.end(); }
        [[nodiscard]] auto begin() const noexcept { return m_options.cbegin(); }
        [[nodiscard]] auto end() const noexcept { return m_options.cend(); }
        [[nodiscard]] auto cbegin() const noexcept { return m_options.cbegin(); }
        [[nodiscard]] auto cend() const noexcept { return m_options.cend(); }

    private:

        std::string m_name;
        std::string m_description;
        OptionList m_options;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_GROUPED_HPP
