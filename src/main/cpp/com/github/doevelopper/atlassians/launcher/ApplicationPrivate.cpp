#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/exception.h>
#include <thread>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <stdexcept>

#include <com/github/doevelopper/atlassians/launcher/Application.hpp>
#include <com/github/doevelopper/atlassians/launcher/ApplicationPrivate.hpp>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializationFactory.hpp>
#include <com/github/doevelopper/atlassians/logging/DefaultInitializationStrategy.hpp>
#include <com/github/doevelopper/atlassians/semver/GitRevision.hpp>

using namespace com::github::doevelopper::atlassians::launcher;
using namespace com::github::doevelopper::atlassians::semver;
using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr ApplicationPrivate::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.ApplicationPrivate"));

ApplicationPrivate::ApplicationPrivate(Application* q) noexcept
    : q_ptr(q)
    , parseResult_{}
{
    this->initializeLogging();

    // Configure parser with standard options
    parser.addStandardOptions();

    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Constructor called");
}

ApplicationPrivate::ApplicationPrivate(Application* q, int argc, char** argv) noexcept
    : q_ptr(q)
    , argc(argc)
    , argv(argv)
    , parseResult_{}
{
    this->initializeLogging();

    // Configure parser with standard options (help, version)
    parser.addStandardOptions();

    // Parse command-line arguments
    parseResult_ = parser.parse(argc, argv);

    if (!parseResult_.success)
    {
        LOG4CXX_WARN(logger, "Command line parsing failed: " << parseResult_.errorMessage);
    }

    // Handle help/version immediately if requested
    if (helpRequested())
    {
        showHelp();
    }
    else if (versionRequested())
    {
        showVersion();
    }

    LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Constructor with arguments called");
}

ApplicationPrivate::~ApplicationPrivate() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // Use LoggingInitializer for proper shutdown (handles logging of end message)
        com::github::doevelopper::atlassians::logging::LoggingInitializer::getInstance().shutdown();
    }
    catch (const std::exception& e)
    {
        // Don't let exceptions escape from destructor
        std::cerr << "Exception during logging shutdown: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception during logging shutdown" << std::endl;
    }

    // try {
    //     if (state.load() == State::Running) {
    //         LOG4CXX_WARN(logger, "Application is still running during destruction, forcing shutdown");
    //         shutdown("Destructor called");
    //     }
    //
    //     if (appThread && appThread->joinable()) {
    //         appThread->join();
    //     }
    //
    //     LOG4CXX_DEBUG(logger, "ApplicationPrivate destroyed");
    // } catch (const std::exception& e) {
    //     // Don't let exceptions escape the destructor
    //     LOG4CXX_ERROR(logger, "Exception in destructor: " << e.what());
    // } catch (...) {
    //     LOG4CXX_ERROR(logger, "Unknown exception in destructor");
    // }
}

bool ApplicationPrivate::initialize()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // State transition check
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        if (state.load() != State::NotInitialized)
        {
            LOG4CXX_WARN(logger, __LOG4CXX_FUNC__ << " Application already initialized or initializing");
            return false;
        }
        state.store(State::Initializing);
    }

    try
    {
        // // Initialize logging first to ensure proper error reporting
        // if (!initializeLogging()) {
        //     LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ <<  " Failed to initialize logging");
        //     state.store(State::NotInitialized);
        //     return false;
        // }

        // Setup configuration
        if (!setupConfiguration()) {
            LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ <<  " Failed to setup configuration");
            state.store(State::NotInitialized);
            return false;
        }

        // Additional initialization steps can be added here

        // Transition to initialized state
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            state.store(State::Initialized);
            stateCondition.notify_all();
        }

        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ <<  " Application initialized successfully");
        return true;
    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ <<  " Initialization failed with exception: " << e.what());
        state.store(State::NotInitialized);
        throw std::runtime_error(std::string("Initialization failed: ") + e.what());
    }
    catch (...)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ <<  " Initialization failed with unknown exception");
        state.store(State::NotInitialized);
        throw std::runtime_error("Initialization failed with unknown error");
    }
}

