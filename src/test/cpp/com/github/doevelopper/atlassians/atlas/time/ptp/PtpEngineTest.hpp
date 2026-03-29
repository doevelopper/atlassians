#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINETEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINETEST_HPP

#include <gtest/gtest.h>

#include <memory>

#include <com/github/doevelopper/premisses/atlas/time/ptp/PtpEngine.hpp>

namespace com::github::doevelopper::premisses::atlas::time::ptp::test
{
class PtpEngineTest : public ::testing::Test
{
public:
    void SetUp() override;

protected:
    std::unique_ptr<PtpEngine> engine;
};
} // namespace com::github::doevelopper::premisses::atlas::time::ptp::test

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_PTP_PTPENGINETEST_HPP
