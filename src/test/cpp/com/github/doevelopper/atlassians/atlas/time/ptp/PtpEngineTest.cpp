#include <com/github/doevelopper/premisses/atlas/time/ptp/PtpEngineTest.hpp>

using namespace com::github::doevelopper::premisses::atlas::time::ptp;
using namespace com::github::doevelopper::premisses::atlas::time::ptp::test;

void PtpEngineTest::SetUp()
{
    PtpEngineBuilder builder;
    engine = builder.build();
    ASSERT_NE(engine, nullptr);
}

TEST_F(PtpEngineTest, TimestampSerializeRoundTrip)
{
    const PtpTimestamp original(1234567U, 42U);
    const auto encoded = original.serialize();
    const auto decoded = PtpTimestamp::deserialize(encoded);

    EXPECT_EQ(decoded, original);
}

TEST_F(PtpEngineTest, BmcaSelectsLowestPriorityClock)
{
    const std::vector<ClockIdentity> clocks{
        ClockIdentity{200U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-z"},
        ClockIdentity{120U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-a"},
        ClockIdentity{130U, 248U, 0xFEU, 0xFFFFU, 128U, "clock-b"}};

    const auto selected = engine->electGrandmaster(clocks);

    ASSERT_TRUE(selected.has_value());
    EXPECT_EQ(selected->clockId, "clock-a");
}

TEST_F(PtpEngineTest, EngineTransitionsToListeningWhenStarted)
{
    engine->start();

    EXPECT_EQ(engine->currentStateName(), "Listening");
}

TEST_F(PtpEngineTest, EngineTransitionsToMasterOnAnnounce)
{
    engine->start();
    const AnnounceMessage announce(ClockIdentity{});

    engine->onMessage(announce);

    EXPECT_EQ(engine->currentStateName(), "Master");
}

TEST_F(PtpEngineTest, EngineTransitionsToFaultyWhenStopped)
{
    engine->start();
    engine->stop();

    EXPECT_EQ(engine->currentStateName(), "Faulty");
}
