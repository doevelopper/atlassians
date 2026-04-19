//
// Created by hroland on 26/05/24.
//

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_APPLICATIONPRIVATE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_APPLICATIONPRIVATE_HPP

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio/signal_set.hpp>

#include <string>
#include <future>
#include <memory>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <typeindex>

#include <com/github/doevelopper/atlassians/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/logging/LoggingInitializationFactory.hpp>
#include <com/github/doevelopper/atlassians/semver/Version.hpp>
#include <com/github/doevelopper/atlassians/semver/GitRevision.hpp>

#include <com/github/doevelopper/atlassians/launcher/args/CommandLineParser.hpp>

namespace com::github::doevelopper::atlassians::launcher
{

    // Forward declarations
    class Application;

    /**
     * @brief Private implementation of the Application class
     *
     * This class implements the PIMPL (Pointer to Implementation) pattern for the Application class.
     * It encapsulates the implementation details of the Application class to reduce compilation
     * dependencies and provide better ABI stability.
     */
    class ApplicationPrivate
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
        Q_DECLARE_PUBLIC(Application)
    public:
        /**
         * @brief Constructor
         *
         * @param q Pointer to the public interface
         */
        explicit ApplicationPrivate(Application* q) noexcept;
        // explicit ApplicationPrivate() noexcept;
        /**
         * @brief Constructor with command-line arguments
         *
         * @param q Pointer to the public interface
         * @param argc Argument count
         * @param argv Argument values
         */
        ApplicationPrivate(Application* q, int argc, char** argv) noexcept;
        // ApplicationPrivate(int argc, char** argv) noexcept;

        /**
         * @brief Destructor
         */
        ~ApplicationPrivate() noexcept;

        /**
         * @brief Initialize the application
         *
         * @return true if initialization succeeded, false otherwise
         * @throws std::runtime_error if critical initialization fails
         */
        bool initialize();

        /**
         * @brief Run the application
         *
         * @return Future that resolves when the application has started
         */
        std::future<void> run();

        /**
         * @brief Shutdown the application
         *
         * @param reason Reason for shutdown
         * @return Future that resolves when shutdown is complete
         */
        std::future<void> shutdown(const std::string& reason);

        Application * q_ptr;
    private:
        /**
         * @brief Process command-line arguments
         *
         * @param argc Argument count
         * @param argv Argument values
         * @return true if arguments are valid, false otherwise
         */
        bool processArguments(int argc, char** argv);

        /**
         * @brief Set up application configuration
         *
         * @return true if configuration succeeded, false otherwise
         * @throws std::runtime_error if critical configuration fails
         */
        bool setupConfiguration();

        /**
         * @brief Initialize logging
         *
         * @return true if logging initialization succeeded, false otherwise
         */
        bool initializeLogging();

        /**
         * @brief The main application loop
         */
        void applicationLoop();

        /**
         * @brief Perform shutdown operations
         *
         * @param reason Reason for shutdown
         */
        void performShutdown(const std::string& reason);

        /**
         * @brief Display help information to stdout
         */
        void showHelp() const;

        /**
         * @brief Display version information to stdout
         */
        void showVersion() const;

        /**
         * @brief Check if help was requested
         * @return true if --help was specified
         */
        [[nodiscard]] bool helpRequested() const noexcept;

        /**
         * @brief Check if version was requested
         * @return true if --version was specified
         */
        [[nodiscard]] bool versionRequested() const noexcept;

        /**
         * @brief Check if early exit is needed (help or version requested)
         * @return true if application should exit early
         */
        [[nodiscard]] bool shouldExitEarly() const noexcept;

        // Application state
        enum class State
        {
            NotInitialized,
            Initializing,
            Initialized,
            Running,
            ShuttingDown,
            Shutdown
        };
        std::atomic<State> state{State::NotInitialized};

        // Command-line arguments
        int argc{0};
        char** argv{nullptr};

        // Application version
        std::unique_ptr<com::github::doevelopper::atlassians::semver::Version> appVersion;

        // Application configuration
        std::string configPath;
        std::string appName;
        std::string appDescription;

        // Thread synchronization
        std::mutex stateMutex;
        std::condition_variable stateCondition;

        // Shutdown signal
        std::atomic<bool> shutdownRequested{false};
        std::string shutdownReason;

        // Thread for application loop
        std::unique_ptr<std::thread> appThread;

        // Promise for signaling application start completion
        std::promise<void> startPromise;

        // Promise for signaling application shutdown completion
        std::promise<void> shutdownPromise;

        // Command-line parser and result
        com::github::doevelopper::atlassians::launcher::args::CommandLineParser parser;
        com::github::doevelopper::atlassians::launcher::args::ParseResult parseResult_;
    };
}

#endif
