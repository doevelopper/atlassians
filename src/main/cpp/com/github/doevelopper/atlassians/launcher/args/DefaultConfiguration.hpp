/*******************************************************************
 * @file   DefaultConfiguration.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Default configuration implementation
 *
 * Provides a standard configuration setup with common options
 * for typical command-line applications.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_DEFAULTCONFIGURATION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_DEFAULTCONFIGURATION_HPP

#include <functional>
#include <string>
#include <vector>

#include <com/github/doevelopper/atlassians/launcher/args/AbstractConfiguration.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args
{
    /**
     * @brief Standard value type for options
     */
    enum class OptionValueType : std::uint8_t
    {
        Null,       ///< No value (flag)
        Integer,    ///< Integer value
        String,     ///< String value
        Boolean,    ///< Boolean value
        StringList, ///< Multiple strings
        Real        ///< Floating point
    };

    /**
     * @brief Handler for unparsed arguments
     */
    using UnparsedHandler = std::function<void(const std::vector<std::string>&)>;

    /**
     * @brief Default configuration with standard options
     *
     * Provides a ready-to-use configuration with:
     * - Standard --help and --version options
     * - --config option for configuration file
     * - --verbose/-v for verbosity level
     * - --quiet/-q for quiet mode
     * - --debug/-d for debug mode
     * - Platform-specific options
     *
     * @example
     * ```cpp
     * DefaultConfiguration config;
     * config.setProgramInfo("myapp", "My Application v1.0");
     *
     * // Add custom options
     * config.getOptionsDescription().add_options()
     *     ("port,p", po::value<int>()->default_value(8080), "Server port")
     *     ("output,o", po::value<std::string>(), "Output file");
     *
     * if (!config.parse(argc, argv))
     * {
     *     return 1;
     * }
     *
     * if (config.isHelpRequested())
     * {
     *     std::cout << config.getHelpMessage();
     *     return 0;
     * }
     *
     * int port = config.get<int>("port", 8080);
     * ```
     */
    class DefaultConfiguration : public AbstractConfiguration
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        /**
         * @brief Default constructor
         */
        DefaultConfiguration();

        /**
         * @brief Construct with program info
         */
        DefaultConfiguration(const std::string& programName, const std::string& description);

        /**
         * @brief Construct and parse immediately
         */
        DefaultConfiguration(int argc, const char* argv[]);

        /**
         * @brief Construct with config file
         */
        DefaultConfiguration(int argc, const char* argv[], const std::string& configFile);

        /**
         * @brief Construct with unparsed handler
         */
        DefaultConfiguration(int argc, const char* argv[], UnparsedHandler handler);

        /**
         * @brief Construct with unparsed handler and config file
         */
        DefaultConfiguration(int argc, const char* argv[],
                             UnparsedHandler handler,
                             const std::string& configFile);

        DefaultConfiguration(const DefaultConfiguration&) = delete;
        DefaultConfiguration(DefaultConfiguration&&) = default;
        DefaultConfiguration& operator=(const DefaultConfiguration&) = delete;
        DefaultConfiguration& operator=(DefaultConfiguration&&) = default;
        ~DefaultConfiguration() noexcept override;

        // ===========================================
        // Convenience Accessors
        // ===========================================

        /**
         * @brief Get verbosity level (0 = normal, 1+ = verbose)
         */
        [[nodiscard]] auto getVerbosity() const -> int;

        /**
         * @brief Check if quiet mode is enabled
         */
        [[nodiscard]] auto isQuiet() const -> bool;

        /**
         * @brief Check if debug mode is enabled
         */
        [[nodiscard]] auto isDebugMode() const -> bool;

        /**
         * @brief Get the config file path
         */
        [[nodiscard]] auto getConfigFilePath() const -> std::string;

        /**
         * @brief Set handler for unparsed arguments
         */
        auto setUnparsedHandler(UnparsedHandler handler) -> DefaultConfiguration&;

    protected:

        /**
         * @brief Define standard options
         */
        void defineOptions() override;

        /**
         * @brief Validate required options
         */
        auto validateOptions() -> bool override;

    private:

        /**
         * @brief Add platform-specific options
         */
        void addPlatformOptions();

        UnparsedHandler m_unparsedHandler;
        bool m_parsed{false};
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_DEFAULTCONFIGURATION_HPP
