#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>
#include <stdexcept>
#include <filesystem>
#include <regex>
#include <log4cxx/logger.h>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>
#include <com/github/doevelopper/atlassians/semver/VersionConstraint.hpp>

namespace com::github::doevelopper::atlassians::launcher
{

    /**
     * @class Package
     * @brief Represents a software package with version and architecture information
     *
     * The Package class encapsulates information about a software package,
     * including its name, version, architecture, and build information.
     * It provides functionality to parse package names and extract metadata.
     */
    class Package
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        /**
         * @brief Default constructor
         *
         * Initializes a Package with empty fields
         */
        Package() noexcept;

        /**
         * @brief Constructor with package name
         *
         * @param name The package name (may include version and architecture information)
         * @throw std::invalid_argument If the name is empty
         */
        explicit Package(std::string name) noexcept;

        /**
         * @brief Constructor with name and version
         *
         * @param name The package name
         * @param version The package version
         * @throw std::invalid_argument If the name is empty
         */
        Package(const std::string& name, const com::github::doevelopper::atlassians::semver::Version& version) noexcept;

        /**
         * @brief Destructor
         */
        ~Package() noexcept;

        /**
         * @brief Get the package name
         *
         * @return The full package name, including version if available
         */
        std::string pkgName();
    private:
        /**
         * @brief Parse a package name to extract metadata
         *
         * @param fullName The full package name to parse
         */
        void parseName(const std::string& fullName);

        /**
         * @brief Determine the root directory for this package
         *
         * This method attempts to find the installation root directory
         * for the package based on environment variables or default locations.
         */
        void determineRoot();

        // Public member variables as specified in the original code
        std::string root;
        std::string name;
        std::string version;
        std::string arch;
        std::string build;
        std::string tag;
    };

// Define the static logger
//LOG4CXX_DEFINE_STATIC_LOGGER(Package::logger, "Package");
}
#endif // PACKAGE_H
