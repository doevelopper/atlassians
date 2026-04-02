
#include <stack>
#include <stdexcept>
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <cmath>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/logging/LoggingInitializer.hpp>
#include <com/github/doevelopper/premisses/logging/DefaultInitializationStrategy.hpp>

using com::github::doevelopper::premisses::logging::LoggingInitializer;
using com::github::doevelopper::premisses::logging::DefaultInitializationStrategy;

using cucumber::ScenarioScope;

// ============================================
// Context Structure for Sharing State
// ============================================
