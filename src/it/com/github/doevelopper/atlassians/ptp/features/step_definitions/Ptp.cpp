#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <com/github/doevelopper/premisses/atlas/time/ptp/PtpEngine.hpp>

namespace {

using com::github::doevelopper::premisses::atlas::time::ptp::PtpEngine;
using com::github::doevelopper::premisses::atlas::time::ptp::PtpEngineBuilder;
using cucumber::ScenarioScope;

struct PtpContext {
    std::unique_ptr<PtpEngine> ptpEngine;
    std::optional<com::github::doevelopper::premisses::atlas::time::ptp::ClockIdentity> selectedPtpMaster;
};

}  // namespace

GIVEN("^a default PTP engine$")
{
    ScenarioScope<PtpContext> context;
    PtpEngineBuilder builder;
    context->ptpEngine = builder.build();
    ASSERT_NE(context->ptpEngine, nullptr);
}

WHEN("^I provide PTP clock candidates$")
{
    ScenarioScope<PtpContext> context;
    using com::github::doevelopper::premisses::atlas::time::ptp::ClockIdentity;

    const std::vector<ClockIdentity> candidates{
        ClockIdentity{128U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-mid"},
        ClockIdentity{110U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-best"},
        ClockIdentity{150U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-low"}};

    context->selectedPtpMaster = context->ptpEngine->electGrandmaster(candidates);
}

THEN("^the selected grandmaster clock id should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedClockId);
    ScenarioScope<PtpContext> context;

    ASSERT_TRUE(context->selectedPtpMaster.has_value());
    EXPECT_EQ(context->selectedPtpMaster->clockId, expectedClockId);
}

WHEN("^I start the PTP engine$")
{
    ScenarioScope<PtpContext> context;
    context->ptpEngine->start();
}

THEN("^the PTP engine state should be \"([^\"]*)\"$")
{
    REGEX_PARAM(std::string, expectedState);
    ScenarioScope<PtpContext> context;

    EXPECT_EQ(context->ptpEngine->currentStateName(), expectedState);
}

GIVEN("^a started PTP engine$")
{
    ScenarioScope<PtpContext> context;
    PtpEngineBuilder builder;
    context->ptpEngine = builder.build();
    ASSERT_NE(context->ptpEngine, nullptr);
    context->ptpEngine->start();
}

WHEN("^I receive an announce message$")
{
    ScenarioScope<PtpContext> context;
    using com::github::doevelopper::premisses::atlas::time::ptp::AnnounceMessage;
    using com::github::doevelopper::premisses::atlas::time::ptp::ClockIdentity;

    const AnnounceMessage announce(ClockIdentity{});
    context->ptpEngine->onMessage(announce);
}