bool ApplicationPrivate::initializeLogging()
{
    try
    {
        // Use Default initialization strategy via Factory
        // This provides console output with BasicConfigurator
        // TODO: Replace with PropertyFile/XMLFile/Environment strategy when command-line args handler is implemented
        auto strategy = com::github::doevelopper::atlassians::logging::LoggingInitializationFactory::create(
            com::github::doevelopper::atlassians::logging::InitializationStrategyType::Default);

        // Use the singleton to initialize logging system
        auto& initializer = com::github::doevelopper::atlassians::logging::LoggingInitializer::getInstance();
        initializer.initialize(std::move(strategy));

        // Log successful initialization
        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ << " Logging initialized with Default strategy");
        return true;
    }
    catch (const com::github::doevelopper::atlassians::logging::LoggingInitializationException& e)
    {
        std::cerr << "Logging initialization failed: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Failed to initialize logging: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cerr << "Failed to initialize logging with unknown error" << std::endl;
        return false;
    }
}

bool ApplicationPrivate::setupConfiguration()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // TODO: Implement configuration loading when needed
    // For now, use default settings
    LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ << " Using default configuration settings");
    return true;
}

std::future<void> ApplicationPrivate::run()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    try
    {
        // Reset the promise before starting
        this->startPromise = std::promise<void>();

        // Check for early exit conditions (help or version requested)
        if (shouldExitEarly())
        {
            LOG4CXX_DEBUG(logger, __LOG4CXX_FUNC__ << " Early exit: help or version was displayed");
            startPromise.set_value();
            return startPromise.get_future();
        }

        // Check if parsing failed
        if (!parseResult_.success)
        {
            LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " Command-line parsing failed: " << parseResult_.errorMessage);
            startPromise.set_exception(std::make_exception_ptr(
                std::runtime_error("Command-line parsing failed: " + parseResult_.errorMessage)));
            return startPromise.get_future();
        }

        // Check if already running
        if (state.load() == State::Running)
        {
            LOG4CXX_WARN(logger, __LOG4CXX_FUNC__ << " Application is already running");
            startPromise.set_value(); // Fulfill the promise immediately
            return startPromise.get_future();
        }

        // Check if initialized
        if (state.load() != State::Initialized)
        {
            // Try to initialize if not already done
            if (!this->initialize())
            {
                LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " Failed to initialize application");
                startPromise.set_exception(std::make_exception_ptr( std::runtime_error("Failed to initialize application")));
                return startPromise.get_future();
            }
        }

        // Update state and create thread
        {
            std::lock_guard<std::mutex> lock(stateMutex);
            state.store(State::Running);
            stateCondition.notify_all();
        }

        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ << " Application initialized and running");

        // For now, just complete immediately (no background loop)
        // TODO: Implement actual application loop when needed
        startPromise.set_value();

        LOG4CXX_INFO(logger, __LOG4CXX_FUNC__ << " Application completed successfully");
        return startPromise.get_future();

    }
    catch (const std::exception& e)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " Failed to start application: " << e.what());

        // Set the exception to propagate it to the caller
        startPromise.set_exception(std::current_exception());
        return startPromise.get_future();
    }
    catch (...)
    {
        LOG4CXX_ERROR(logger, __LOG4CXX_FUNC__ << " Failed to start application with unknown error");

        // Set the exception to propagate it to the caller
        startPromise.set_exception(std::make_exception_ptr(std::runtime_error("Failed to start application with unknown error")));
        return startPromise.get_future();
    }
}

void ApplicationPrivate::applicationLoop()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    LOG4CXX_INFO(logger, "Application main loop started");

    // Main application loop
    while (!shutdownRequested.load()) {
        // Process events, update application state, etc.

        // For now, just sleep to avoid high CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    LOG4CXX_INFO(logger, "Application main loop ended");

    // Update state to indicate shutdown
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        state.store(State::Shutdown);
        stateCondition.notify_all();
    }
}

