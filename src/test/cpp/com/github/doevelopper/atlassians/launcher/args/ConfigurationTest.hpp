/*******************************************************************
 * @file   ConfigurationTest.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests for Configuration classes
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_CONFIGURATIONTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_CONFIGURATIONTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/atlassians/launcher/args/DefaultConfiguration.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::launcher::args::test
{
    /**
     * @brief Test fixture for Configuration classes
     */
    class ConfigurationTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        ConfigurationTest();
        ConfigurationTest(const ConfigurationTest&) = delete;
        ConfigurationTest(ConfigurationTest&&) = delete;
        ConfigurationTest& operator=(const ConfigurationTest&) = delete;
        ConfigurationTest& operator=(ConfigurationTest&&) = delete;
        ~ConfigurationTest() override;

        void SetUp() override;
        void TearDown() override;

    protected:

        /**
         * @brief Helper to create argument array
         */
        auto makeArgs(const std::vector<std::string>& args) -> std::vector<const char*>;

        std::unique_ptr<DefaultConfiguration> m_config;
    };

}  // namespace com::github::doevelopper::atlassians::launcher::args::test

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_CONFIGURATIONTEST_HPP
