
#include <stack>
#include <stdexcept>
#include <string>
#include <memory>
#include <variant>
#include <optional>
#include <cmath>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/atlas/NoOp.hpp>

using com::github::doevelopper::premisses::atlas::NoOp;
using cucumber::ScenarioScope;

// ============================================
// Context Structure for Sharing State
// ============================================

struct NoOp
{
    std::unique_ptr<NoOp> noop;
};

// ============================================
// Background Steps
// ============================================

GIVEN("^a NoOp instance$") {
    ScenarioScope<NoOp> context;
    context->noop = std::make_unique<NoOp>();
}

WHEN("^the NoOp instance is created$") {
    // No additional action needed; instance created in GIVEN step
}

THEN("^the NoOp instance should not be null$") {
    ScenarioScope<NoOp> context;
    ASSERT_NE(context->noop, nullptr);
}