void ApplicationPrivate::performShutdown(const std::string& reason)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    LOG4CXX_INFO(logger, "Shutting down application: " << reason);

    // Set the shutdown flag to stop the main loop
    shutdownRequested.store(true);
    shutdownReason = reason;

    // Update state
    {
        std::lock_guard<std::mutex> lock(stateMutex);
        state.store(State::ShuttingDown);
        stateCondition.notify_all();
    }
}

std::future<void> ApplicationPrivate::shutdown(const std::string& reason)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    // Reset the promise
    shutdownPromise = std::promise<void>();

    try {
        // If not running, return immediately
        if (state.load() != State::Running) {
            LOG4CXX_WARN(logger, "Application is not running, cannot shut down");
            shutdownPromise.set_value();
            return shutdownPromise.get_future();
        }

        // Initiate shutdown
        performShutdown(reason);

        // Create a thread to wait for the application to shut down
        std::thread shutdownThread([this]() {
            try {
                // Wait for application thread to finish
                if (appThread && appThread->joinable()) {
                    appThread->join();
                }

                LOG4CXX_INFO(logger, "Application shutdown complete");
                shutdownPromise.set_value();
            } catch (const std::exception& e) {
                LOG4CXX_ERROR(logger, "Exception during shutdown: " << e.what());
                shutdownPromise.set_exception(std::current_exception());
            } catch (...) {
                LOG4CXX_ERROR(logger, "Unknown exception during shutdown");
                shutdownPromise.set_exception(std::make_exception_ptr(
                    std::runtime_error("Unknown exception during shutdown")));
            }
        });

        // Detach the thread as we'll track completion via the promise
        shutdownThread.detach();

        return shutdownPromise.get_future();
    } catch (const std::exception& e) {
        LOG4CXX_ERROR(logger, "Failed to initiate shutdown: " << e.what());
        shutdownPromise.set_exception(std::current_exception());
        return shutdownPromise.get_future();
    } catch (...) {
        LOG4CXX_ERROR(logger, "Failed to initiate shutdown with unknown error");
        shutdownPromise.set_exception(std::make_exception_ptr(
            std::runtime_error("Failed to initiate shutdown with unknown error")));
        return shutdownPromise.get_future();
    }
}

void ApplicationPrivate::showHelp() const
{
    std::cout << "Usage: " << LIBRARY_NAME << " [options]\n\n"
              << LIBRARY_DESCRIPTION << "\n\n"
              << parser.getHelpText()
              << std::endl;
}

void ApplicationPrivate::showVersion() const
{
    std::cout << LIBRARY_NAME << " " << GitRevision::versionString() << "\n"
              << "\n"
              << "Git Commit: " << GitRevision::commitHashShort();

    if (GitRevision::isDirty())
    {
        std::cout << " (dirty)";
    }
    std::cout << "\n";

    std::cout << "Git Branch: " << GitRevision::branch() << "\n"
              << "Build Type: " << GitRevision::buildType() << "\n"
              << "Build Time: " << GitRevision::buildTimestamp() << "\n"
              << "Compiler:   " << GitRevision::compilerId() << " " << GitRevision::compilerVersion() << "\n"
              << std::endl;
}

bool ApplicationPrivate::helpRequested() const noexcept
{
    try
    {
        auto helpOpt = parser.getValue<bool>("help");
        return parseResult_.success && helpOpt.has_value() && helpOpt.value();
    }
    catch (...)
    {
        return false;
    }
}

bool ApplicationPrivate::versionRequested() const noexcept
{
    try
    {
        auto versionOpt = parser.getValue<bool>("version");
        return parseResult_.success && versionOpt.has_value() && versionOpt.value();
    }
    catch (...)
    {
        return false;
    }
}

bool ApplicationPrivate::shouldExitEarly() const noexcept
{
    return helpRequested() || versionRequested();
}
