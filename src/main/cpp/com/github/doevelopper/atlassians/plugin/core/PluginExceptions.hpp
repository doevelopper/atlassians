/**
 * @file PluginExceptions.hpp
 * @brief Exception hierarchy for the Plugin Architecture Framework
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXCEPTIONS_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXCEPTIONS_HPP

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace com::github::doevelopper::atlassians::plugin
{
    /**
     * @brief Base exception class for all plugin-related errors.
     */
    class PluginException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;

        /**
         * @brief Construct with message.
         */
        explicit PluginException(const std::string& message)
            : std::runtime_error(message)
            , pluginName_{}
        {
        }

        /**
         * @brief Construct with message and plugin name.
         */
        PluginException(const std::string& message, std::string pluginName)
            : std::runtime_error(message)
            , pluginName_(std::move(pluginName))
        {
        }

        /**
         * @brief Get the name of the plugin that caused the exception.
         */
        [[nodiscard]] auto getPluginName() const noexcept -> const std::string&
        {
            return pluginName_;
        }

    protected:
        std::string pluginName_;
    };

    /**
     * @brief Exception thrown when plugin loading fails.
     */
    class PluginLoadException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin path.
         */
        explicit PluginLoadException(const std::filesystem::path& path)
            : PluginException("Failed to load plugin: " + path.string())
            , pluginPath_(path)
        {
        }

        /**
         * @brief Construct with plugin path and reason.
         */
        PluginLoadException(const std::filesystem::path& path, const std::string& reason)
            : PluginException("Failed to load plugin '" + path.string() + "': " + reason)
            , pluginPath_(path)
        {
        }

        /**
         * @brief Get the path of the plugin that failed to load.
         */
        [[nodiscard]] auto getPluginPath() const noexcept -> const std::filesystem::path&
        {
            return pluginPath_;
        }

    private:
        std::filesystem::path pluginPath_;
    };

    /**
     * @brief Exception thrown when plugin unloading fails.
     */
    class PluginUnloadException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin name.
         */
        explicit PluginUnloadException(const std::string& pluginName)
            : PluginException("Failed to unload plugin: " + pluginName, pluginName)
        {
        }

        /**
         * @brief Construct with plugin name and reason.
         */
        PluginUnloadException(const std::string& pluginName, const std::string& reason)
            : PluginException("Failed to unload plugin '" + pluginName + "': " + reason, pluginName)
        {
        }
    };

    /**
     * @brief Exception thrown when plugin initialization fails.
     */
    class PluginInitializationException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin name.
         */
        explicit PluginInitializationException(const std::string& pluginName)
            : PluginException("Failed to initialize plugin: " + pluginName, pluginName)
        {
        }

        /**
         * @brief Construct with plugin name and reason.
         */
        PluginInitializationException(const std::string& pluginName, const std::string& reason)
            : PluginException("Failed to initialize plugin '" + pluginName + "': " + reason, pluginName)
        {
        }
    };

    /**
     * @brief Exception thrown when dependency resolution fails.
     */
    class PluginDependencyException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin name and missing dependency.
         */
        PluginDependencyException(const std::string& pluginName, const std::string& missingDependency)
            : PluginException("Plugin '" + pluginName + "' requires missing dependency: " + missingDependency, pluginName)
            , missingDependencies_{missingDependency}
        {
        }

        /**
         * @brief Construct with plugin name and list of missing dependencies.
         */
        PluginDependencyException(const std::string& pluginName, std::vector<std::string> missingDependencies)
            : PluginException(buildMessage(pluginName, missingDependencies), pluginName)
            , missingDependencies_(std::move(missingDependencies))
        {
        }

        /**
         * @brief Get the list of missing dependencies.
         */
        [[nodiscard]] auto getMissingDependencies() const noexcept -> const std::vector<std::string>&
        {
            return missingDependencies_;
        }

    private:
        [[nodiscard]] static auto buildMessage(const std::string& pluginName,
                                                const std::vector<std::string>& deps) -> std::string
        {
            std::string msg = "Plugin '" + pluginName + "' has missing dependencies: ";
            for (std::size_t i = 0; i < deps.size(); ++i)
            {
                if (i > 0) msg += ", ";
                msg += deps[i];
            }
            return msg;
        }

        std::vector<std::string> missingDependencies_;
    };

    /**
     * @brief Exception thrown when a circular dependency is detected.
     */
    class PluginCircularDependencyException : public PluginException
    {
    public:
        /**
         * @brief Construct with the dependency cycle.
         */
        explicit PluginCircularDependencyException(std::vector<std::string> cycle)
            : PluginException(buildMessage(cycle))
            , cycle_(std::move(cycle))
        {
        }

        /**
         * @brief Get the circular dependency chain.
         */
        [[nodiscard]] auto getCycle() const noexcept -> const std::vector<std::string>&
        {
            return cycle_;
        }

    private:
        [[nodiscard]] static auto buildMessage(const std::vector<std::string>& cycle) -> std::string
        {
            std::string msg = "Circular dependency detected: ";
            for (std::size_t i = 0; i < cycle.size(); ++i)
            {
                if (i > 0) msg += " -> ";
                msg += cycle[i];
            }
            return msg;
        }

        std::vector<std::string> cycle_;
    };

    /**
     * @brief Exception thrown when plugin API version is incompatible.
     */
    class PluginVersionMismatchException : public PluginException
    {
    public:
        /**
         * @brief Construct with version mismatch details.
         */
        PluginVersionMismatchException(const std::string& pluginName,
                                        std::string requiredVersion,
                                        std::string actualVersion)
            : PluginException("Plugin '" + pluginName + "' version mismatch: required " +
                              requiredVersion + ", got " + actualVersion, pluginName)
            , requiredVersion_(std::move(requiredVersion))
            , actualVersion_(std::move(actualVersion))
        {
        }

        /**
         * @brief Get the required version.
         */
        [[nodiscard]] auto getRequiredVersion() const noexcept -> const std::string&
        {
            return requiredVersion_;
        }

        /**
         * @brief Get the actual version.
         */
        [[nodiscard]] auto getActualVersion() const noexcept -> const std::string&
        {
            return actualVersion_;
        }

    private:
        std::string requiredVersion_;
        std::string actualVersion_;
    };

    /**
     * @brief Exception thrown when plugin state transition is invalid.
     */
    class PluginStateException : public PluginException
    {
    public:
        using PluginException::PluginException;

        /**
         * @brief Construct with plugin name and state details.
         */
        PluginStateException(const std::string& pluginName,
                             const std::string& currentState,
                             const std::string& requestedState)
            : PluginException("Invalid state transition for plugin '" + pluginName +
                              "': cannot transition from " + currentState + " to " + requestedState, pluginName)
            , currentState_(currentState)
            , requestedState_(requestedState)
        {
        }

        [[nodiscard]] auto getCurrentState() const noexcept -> const std::string&
        {
            return currentState_;
        }

        [[nodiscard]] auto getRequestedState() const noexcept -> const std::string&
        {
            return requestedState_;
        }

    private:
        std::string currentState_;
        std::string requestedState_;
    };

    /**
     * @brief Exception thrown when a plugin is not found.
     */
    class PluginNotFoundException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin name.
         */
        explicit PluginNotFoundException(const std::string& pluginName)
            : PluginException("Plugin not found: " + pluginName, pluginName)
        {
        }
    };

    /**
     * @brief Exception thrown when plugin security validation fails.
     */
    class PluginSecurityException : public PluginException
    {
    public:
        using PluginException::PluginException;

        /**
         * @brief Construct with plugin name and security violation.
         */
        PluginSecurityException(const std::string& pluginName, const std::string& violation)
            : PluginException("Security violation for plugin '" + pluginName + "': " + violation, pluginName)
            , violation_(violation)
        {
        }

        /**
         * @brief Get the security violation details.
         */
        [[nodiscard]] auto getViolation() const noexcept -> const std::string&
        {
            return violation_;
        }

    private:
        std::string violation_;
    };

    /**
     * @brief Exception thrown when plugin validation fails.
     */
    class PluginValidationException : public PluginException
    {
    public:
        /**
         * @brief Construct with plugin name and validation errors.
         */
        PluginValidationException(const std::string& pluginName, std::vector<std::string> errors)
            : PluginException(buildMessage(pluginName, errors), pluginName)
            , errors_(std::move(errors))
        {
        }

        /**
         * @brief Get the validation errors.
         */
        [[nodiscard]] auto getErrors() const noexcept -> const std::vector<std::string>&
        {
            return errors_;
        }

    private:
        [[nodiscard]] static auto buildMessage(const std::string& pluginName,
                                                const std::vector<std::string>& errors) -> std::string
        {
            std::string msg = "Plugin '" + pluginName + "' validation failed: ";
            for (std::size_t i = 0; i < errors.size(); ++i)
            {
                if (i > 0) msg += "; ";
                msg += errors[i];
            }
            return msg;
        }

        std::vector<std::string> errors_;
    };

    /**
     * @brief Exception thrown when plugin communication fails.
     */
    class PluginCommunicationException : public PluginException
    {
    public:
        using PluginException::PluginException;

        /**
         * @brief Construct with source and target plugins.
         */
        PluginCommunicationException(const std::string& sourcePlugin,
                                      const std::string& targetPlugin,
                                      const std::string& reason)
            : PluginException("Communication failed from '" + sourcePlugin + "' to '" +
                              targetPlugin + "': " + reason, sourcePlugin)
            , targetPlugin_(targetPlugin)
        {
        }

        [[nodiscard]] auto getTargetPlugin() const noexcept -> const std::string&
        {
            return targetPlugin_;
        }

    private:
        std::string targetPlugin_;
    };

} // namespace com::github::doevelopper::atlassians::plugin

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_PLUGIN_CORE_PLUGINEXCEPTIONS_HPP
