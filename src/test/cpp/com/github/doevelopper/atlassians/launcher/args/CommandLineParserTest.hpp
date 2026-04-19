/*******************************************************************
 * @file   CommandLineParserTest.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests for CommandLineParser
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_COMMANDLINEPARSERTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_COMMANDLINEPARSERTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/launcher/args/CommandLineParser.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/TypedOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/BooleanOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/MultiValueOption.hpp>
#include <com/github/doevelopper/atlassians/launcher/args/PositionalOption.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args::test
{
    /**
     * @brief Test fixture for CommandLineParser tests
     */
    class CommandLineParserTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        CommandLineParserTest();
        CommandLineParserTest(const CommandLineParserTest&) = delete;
        CommandLineParserTest(CommandLineParserTest&&) = delete;
        CommandLineParserTest& operator=(const CommandLineParserTest&) = delete;
        CommandLineParserTest& operator=(CommandLineParserTest&&) = delete;
        ~CommandLineParserTest() override;

        void SetUp() override;
        void TearDown() override;

    protected:

        /**
         * @brief Helper to create argument array from vector
         */
        auto makeArgs(const std::vector<std::string>& args) -> std::vector<const char*>;

        /**
         * @brief Helper to parse with error handling
         */
        auto parseWithArgs(CommandLineParser& parser, const std::vector<std::string>& args) -> ParseResult;

        std::unique_ptr<CommandLineParser> m_parser;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args::test

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_COMMANDLINEPARSERTEST_HPP
