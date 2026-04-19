/**
 * @file SimpleCalculatorPlugin.hpp
 * @brief Example simple calculator plugin
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_EXAMPLES_SIMPLECALCULATORPLUGIN_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_EXAMPLES_SIMPLECALCULATORPLUGIN_HPP

#include <com/github/doevelopper/atlassians/plugin/core/BasePlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExport.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::plugin::examples
{
    /**
     * @brief Simple calculator interface.
     */
    class ICalculator
    {
    public:
        virtual ~ICalculator() = default;

        [[nodiscard]] virtual auto add(double a, double b) const noexcept -> double = 0;
        [[nodiscard]] virtual auto subtract(double a, double b) const noexcept -> double = 0;
        [[nodiscard]] virtual auto multiply(double a, double b) const noexcept -> double = 0;
        [[nodiscard]] virtual auto divide(double a, double b) const -> double = 0;
    };

    /**
     * @brief Example plugin demonstrating basic plugin implementation.
     *
     * Provides simple calculator operations and demonstrates:
     * - BasePlugin inheritance
     * - Service registration
     * - Event publishing
     * - Configuration usage
     */
    class SimpleCalculatorPlugin : public BasePlugin, public ICalculator
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        // ============================================
        // Plugin Metadata Constants
        // ============================================

        static constexpr const char* PLUGIN_NAME = "SimpleCalculator";
        static constexpr const char* PLUGIN_DESCRIPTION = "A simple calculator plugin for basic arithmetic operations";
        static constexpr const char* PLUGIN_AUTHOR = "ACME Technology Corp.";
        static constexpr PluginVersion PLUGIN_VERSION = {1, 0, 0};

        // ============================================
        // Constructors and Destructor
        // ============================================

        /**
         * @brief Default constructor.
         */
        SimpleCalculatorPlugin();

        /**
         * @brief Destructor.
         */
        ~SimpleCalculatorPlugin() noexcept override;

        // ============================================
        // IPlugin Implementation
        // ============================================

        [[nodiscard]] auto getName() const noexcept -> std::string_view override;
        [[nodiscard]] auto getVersion() const noexcept -> PluginVersion override;
        [[nodiscard]] auto getDescription() const noexcept -> std::string_view override;

        // ============================================
        // ICalculator Implementation
        // ============================================

        [[nodiscard]] auto add(double a, double b) const noexcept -> double override;
        [[nodiscard]] auto subtract(double a, double b) const noexcept -> double override;
        [[nodiscard]] auto multiply(double a, double b) const noexcept -> double override;
        [[nodiscard]] auto divide(double a, double b) const -> double override;

    protected:
        // ============================================
        // BasePlugin Template Method Overrides
        // ============================================

        auto doInitialize(IPluginContext* context) -> bool override;
        auto doStart() -> bool override;
        auto doStop() -> bool override;
        auto doShutdown() -> void override;

    private:
        std::size_t operationCount_{0};
        bool logOperations_{false};
    };

} // namespace com::github::doevelopper::atlassians::plugin::examples

// ============================================
// Plugin Export Declaration
// ============================================

extern "C"
{
    PLUGIN_EXPORT auto createPlugin()
        -> com::github::doevelopper::atlassians::plugin::IPlugin*;

    PLUGIN_EXPORT auto destroyPlugin(com::github::doevelopper::atlassians::plugin::IPlugin* plugin)
        -> void;

    PLUGIN_EXPORT auto getPluginApiVersion() -> int;
}

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_EXAMPLES_SIMPLECALCULATORPLUGIN_HPP
