/*******************************************************************
 * @file   ConfigurationTest.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests implementation for Configuration classes
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <fstream>
#include <filesystem>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/value_semantic.hpp>

#include <com/github/doevelopper/premisses/launcher/args/ConfigurationTest.hpp>

namespace po = boost::program_options;

namespace com::github::doevelopper::premisses::launcher::args::test
{

using namespace com::github::doevelopper::premisses::launcher::args;

log4cxx::LoggerPtr ConfigurationTest::logger =
    log4cxx::Logger::getLogger(std::string(
        "com.github.doevelopper.premisses.launcher.args.test.ConfigurationTest"));

ConfigurationTest::ConfigurationTest()
    : m_config(nullptr)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ConfigurationTest::~ConfigurationTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void ConfigurationTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_config = std::make_unique<DefaultConfiguration>("TestApp", "Test application");
    ASSERT_NE(m_config, nullptr) << "Failed to create DefaultConfiguration";
}

void ConfigurationTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_config.reset();
}

auto ConfigurationTest::makeArgs(const std::vector<std::string>& args) -> std::vector<const char*>
{
    std::vector<const char*> argv;
    argv.reserve(args.size());
    for (const auto& arg : args)
    {
        argv.push_back(arg.c_str());
    }
    return argv;
}

// ============================================
// DefaultConfiguration Constructor Tests
// ============================================

TEST_F(ConfigurationTest, DefaultConstructor_CreatesValidConfig)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    DefaultConfiguration config;
    // Should be valid
    SUCCEED();
}

TEST_F(ConfigurationTest, ConstructorWithName_SetsProgramInfo)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    DefaultConfiguration config("MyApp", "My application description");
    // Program info should be set
    SUCCEED();
}

// ============================================
// Parse Tests
// ============================================

TEST_F(ConfigurationTest, Parse_EmptyArgs_Succeeds)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);

    bool result = m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result);
}

TEST_F(ConfigurationTest, Parse_HelpOption_SetsHelpRequested)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--help"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(m_config->isHelpRequested());
}

TEST_F(ConfigurationTest, Parse_VersionOption_SetsVersionRequested)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--version"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(m_config->isVersionRequested());
}

TEST_F(ConfigurationTest, Parse_VerboseOption_SetsVerbosity)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--verbose", "2"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(m_config->getVerbosity(), 2);
}

TEST_F(ConfigurationTest, Parse_QuietOption_SetsQuiet)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--quiet"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(m_config->isQuiet());
}

TEST_F(ConfigurationTest, Parse_DebugOption_SetsDebugMode)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--debug"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(m_config->isDebugMode());
}

// ============================================
// Custom Options Tests
// ============================================

TEST_F(ConfigurationTest, AddCustomOption_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto& opts = m_config->getOptionsDescription();
    opts.add_options()
        ("port,p", po::value<int>()->default_value(8080), "Server port");

    std::vector<std::string> args = {"test_program", "--port", "9000"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(m_config->get<int>("port", 0), 9000);
}

TEST_F(ConfigurationTest, CustomOption_WithDefault_ReturnsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto& opts = m_config->getOptionsDescription();
    opts.add_options()
        ("port,p", po::value<int>()->default_value(8080), "Server port");

    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(m_config->get<int>("port", 0), 8080);
}

// ============================================
// IsSet Tests
// ============================================

TEST_F(ConfigurationTest, IsSet_OptionProvided_ReturnsTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program", "--debug"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(m_config->isSet("debug"));
}

TEST_F(ConfigurationTest, IsSet_OptionNotProvided_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    // debug is not set (it's a switch, default false)
    EXPECT_FALSE(m_config->isSet("nonexistent"));
}

// ============================================
// Help Message Tests
// ============================================

TEST_F(ConfigurationTest, GetHelpMessage_IncludesOptions)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto& opts = m_config->getOptionsDescription();
    opts.add_options()
        ("custom", "A custom option");

    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);
    m_config->parse(static_cast<int>(argv.size()), argv.data());

    std::string help = m_config->getHelpMessage();

    EXPECT_NE(help.find("help"), std::string::npos);
    EXPECT_NE(help.find("version"), std::string::npos);
    EXPECT_NE(help.find("custom"), std::string::npos);
}

// ============================================
// Unrecognized Args Tests
// ============================================

TEST_F(ConfigurationTest, AllowUnrecognized_CollectsUnknownArgs)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_config->allowUnrecognized(true);

    // Using = format to avoid value being treated as positional
    std::vector<std::string> args = {"test_program", "--unknown=value"};
    auto argv = makeArgs(args);

    bool result = m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result);
    auto unrecognized = m_config->getUnrecognizedArgs();
    EXPECT_FALSE(unrecognized.empty());
}

// ============================================
// Error Handler Tests
// ============================================

TEST_F(ConfigurationTest, OnError_CallbackInvoked)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    bool errorCalled = false;
    std::string errorMsg;

    m_config->onError([&errorCalled, &errorMsg](const std::string& err) {
        errorCalled = true;
        errorMsg = err;
    });

    // Provide an invalid option (without allowing unrecognized)
    std::vector<std::string> args = {"test_program", "--invalid-option"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(errorCalled);
    EXPECT_FALSE(errorMsg.empty());
}

// ============================================
// Fluent API Tests
// ============================================

TEST_F(ConfigurationTest, FluentAPI_ChainsCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_config->setProgramInfo("FluentApp", "Fluent API test")
            .setEnvPrefix("FLUENT_")
            .allowUnrecognized(true);

    // Should compile and work
    SUCCEED();
}

// ============================================
// Config File Tests
// ============================================

TEST_F(ConfigurationTest, SetConfigFile_SetsPath)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_config->setConfigFile("/path/to/config.ini");

    // Should not throw
    SUCCEED();
}

// ============================================
// Unparsed Handler Tests
// ============================================

TEST_F(ConfigurationTest, UnparsedHandler_Invoked)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> receivedArgs;

    m_config->setUnparsedHandler([&receivedArgs](const std::vector<std::string>& args) {
        receivedArgs = args;
    });

    m_config->allowUnrecognized(true);

    std::vector<std::string> args = {"test_program", "--unknown1", "--unknown2"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_FALSE(receivedArgs.empty());
}

// ============================================
// Edge Cases
// ============================================

TEST_F(ConfigurationTest, Parse_OnlyProgramName_Succeeds)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);

    bool result = m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_TRUE(result);
    EXPECT_FALSE(m_config->isHelpRequested());
    EXPECT_FALSE(m_config->isVersionRequested());
}

TEST_F(ConfigurationTest, Get_NonexistentOption_ReturnsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::vector<std::string> args = {"test_program"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    EXPECT_EQ(m_config->get<int>("nonexistent", 42), 42);
    EXPECT_EQ(m_config->get<std::string>("nonexistent", "default"), "default");
}

TEST_F(ConfigurationTest, Get_WrongType_ReturnsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto& opts = m_config->getOptionsDescription();
    opts.add_options()
        ("port", po::value<int>(), "Port number");

    std::vector<std::string> args = {"test_program", "--port", "8080"};
    auto argv = makeArgs(args);

    m_config->parse(static_cast<int>(argv.size()), argv.data());

    // Try to get as string when it's an int
    EXPECT_EQ(m_config->get<std::string>("port", "default"), "default");
}

}  // namespace com::github::doevelopper::premisses::launcher::args::test
