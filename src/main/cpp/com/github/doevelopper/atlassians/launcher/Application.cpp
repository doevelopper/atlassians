/*******************************************************************
* @file
 * @version  v0.0.
 * @date     2024/04/18 W 09:55:59
 * @brief    Main entry point of application
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2023 ACME Technology Corp. All rights reserved.
********************************************************************/

#include <log4cxx/helpers/exception.h>
#include <stdexcept>


#include <com/github/doevelopper/atlassians/launcher/Application.hpp>
#include <com/github/doevelopper/atlassians/launcher/ApplicationPrivate.hpp>

using namespace com::github::doevelopper::atlassians::launcher;
using namespace com::github::doevelopper::atlassians::semver;

log4cxx::LoggerPtr Application::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.Application"));


// LOG4CXX_IMPL_STATIC_LOGGER(Application, "Application")
// LOG4CXX_IMPL_STATIC_LOGGER(Application::Package, "Application.Package")
//
// // Package implementation
//
// Application::Package::Package()
//     : root("")
//     , name("")
//     , version("")
//     , arch("")
//     , build("")
//     , tag("")
// {
//     LOG4CXX_TRACE(logger, "Created empty package");
// }
//
// Application::Package::Package(std::string name)
//     : root("")
//     , name(std::move(name))
//     , version("")
//     , arch("")
//     , build("")
//     , tag("")
// {
//     LOG4CXX_TRACE(logger, "Created package with name: " << this->name);
// }
//
// Application::Package::Package(const std::string& name, const Version& version)
//     : root("")
//     , name(name)
//     , version(version.toString())
//     , arch("")
//     , build("")
//     , tag("")
// {
//     LOG4CXX_TRACE(logger, "Created package with name: " << this->name << " and version: " << this->version);
// }
//
// Application::Package::~Package()
// {
//     LOG4CXX_TRACE(logger, "Destroyed package: " << name);
// }
//
// std::string Application::Package::pkgName()
// {
//     return name + "-" + version + (tag.empty() ? "" : "-" + tag);
// }
//
// // Application implementation
//
Application::Application() noexcept
    : d_ptr(std::make_unique<ApplicationPrivate>(this))
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
    try
    {
        // Perform basic initialization
        d_ptr->initialize();
    }
    catch (const std::exception& e)
    {

        // We're in a noexcept constructor, so we can't throw
        // Just log the error and continue with a partially initialized state
        LOG4CXX_ERROR(logger,  __LOG4CXX_FUNC__ << " Initialization failed: " << e.what());
    }
    catch (...)
    {
        LOG4CXX_ERROR(logger,  __LOG4CXX_FUNC__ << " Initialization failed with unknown exception");
    }
}

Application::Application([[maybe_unused]] int argc, [[maybe_unused]] char** argv) noexcept
    : d_ptr(std::make_unique<ApplicationPrivate>(this, argc, argv))
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
    // try
    // {
    //     // Perform initialization with arguments
    //     // d_ptr->initialize();
    // }
    // catch (const std::exception& e)
    // {
    // //     LOG4CXX_ERROR(logger, "Initialization failed: " << e.what());
    // //     // We're in a noexcept constructor, so we can't throw
    // }
    // catch (...)
    // {
    // //     LOG4CXX_ERROR(logger, "Initialization failed with unknown exception");
    // }
}

Application::~Application() noexcept
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);

    //     try {
    //         // If the application is still running, shut it down
    //         if (isRunning()) {
    //             LOG4CXX_WARN(logger, "Application is still running during destruction, forcing shutdown");
    //             shutdown("Destructor called").wait();
    //         }
    //     } catch (const std::exception& e) {
    //         // Don't let exceptions escape the destructor
    //         LOG4CXX_ERROR(logger, "Exception in destructor: " << e.what());
    //     } catch (...) {
    //         LOG4CXX_ERROR(logger, "Unknown exception in destructor");
    //     }
    // LOG4CXX_DEBUG(logger, "Application destroyed");
}

std::future<void> Application::run()
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
    try
    {
        // Delegate to the private implementation
        LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__ << " Delegate to the private implementation.");
        // return std::async(std::launch::async, [this] { d_ptr->run(); });
        return d_ptr->run();
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " Failed to start application: " << e.what());
        // Create a future with the exception
        std::promise<void> promise;
        promise.set_exception(std::current_exception());
        return promise.get_future();
    }
}

std::future<void> Application::shutdown(const std::string& reason)
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__ << "  " << reason);

     try
     {
            return d_ptr->shutdown(reason);
        // return std::async(std::launch::async, [this, reason] { d_ptr->shutdown(reason); });
     }
    catch (const std::exception& e)
    {
         LOG4CXX_ERROR(logger, "Failed to shutdown application: " << e.what());
//
//         // Create a future with the exception
//         std::promise<void> promise;
//         promise.set_exception(std::current_exception());
//         return promise.get_future();
     }
}

Version Application::getVersion() const
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
//     // Implementation depends on your Version class
//     // This is a placeholder
     return Version();
}
//
std::string Application::getName() const
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
//     // This would typically access a member of ApplicationPrivate
//     // For demonstration, we return a placeholder
     return "ApplicationName";
}

// std::string Application::getDescription() const
// {
//     // This would typically access a member of ApplicationPrivate
//     // For demonstration, we return a placeholder
//     return "Application Description";
// }
//
bool Application::isRunning() const
{
    LOG4CXX_DEBUG(logger,  __LOG4CXX_FUNC__);
//     // This would check the state in ApplicationPrivate
//     // For demonstration, we use a naive implementation
//     // In a real implementation, you would access the state in ApplicationPrivate
//     try {
//         // Placeholder for accessing state in ApplicationPrivate
//         return true; // Replace with actual implementation
//     } catch (const std::exception& e) {
//         LOG4CXX_ERROR(logger, "Error checking if application is running: " << e.what());
//         return false;
//     }
    return false;
}
