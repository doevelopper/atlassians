
#include <com/github/doevelopper/atlassians/utdriver/EnvironmentRecorder.hpp>

using namespace com::github::doevelopper::atlassians::utdriver;

// Initialize static logger
log4cxx::LoggerPtr EnvironmentRecorder::logger =
    log4cxx::Logger::getLogger("com.github.doevelopper.atlassians.utdriver.EnvironmentRecorder");

EnvironmentRecorder::EnvironmentRecorder() noexcept
{
    LOG4CXX_TRACE(logger, "EnvironmentRecorder default constructor");
}

EnvironmentRecorder::~EnvironmentRecorder() noexcept
{
    LOG4CXX_TRACE(logger, "EnvironmentRecorder destructor");
    // Note: Google Test owns this object - destructor called automatically
    // Do not perform cleanup here - use TearDown() instead
}

void EnvironmentRecorder::SetUp() {
        char hostname[HOST_NAME_MAX];
        gethostname(hostname, HOST_NAME_MAX);
         struct utsname uts;
        uname(&uts);
        // Record system information
        ::testing::Test::RecordProperty("hostname", hostname);
        ::testing::Test::RecordProperty("user", getenv("USER") ? getenv("USER") : "unknown");
        ::testing::Test::RecordProperty("os",std::string(uts.sysname)
            + std::string(" ")
            + uts.release
            + std::string(" ")
            + uts.version
        );
        ::testing::Test::RecordProperty("compiler",
#ifdef __GNUC__
            "GCC " __VERSION__
#elif defined(_MSC_VER)
            "MSVC"
#elif defined(__clang__)
            "Clang " __clang_version__
#else
            "Unknown"
#endif
        );
        ::testing::Test::RecordProperty("build_type",
#ifdef NDEBUG
            "Release"
#else
            "Debug"
#endif
        );
    }

void EnvironmentRecorder::TearDown() {
        // No specific teardown actions needed
    }
