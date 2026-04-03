/**
 * @file SimpleCalculatorPlugin.cpp
 * @brief Example simple calculator plugin implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <stdexcept>

#include <com/github/doevelopper/atlassians/plugin/examples/SimpleCalculatorPlugin.hpp>
#include <com/github/doevelopper/atlassians/plugin/core/PluginExceptions.hpp>

namespace com::github::doevelopper::atlassians::plugin::examples
{
    log4cxx::LoggerPtr SimpleCalculatorPlugin::logger =
        log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.plugin.examples.SimpleCalculatorPlugin"));

    // ============================================
    // Constructors and Destructor
    // ============================================

    SimpleCalculatorPlugin::SimpleCalculatorPlugin()
    {
        LOG4CXX_TRACE(logger, "SimpleCalculatorPlugin created");
    }

    SimpleCalculatorPlugin::~SimpleCalculatorPlugin() noexcept
    {
        LOG4CXX_TRACE(logger, "SimpleCalculatorPlugin destroyed, total operations: "
                     << operationCount_);
    }

    // ============================================
    // IPlugin Implementation
    // ============================================

    auto SimpleCalculatorPlugin::getName() const noexcept -> std::string_view
    {
        return PLUGIN_NAME;
    }

    auto SimpleCalculatorPlugin::getVersion() const noexcept -> PluginVersion
    {
        return PLUGIN_VERSION;
    }

    auto SimpleCalculatorPlugin::getDescription() const noexcept -> std::string_view
    {
        return PLUGIN_DESCRIPTION;
    }

    // ============================================
    // ICalculator Implementation
    // ============================================

    auto SimpleCalculatorPlugin::add(double a, double b) const noexcept -> double
    {
        double result = a + b;

        if (logOperations_)
        {
            LOG4CXX_DEBUG(logger, "add(" << a << ", " << b << ") = " << result);
        }

        ++const_cast<SimpleCalculatorPlugin*>(this)->operationCount_;
        return result;
    }

    auto SimpleCalculatorPlugin::subtract(double a, double b) const noexcept -> double
    {
        double result = a - b;

        if (logOperations_)
        {
            LOG4CXX_DEBUG(logger, "subtract(" << a << ", " << b << ") = " << result);
        }

        ++const_cast<SimpleCalculatorPlugin*>(this)->operationCount_;
        return result;
    }

    auto SimpleCalculatorPlugin::multiply(double a, double b) const noexcept -> double
    {
        double result = a * b;

        if (logOperations_)
        {
            LOG4CXX_DEBUG(logger, "multiply(" << a << ", " << b << ") = " << result);
        }

        ++const_cast<SimpleCalculatorPlugin*>(this)->operationCount_;
        return result;
    }

    auto SimpleCalculatorPlugin::divide(double a, double b) const -> double
    {
        if (b == 0.0)
        {
            LOG4CXX_ERROR(logger, "Division by zero attempted");
            throw std::invalid_argument("Division by zero");
        }

        double result = a / b;

        if (logOperations_)
        {
            LOG4CXX_DEBUG(logger, "divide(" << a << ", " << b << ") = " << result);
        }

        ++const_cast<SimpleCalculatorPlugin*>(this)->operationCount_;
        return result;
    }

    // ============================================
    // BasePlugin Template Method Overrides
    // ============================================

    auto SimpleCalculatorPlugin::doInitialize(IPluginContext* context) -> bool
    {
        LOG4CXX_DEBUG(logger, "Initializing SimpleCalculatorPlugin");

        if (context)
        {
            // Read configuration
            logOperations_ = context->getConfigurationBool("log_operations", false);

            LOG4CXX_INFO(logger, "Configuration: log_operations = "
                        << (logOperations_ ? "true" : "false"));

            // Register ourselves as a calculator service
            context->registerService("ICalculator",
                std::any(static_cast<ICalculator*>(this)));

            LOG4CXX_DEBUG(logger, "Registered ICalculator service");
        }

        operationCount_ = 0;
        return true;
    }

    auto SimpleCalculatorPlugin::doStart() -> bool
    {
        LOG4CXX_INFO(logger, "SimpleCalculatorPlugin started");

        // Publish a startup event
        if (auto* ctx = getContext())
        {
            PluginEvent event;
            event.source = std::string(PLUGIN_NAME);
            event.type = "plugin.started";
            event.data = std::string("SimpleCalculator ready for operations");

            ctx->publishEvent("calculator.status", event);
        }

        return true;
    }

    auto SimpleCalculatorPlugin::doStop() -> bool
    {
        LOG4CXX_INFO(logger, "SimpleCalculatorPlugin stopping, performed "
                    << operationCount_ << " operations");

        // Publish a shutdown event
        if (auto* ctx = getContext())
        {
            PluginEvent event;
            event.source = std::string(PLUGIN_NAME);
            event.type = "plugin.stopped";
            event.data = operationCount_;

            ctx->publishEvent("calculator.status", event);
        }

        return true;
    }

    auto SimpleCalculatorPlugin::doShutdown() -> void
    {
        LOG4CXX_DEBUG(logger, "Shutting down SimpleCalculatorPlugin");

        if (auto* ctx = getContext())
        {
            ctx->unregisterService("ICalculator");
        }
    }

} // namespace com::github::doevelopper::atlassians::plugin::examples

// ============================================
// Plugin Export Implementation
// ============================================

extern "C"
{
    PLUGIN_EXPORT auto createPlugin()
        -> com::github::doevelopper::atlassians::plugin::IPlugin*
    {
        return new com::github::doevelopper::atlassians::plugin::examples::SimpleCalculatorPlugin();
    }

    PLUGIN_EXPORT auto destroyPlugin(com::github::doevelopper::atlassians::plugin::IPlugin* plugin)
        -> void
    {
        delete plugin;
    }

    PLUGIN_EXPORT auto getPluginApiVersion() -> int
    {
        return PLUGIN_API_VERSION;
    }
}
