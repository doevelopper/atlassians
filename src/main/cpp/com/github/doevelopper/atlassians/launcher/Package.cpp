
#include <com/github/doevelopper/atlassians/launcher/Package.hpp>

using namespace com::github::doevelopper::atlassians::launcher;
using namespace com::github::doevelopper::atlassians::semver;

log4cxx::LoggerPtr Package::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.Application"));

Package::Package() noexcept
    : root{}
    , name{}
    , version{}
    , arch{}
    , build{}
    , tag{}
{
    LOG4CXX_DEBUG(logger, "Creating empty Package");
}

Package::Package(std::string name) noexcept
    : root{}
    , name{}
    , version{}
    , arch{}
    , build{}
    , tag{}
{
    if (name.empty())
    {
        LOG4CXX_ERROR(logger, "Attempted to create Package with empty name");
        throw std::invalid_argument("Package name cannot be empty");
    }

    LOG4CXX_DEBUG(logger, "Creating Package with name: " << name);
    //parseName(name);
}

Package::Package(const std::string& name, const Version& version) noexcept
    :  root{}
    ,  name{}
    ,  version{version.toString()}
    ,  arch{}
    ,  build{}
    ,  tag{}
{
    if (name.empty())
    {
        LOG4CXX_ERROR(logger, "Attempted to create Package with empty name");
        throw std::invalid_argument("Package name cannot be empty");
    }

    LOG4CXX_DEBUG(logger, "Creating Package with name: " << name << " and version: " << version.toString());
    this->name = name;
    //determineRoot();
}

Package::~Package()
{
    LOG4CXX_DEBUG(logger, "Destroying Package: " << name);
}
//
// std::string Package::pkgName()
// {
//     std::string result = name;
//
//     if (!version.empty()) {
//         result += "-" + version;
//     }
//
//     if (!arch.empty()) {
//         result += "." + arch;
//     }
//
//     if (!build.empty()) {
//         result += "-" + build;
//     }
//
//     if (!tag.empty()) {
//         result += "." + tag;
//     }
//
//     return result;
// }
//
//
// void Package::parseName(const std::string& fullName)
// {
//             try {
//                 // Regex pattern to match package format: name-version.arch-build.tag
//                 std::regex packagePattern(R"(^([\w\.\-]+?)(?:-([0-9\.]+))?(?:\.([a-zA-Z0-9_]+))?(?:-([a-zA-Z0-9_]+))?(?:\.([a-zA-Z0-9_]+))?$)");
//                 std::smatch matches;
//
//                 if (std::regex_match(fullName, matches, packagePattern)) {
//                     // matches[0] is the full match
//                     // matches[1] is the package name
//                     // matches[2] is the version (if present)
//                     // matches[3] is the architecture (if present)
//                     // matches[4] is the build (if present)
//                     // matches[5] is the tag (if present)
//
//                     name = matches[1].str();
//
//                     if (matches.size() > 2 && matches[2].matched) {
//                         version = matches[2].str();
//                     }
//
//                     if (matches.size() > 3 && matches[3].matched) {
//                         arch = matches[3].str();
//                     }
//
//                     if (matches.size() > 4 && matches[4].matched) {
//                         build = matches[4].str();
//                     }
//
//                     if (matches.size() > 5 && matches[5].matched) {
//                         tag = matches[5].str();
//                     }
//
//                     determineRoot();
//                     LOG4CXX_INFO(logger, "Successfully parsed package name: " << fullName);
//                 } else {
//                     // If the regex doesn't match, use the full name as the package name
//                     name = fullName;
//                     determineRoot();
//                     LOG4CXX_WARN(logger, "Could not parse package name components from: " << fullName);
//                 }
//             } catch (const std::regex_error& e) {
//                 LOG4CXX_ERROR(logger, "Regex error while parsing package name: " << e.what());
//                 name = fullName;
//                 determineRoot();
//             } catch (const std::exception& e) {
//                 LOG4CXX_ERROR(logger, "Error parsing package name: " << e.what());
//                 name = fullName;
//                 determineRoot();
//             }
// }
//
// void Package::determineRoot()
// {
//     try {
//         // First, check if there's an environment variable with the package name
//         std::string envVarName = name + "_ROOT";
//         std::transform(envVarName.begin(), envVarName.end(), envVarName.begin(), ::toupper);
//
//         const char* envRoot = std::getenv(envVarName.c_str());
//         if (envRoot != nullptr) {
//             root = envRoot;
//             LOG4CXX_INFO(logger, "Found package root from environment variable " << envVarName << ": " << root);
//             return;
//         }
//
//         // Check common installation directories
//         std::vector<std::string> commonPaths = {
//             "/opt/" + name,
//             "/usr/local/" + name,
//             "/usr/share/" + name
//         };
//
//         for (const auto& path : commonPaths) {
//             if (std::filesystem::exists(path)) {
//                 root = path;
//                 LOG4CXX_INFO(logger, "Found package root at common path: " << root);
//                 return;
//             }
//         }
//
//         // If all else fails, use the current directory
//         root = std::filesystem::current_path().string();
//         LOG4CXX_WARN(logger, "Could not determine package root. Using current directory: " << root);
//     } catch (const std::filesystem::filesystem_error& e) {
//         LOG4CXX_ERROR(logger, "Filesystem error while determining package root: " << e.what());
//         root = ".";  // Use relative path as fallback
//     } catch (const std::exception& e) {
//         LOG4CXX_ERROR(logger, "Error determining package root: " << e.what());
//         root = ".";  // Use relative path as fallback
//     }
// }
