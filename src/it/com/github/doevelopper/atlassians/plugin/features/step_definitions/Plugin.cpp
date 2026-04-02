
#include <stack>
#include <stdexcept>
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <cmath>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/plugin/core/BasePlugin.hpp>
#include <com/github/doevelopper/premisses/plugin/core/PluginExport.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

using com::github::doevelopper::premisses::plugin::core::BasePlugin;
using cucumber::ScenarioScope;

// ============================================
// Context Structure for Sharing State
// ============================================
