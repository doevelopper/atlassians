/*******************************************************************
 * @file   TypedOptionTest.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Unit tests implementation for option types
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/premisses/launcher/args/TypedOptionTest.hpp>

namespace com::github::doevelopper::premisses::launcher::args::test
{

using namespace com::github::doevelopper::premisses::launcher::args;

log4cxx::LoggerPtr TypedOptionTest::logger =
    log4cxx::Logger::getLogger(std::string(
        "com.github.doevelopper.premisses.launcher.args.test.TypedOptionTest"));

TypedOptionTest::TypedOptionTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

TypedOptionTest::~TypedOptionTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void TypedOptionTest::SetUp()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    m_vm.clear();
}

void TypedOptionTest::TearDown()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// StringOption Tests
// ============================================

TEST_F(TypedOptionTest, StringOption_Constructor_SetsNameAndDescription)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    StringOption opt("output", "o", "Output file path");

    EXPECT_EQ(opt.getLongName(), "output");
    EXPECT_EQ(opt.getShortName(), "o");
    EXPECT_EQ(opt.getDescription(), "Output file path");
}

TEST_F(TypedOptionTest, StringOption_GetKind_ReturnsNamed)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    StringOption opt("test", "", "Test option");

    EXPECT_EQ(opt.getKind(), OptionKind::Named);
}

TEST_F(TypedOptionTest, StringOption_WithDefaultValue_SetsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    StringOption opt("output", "o", "Output file");
    opt.withDefaultValue("default.txt");

    EXPECT_TRUE(opt.hasDefaultValue());
}

TEST_F(TypedOptionTest, StringOption_FluentAPI_ChainsCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    StringOption opt("config", "c", "Config file");
    opt.withDefaultValue("config.ini")
       .required()
       .hidden();

    auto traits = opt.getTraits();
    EXPECT_TRUE(traits.isRequired);
    EXPECT_TRUE(traits.isHidden);
}

// ============================================
// IntOption Tests
// ============================================

TEST_F(TypedOptionTest, IntOption_Constructor_SetsCorrectType)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    IntOption opt("count", "n", "Item count");

    EXPECT_EQ(opt.getLongName(), "count");
}

TEST_F(TypedOptionTest, IntOption_WithDefaultValue_SetsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    IntOption opt("port", "p", "Port number");
    opt.withDefaultValue(8080);

    EXPECT_TRUE(opt.hasDefaultValue());
}

TEST_F(TypedOptionTest, IntOption_WithImplicitValue_SetsImplicit)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    IntOption opt("verbosity", "v", "Verbosity level");
    opt.withImplicitValue(1);

    EXPECT_TRUE(opt.hasImplicitValue());
}

// ============================================
// DoubleOption Tests
// ============================================

TEST_F(TypedOptionTest, DoubleOption_WithDefaultValue_SetsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    DoubleOption opt("threshold", "t", "Threshold value");
    opt.withDefaultValue(0.5);

    EXPECT_TRUE(opt.hasDefaultValue());
}

// ============================================
// BooleanOption Tests
// ============================================

TEST_F(TypedOptionTest, BooleanOption_Constructor_CreatesSwitchOption)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    BooleanOption opt("verbose", "v", "Enable verbose mode");

    EXPECT_EQ(opt.getLongName(), "verbose");
    EXPECT_EQ(opt.getKind(), OptionKind::Boolean);
}

TEST_F(TypedOptionTest, BooleanOption_Negatable_AddsNoPrefix)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    BooleanOption opt("cache", "", "Enable caching");
    opt.negatable();

    EXPECT_TRUE(opt.isNegatable());
}

TEST_F(TypedOptionTest, BooleanOption_GetTraits_ReturnsCorrectType)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    BooleanOption opt("debug", "d", "Debug mode");

    auto traits = opt.getTraits();
    EXPECT_FALSE(traits.isRequired);  // Booleans typically not required
}

// ============================================
// MultiValueOption Tests
// ============================================

TEST_F(TypedOptionTest, MultiValueOption_Constructor_CreatesMultiValued)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    MultiValueOption<std::string> opt("include", "I", "Include paths");

    EXPECT_EQ(opt.getLongName(), "include");
    EXPECT_EQ(opt.getKind(), OptionKind::MultiValue);
}

TEST_F(TypedOptionTest, MultiValueOption_Composing_AllowsMultipleOccurrences)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    MultiValueOption<std::string> opt("lib", "L", "Library paths");
    opt.composing();

    EXPECT_TRUE(opt.isComposing());
}

TEST_F(TypedOptionTest, MultiValueOption_WithMinCount_SetsMinimum)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    MultiValueOption<std::string> opt("input", "", "Input files");
    opt.withMinCount(1);

    EXPECT_EQ(opt.getMinCount(), 1);
}

TEST_F(TypedOptionTest, MultiValueOption_WithMaxCount_SetsMaximum)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    MultiValueOption<std::string> opt("input", "", "Input files");
    opt.withMaxCount(5);

    EXPECT_EQ(opt.getMaxCount(), 5);
}

TEST_F(TypedOptionTest, MultiValueOption_WithDefaultValues_SetsDefaults)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    MultiValueOption<std::string> opt("paths", "", "Search paths");
    opt.withDefaultValues({".", ".."});

    EXPECT_TRUE(opt.hasDefaultValue());
}

// ============================================
// ImplicitOption Tests
// ============================================

TEST_F(TypedOptionTest, ImplicitOption_Constructor_SetsImplicitValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    ImplicitOption<int> opt("verbosity", "v", "Verbosity level");
    opt.withImplicitValue(1);

    EXPECT_EQ(opt.getLongName(), "verbosity");
    EXPECT_EQ(opt.getKind(), OptionKind::Implicit);
    EXPECT_TRUE(opt.hasImplicitValue());
}

TEST_F(TypedOptionTest, ImplicitOption_WithDefault_SetsBothValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    ImplicitOption<int> opt("level", "", "Level");
    opt.withImplicitValue(1).withDefaultValue(0);

    EXPECT_TRUE(opt.hasDefaultValue());
    EXPECT_TRUE(opt.hasImplicitValue());
}

// ============================================
// HiddenOption Decorator Tests
// ============================================

TEST_F(TypedOptionTest, HiddenOption_WrapsOption_SetsHiddenTrait)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto inner = std::make_shared<StringOption>("secret", "", "Secret value");
    HiddenOption hidden(inner);

    auto traits = hidden.getTraits();
    EXPECT_TRUE(traits.isHidden);
}

TEST_F(TypedOptionTest, HiddenOption_MakeHidden_CreatesHiddenWrapper)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto hidden = makeHidden<StringOption>("api-key", "", "API key");

    EXPECT_NE(hidden, nullptr);
    auto traits = hidden->getTraits();
    EXPECT_TRUE(traits.isHidden);
}

TEST_F(TypedOptionTest, HiddenOption_DelegatesLongName)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto inner = std::make_shared<IntOption>("debug-level", "D", "Debug level");
    HiddenOption hidden(inner);

    EXPECT_EQ(hidden.getLongName(), "debug-level");
    EXPECT_EQ(hidden.getShortName(), "D");
}

// ============================================
// RequiredOption Decorator Tests
// ============================================

TEST_F(TypedOptionTest, RequiredOption_WrapsOption_SetsRequiredTrait)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto inner = std::make_shared<StringOption>("config", "c", "Config file");
    RequiredOption required(inner);

    auto traits = required.getTraits();
    EXPECT_TRUE(traits.isRequired);
}

TEST_F(TypedOptionTest, RequiredOption_MakeRequired_CreatesRequiredWrapper)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto required = makeRequired<StringOption>("input", "i", "Input file");

    EXPECT_NE(required, nullptr);
    auto traits = required->getTraits();
    EXPECT_TRUE(traits.isRequired);
}

TEST_F(TypedOptionTest, RequiredOption_CheckRequired_ThrowsWhenNotSet)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto inner = std::make_shared<StringOption>("mandatory", "", "Mandatory option");
    RequiredOption required(inner);

    // With empty variables map, checkRequired should throw
    EXPECT_THROW(required.checkRequired(m_vm), RequiredOptionMissingException);
}

// ============================================
// OptionalOption Decorator Tests
// ============================================

TEST_F(TypedOptionTest, OptionalOption_WrapsOption_KeepsOptional)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto inner = std::make_shared<StringOption>("output", "o", "Output file");
    OptionalOption optional(inner);

    auto traits = optional.getTraits();
    EXPECT_FALSE(traits.isRequired);
}

TEST_F(TypedOptionTest, OptionalOption_MakeOptionalWithDefault_SetsDefault)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // Template order is <OptionT, T, Args...> with default value as first arg
    auto opt = makeOptionalWithDefault<StringOption, std::string>(
        std::string("default.txt"), "output", "o", "Output file");

    EXPECT_NE(opt, nullptr);
    EXPECT_TRUE(opt->hasDefaultValue());
}

// ============================================
// Option Traits Tests
// ============================================

TEST_F(TypedOptionTest, OptionTraits_DefaultValues_AreCorrect)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    OptionTraits traits;

    EXPECT_FALSE(traits.isRequired);
    EXPECT_FALSE(traits.isHidden);
    EXPECT_FALSE(traits.isPositional);
    EXPECT_FALSE(traits.allowsMultiple);
    EXPECT_FALSE(traits.hasImplicit);
}

TEST_F(TypedOptionTest, OptionKind_AllValuesDistinct)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    EXPECT_NE(OptionKind::Named, OptionKind::Positional);
    EXPECT_NE(OptionKind::Named, OptionKind::Boolean);
    EXPECT_NE(OptionKind::Boolean, OptionKind::MultiValue);
    EXPECT_NE(OptionKind::MultiValue, OptionKind::Implicit);
    EXPECT_NE(OptionKind::Implicit, OptionKind::ConfigFile);
}

// ============================================
// Fluent API Chain Tests
// ============================================

TEST_F(TypedOptionTest, FluentAPI_AllMethodsChain)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    IntOption opt("count", "n", "Count value");

    // All these should compile and chain correctly
    opt.withDefaultValue(0)
       .withImplicitValue(1)
       .required()
       .hidden();

    EXPECT_TRUE(opt.getTraits().isRequired);
    EXPECT_TRUE(opt.getTraits().isHidden);
    EXPECT_TRUE(opt.getTraits().hasDefault);
    EXPECT_TRUE(opt.getTraits().hasImplicit);
}

// ============================================
// Copy/Move Tests
// ============================================

TEST_F(TypedOptionTest, SharedPtr_OptionsCanBeShared)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto opt = std::make_shared<StringOption>("shared", "", "Shared option");

    auto copy = opt;
    EXPECT_EQ(opt.use_count(), 2);
    EXPECT_EQ(copy->getLongName(), "shared");
}

}  // namespace com::github::doevelopper::premisses::launcher::args::test
