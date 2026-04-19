/*******************************************************************
* @file
 * @version  v0.0.
 * @date     2024/04/18 W 09:55:59
 * @brief    Main entry point of application
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2023 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_APPLICATION_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_APPLICATION_HPP

#include <future>
#include <cstdlib>

#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/IOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/Package.hpp>
#include <com/github/doevelopper/atlassians/launcher/Package.hpp>
namespace com::github::doevelopper::atlassians::launcher
{

    using StatusCode = std::uint_fast64_t;
    constexpr StatusCode SUCCESS = EXIT_SUCCESS;
    constexpr StatusCode FAILURE = EXIT_FAILURE;

    class ApplicationPrivate;
    /**
     * @brief Main application class
     *
     * This class provides the public interface for the application.
     * It uses the PIMPL (Pointer to Implementation) pattern to hide implementation details.
     */
    class Application
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
        Q_DECLARE_PRIVATE(Application)
        Q_DISABLE_COPY_MOVE(Application)

    public:
        Application() noexcept;
        virtual ~Application() noexcept;
        /*!
         * @brief Constructor with command-line arguments
         *
         * @param argc Argument count
         * @param argv Argument values
         */
        Application([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) noexcept;

        /*! @name Essential operations
         * These are the commands used to run and stop.
         *  @{
         */

        /**
         * @brief Run the application
         * @return Future that resolves when the application has started
         */
        std::future<void> run();

        /**
         * @brief Shutdown the application
         * @param reason Reason for shutdown
         * @return Future that resolves when shutdown is complete
         */
        std::future<void> shutdown(const std::string& reason);
        /** @} */

        /**
            * @brief Get the application version
            *
            * @return The application version
            */
        com::github::doevelopper::atlassians::semver::Version getVersion() const;

        /**
         * @brief Get the application name
         *
         * @return The application name
         */
        std::string getName() const;

        /**
         * @brief Get the application description
         *
         * @return The application description
         */
        std::string getDescription() const;

        /**
         * @brief Check if the application is running
         *
         * @return true if the application is running, false otherwise
         */
        bool isRunning() const;

        /**
         * @brief Get the application package information
         *
         * @return The application package
         */
        Package getPackage() const;
    protected:
    private:
        std::unique_ptr<ApplicationPrivate> d_ptr;
        // Package m_package;
    };
}

#endif
