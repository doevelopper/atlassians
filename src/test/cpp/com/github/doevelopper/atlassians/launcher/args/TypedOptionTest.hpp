/*******************************************************************
 * @file   TypedOptionTest.hpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests for TypedOption and concrete option types
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_TYPEDOPTIONTEST_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_TYPEDOPTIONTEST_HPP

#include <gtest/gtest.h>

#include <com/github/doevelopper/premisses/launcher/args/TypedOption.hpp>
#include <com/github/doevelopper/premisses/launcher/args/BooleanOption.hpp>
#include <com/github/doevelopper/premisses/launcher/args/MultiValueOption.hpp>
#include <com/github/doevelopper/premisses/launcher/args/ImplicitOption.hpp>
#include <com/github/doevelopper/premisses/launcher/args/HiddenOption.hpp>
#include <com/github/doevelopper/premisses/launcher/args/Required.hpp>
#include <com/github/doevelopper/premisses/launcher/args/Optional.hpp>
#include <com/github/doevelopper/premisses/logging/LogMacros.hpp>

namespace com::github::doevelopper::premisses::launcher::args::test
{
    /**
     * @brief Test fixture for option type tests
     */
    class TypedOptionTest : public ::testing::Test
    {
        LOG4CXX_DECLARE_STATIC_TEST_LOGGER

    public:

        TypedOptionTest();
        TypedOptionTest(const TypedOptionTest&) = delete;
        TypedOptionTest(TypedOptionTest&&) = delete;
        TypedOptionTest& operator=(const TypedOptionTest&) = delete;
        TypedOptionTest& operator=(TypedOptionTest&&) = delete;
        ~TypedOptionTest() override;

        void SetUp() override;
        void TearDown() override;

    protected:

        boost::program_options::variables_map m_vm;
    };

}  // namespace com::github::doevelopper::premisses::launcher::args::test

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LAUNCHER_ARGS_TEST_TYPEDOPTIONTEST_HPP
