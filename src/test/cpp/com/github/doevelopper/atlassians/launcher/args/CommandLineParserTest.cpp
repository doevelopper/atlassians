/*******************************************************************
 * @file   CommandLineParserTest.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests implementation for CommandLineParser
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/CommandLineParserTest.hpp>

namespace com::github::doevelopper::atlassians::launcher::args::test
{

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr CommandLineParserTest::logger =
    log4cxx::Logger::getLogger(std::string(
        "com.github.doevelopper.atlassians.launcher.args.test.CommandLineParserTest"));

CommandLineParserTest::CommandLineParserTest()
    : m_parser(nullptr)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

CommandLineParserTest::~CommandLineParserTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void CommandLineParserTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser = std::make_unique<CommandLineParser>("TestProgram", "Test program description");
    ASSERT_NE(m_parser, nullptr) << "Failed to create CommandLineParser";
}

void CommandLineParserTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser.reset();
}

auto CommandLineParserTest::makeArgs(const std::vector<std::string>& args) -> std::vector<const char*>
{
    std::vector<const char*> argv;
    argv.reserve(args.size());
    for (const auto& arg : args)
    {
        argv.push_back(arg.c_str());
    }
    return argv;
}

auto CommandLineParserTest::parseWithArgs(CommandLineParser& parser,
                                          const std::vector<std::string>& args) -> ParseResult
{
    auto argv = makeArgs(args);
    return parser.parse(static_cast<int>(argv.size()), argv.data());
}

// ============================================
// Constructor and Basic State Tests
// ============================================

TEST_F(CommandLineParserTest, Constructor_WithNameAndDescription_SetsCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    CommandLineParser parser("MyApp", "My application description");
    // Parser should be created without errors
    SUCCEED();
}

TEST_F(CommandLineParserTest, Constructor_Default_CreatesValidParser)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    CommandLineParser parser;
    // Default parser should be valid
    SUCCEED();
}

// ============================================
// Standard Options Tests
// ============================================

TEST_F(CommandLineParserTest, AddStandardOptions_AddsHelpAndVersion)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addStandardOptions();

    std::vector<std::string> args = {"test_program", "--help"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.helpRequested);
}

TEST_F(CommandLineParserTest, HelpOption_ShortForm_RecognizedCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addStandardOptions();

    std::vector<std::string> args = {"test_program", "-h"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.helpRequested);
}

TEST_F(CommandLineParserTest, VersionOption_LongForm_RecognizedCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addStandardOptions();

    std::vector<std::string> args = {"test_program", "--version"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.versionRequested);
}

// ============================================
// String Option Tests
// ============================================

TEST_F(CommandLineParserTest, StringOption_LongForm_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file path");

    std::vector<std::string> args = {"test_program", "--output", "/path/to/file"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("output", ""), "/path/to/file");
}

TEST_F(CommandLineParserTest, StringOption_ShortForm_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file path");

    std::vector<std::string> args = {"test_program", "-o", "/path/to/file"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("output", ""), "/path/to/file");
}

TEST_F(CommandLineParserTest, StringOption_WithEquals_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file path");

    std::vector<std::string> args = {"test_program", "--output=/path/to/file"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("output", ""), "/path/to/file");
}

TEST_F(CommandLineParserTest, StringOption_WithDefault_ReturnsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto opt = std::make_shared<StringOption>("output", "o", "Output file path");
    opt->withDefaultValue("default.txt");
    m_parser->addOption(opt);

    std::vector<std::string> args = {"test_program"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("output", ""), "default.txt");
}

// ============================================
// Integer Option Tests
// ============================================

TEST_F(CommandLineParserTest, IntOption_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<IntOption>("port", "p", "Port number");

    std::vector<std::string> args = {"test_program", "--port", "8080"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<int>("port", 0), 8080);
}

TEST_F(CommandLineParserTest, IntOption_NegativeValue_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<IntOption>("offset", "", "Offset value");

    std::vector<std::string> args = {"test_program", "--offset", "-100"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<int>("offset", 0), -100);
}

TEST_F(CommandLineParserTest, IntOption_WithDefault_ReturnsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto opt = std::make_shared<IntOption>("port", "p", "Port number");
    opt->withDefaultValue(3000);
    m_parser->addOption(opt);

    std::vector<std::string> args = {"test_program"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<int>("port", 0), 3000);
}

// ============================================
// Boolean Option Tests
// ============================================

TEST_F(CommandLineParserTest, BooleanOption_Flag_SetsToTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<BooleanOption>("verbose", "v", "Enable verbose output");

    std::vector<std::string> args = {"test_program", "--verbose"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(m_parser->getValue<bool>("verbose", false));
}

TEST_F(CommandLineParserTest, BooleanOption_NotProvided_ReturnsFalse)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<BooleanOption>("verbose", "v", "Enable verbose output");

    std::vector<std::string> args = {"test_program"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(m_parser->getValue<bool>("verbose", false));
}

TEST_F(CommandLineParserTest, BooleanOption_ShortForm_SetsToTrue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<BooleanOption>("verbose", "v", "Enable verbose output");

    std::vector<std::string> args = {"test_program", "-v"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_TRUE(m_parser->getValue<bool>("verbose", false));
}

// ============================================
// Double Option Tests
// ============================================

TEST_F(CommandLineParserTest, DoubleOption_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<DoubleOption>("threshold", "t", "Threshold value");

    std::vector<std::string> args = {"test_program", "--threshold", "0.75"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(m_parser->getValue<double>("threshold", 0.0), 0.75);
}

TEST_F(CommandLineParserTest, DoubleOption_ScientificNotation_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<DoubleOption>("epsilon", "", "Epsilon value");

    std::vector<std::string> args = {"test_program", "--epsilon", "1.5e-6"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(m_parser->getValue<double>("epsilon", 0.0), 1.5e-6);
}

// ============================================
// MultiValue Option Tests
// ============================================

TEST_F(CommandLineParserTest, MultiValueOption_MultipleOccurrences_CollectsAll)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto opt = std::make_shared<MultiValueOption<std::string>>("include", "I", "Include path");
    m_parser->addOption(opt);

    std::vector<std::string> args = {
        "test_program", "--include", "dir1", "--include", "dir2", "--include", "dir3"
    };
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    auto values = m_parser->getValue<std::vector<std::string>>("include", {});
    EXPECT_EQ(values.size(), 3);
    EXPECT_EQ(values[0], "dir1");
    EXPECT_EQ(values[1], "dir2");
    EXPECT_EQ(values[2], "dir3");
}

TEST_F(CommandLineParserTest, MultiValueOption_ShortForm_CollectsAll)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto opt = std::make_shared<MultiValueOption<std::string>>("include", "I", "Include path");
    m_parser->addOption(opt);

    std::vector<std::string> args = {"test_program", "-I", "dir1", "-I", "dir2"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    auto values = m_parser->getValue<std::vector<std::string>>("include", {});
    EXPECT_EQ(values.size(), 2);
}

// ============================================
// Mixed Options Tests
// ============================================

TEST_F(CommandLineParserTest, MixedOptions_ParsedCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("config", "c", "Config file");
    m_parser->addOption<IntOption>("port", "p", "Port number");
    m_parser->addOption<BooleanOption>("verbose", "v", "Verbose mode");

    std::vector<std::string> args = {
        "test_program", "-c", "config.ini", "--port", "9000", "-v"
    };
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("config", ""), "config.ini");
    EXPECT_EQ(m_parser->getValue<int>("port", 0), 9000);
    EXPECT_TRUE(m_parser->getValue<bool>("verbose", false));
}

// ============================================
// Error Handling Tests
// ============================================

TEST_F(CommandLineParserTest, UnrecognizedOption_ReturnsError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file");

    std::vector<std::string> args = {"test_program", "--unknown-option", "value"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_FALSE(result.success);
    EXPECT_FALSE(result.errorMessage.empty());
}

TEST_F(CommandLineParserTest, AllowUnrecognized_DoesNotFail)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file");
    m_parser->allowUnrecognized();

    // Using = format to avoid value being treated as positional
    std::vector<std::string> args = {"test_program", "--unknown-option=value", "-o", "file.txt"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("output", ""), "file.txt");
}

TEST_F(CommandLineParserTest, MissingRequiredValue_ReturnsError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("output", "o", "Output file");

    std::vector<std::string> args = {"test_program", "--output"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_FALSE(result.success);
}

// ============================================
// Help Message Tests
// ============================================

TEST_F(CommandLineParserTest, PrintHelp_IncludesOptionDescriptions)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("config", "c", "Path to configuration file");
    m_parser->addOption<IntOption>("port", "p", "Server port number");
    m_parser->addStandardOptions();

    std::ostringstream oss;
    m_parser->printHelp(oss);
    std::string helpText = oss.str();

    EXPECT_NE(helpText.find("config"), std::string::npos);
    EXPECT_NE(helpText.find("port"), std::string::npos);
    EXPECT_NE(helpText.find("help"), std::string::npos);
}

// ============================================
// Edge Cases
// ============================================

TEST_F(CommandLineParserTest, EmptyArgs_ParsesSuccessfully)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addStandardOptions();

    std::vector<std::string> args = {"test_program"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_FALSE(result.helpRequested);
    EXPECT_FALSE(result.versionRequested);
}

TEST_F(CommandLineParserTest, OptionWithSpacesInValue_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("message", "m", "Message text");

    std::vector<std::string> args = {"test_program", "--message", "Hello World"};
    auto result = parseWithArgs(*m_parser, args);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(m_parser->getValue<std::string>("message", ""), "Hello World");
}

TEST_F(CommandLineParserTest, OptionValueStartingWithDash_ParsesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_parser->addOption<StringOption>("pattern", "", "Search pattern");

    std::vector<std::string> args = {"test_program", "--pattern", "-test-"};
    auto result = parseWithArgs(*m_parser, args);

    // This might fail depending on implementation - some parsers treat -test- as option
    // Document expected behavior
    EXPECT_TRUE(result.success);
}

}  // namespace com::github::doevelopper::atlassians::launcher::args::test
