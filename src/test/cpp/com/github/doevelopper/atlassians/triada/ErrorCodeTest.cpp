/**
 * @file ErrorCodeTest.cpp
 * @brief Unit tests for ErrorCode class implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/atlassians/triada/ErrorCodeTest.hpp>

#include <unordered_set>
#include <sstream>

using namespace com::github::doevelopper::atlassians::triada;
using namespace com::github::doevelopper::atlassians::triada::test;

log4cxx::LoggerPtr ErrorCodeTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.triada.test.ErrorCodeTest"));

log4cxx::LoggerPtr ErrorSeverityTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.triada.test.ErrorSeverityTest"));

log4cxx::LoggerPtr ServiceIdTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlassians.triada.test.ServiceIdTest"));

// ============================================
// ErrorCodeTest Implementation
// ============================================

ErrorCodeTest::ErrorCodeTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorCodeTest::~ErrorCodeTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorCodeTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorCodeTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorCodeTest::verifyErrorCodeFields(
    const ErrorCode& code,
    ErrorSeverity expectedSeverity,
    std::uint8_t expectedReserved,
    ServiceId expectedService,
    std::uint16_t expectedMission,
    std::uint64_t expectedCodeValue) -> void
{
    EXPECT_EQ(code.severity(), expectedSeverity);
    EXPECT_EQ(code.reserved(), expectedReserved);
    EXPECT_EQ(code.service(), expectedService);
    EXPECT_EQ(code.mission(), expectedMission);
    EXPECT_EQ(code.code(), expectedCodeValue);
}

// ============================================
// Default Constructor Tests
// ============================================

TEST_F(ErrorCodeTest, DefaultConstructor_CreatesSuccessCode)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    
    EXPECT_EQ(code.value(), 0ULL);
    EXPECT_TRUE(code.isSuccess());
    EXPECT_FALSE(code.isError());
    EXPECT_FALSE(code.isFatal());
}

TEST_F(ErrorCodeTest, DefaultConstructor_IsConstexpr)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    constexpr ErrorCode code;
    
    static_assert(code.value() == 0ULL, "Default ErrorCode should have value 0");
    EXPECT_EQ(code.value(), 0ULL);
}

// ============================================
// Value Constructor Tests
// ============================================

TEST_F(ErrorCodeTest, ValueConstructor_StoresRawValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    constexpr std::uint64_t testValue = 0x123456789ABCDEF0ULL;
    ErrorCode code(testValue);
    
    EXPECT_EQ(code.value(), testValue);
}

TEST_F(ErrorCodeTest, ValueConstructor_MaxValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    constexpr std::uint64_t maxValue = std::numeric_limits<std::uint64_t>::max();
    ErrorCode code(maxValue);
    
    EXPECT_EQ(code.value(), maxValue);
}

// ============================================
// Create Factory Method Tests
// ============================================

TEST_F(ErrorCodeTest, Create_EncodesCorrectly)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(
        ErrorSeverity::Error,
        ServiceId::Core,
        0x100,
        0xABCD12345ULL,
        0x05
    );
    
    verifyErrorCodeFields(
        code,
        ErrorSeverity::Error,
        0x05,
        ServiceId::Core,
        0x100,
        0xABCD12345ULL
    );
}

TEST_F(ErrorCodeTest, Create_HandlesZeroValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(
        ErrorSeverity::Trace,
        ServiceId::Core,
        0x000,
        0x00ULL,
        0x00
    );
    
    verifyErrorCodeFields(
        code,
        ErrorSeverity::Trace,
        0x00,
        ServiceId::Core,
        0x000,
        0x00ULL
    );
}

TEST_F(ErrorCodeTest, Create_HandlesMaxFieldValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(
        ErrorSeverity::Fatal,
        static_cast<ServiceId>(0x1F),
        0x0FFF,
        0xFFFFFFFFFFULL,
        0x0F
    );
    
    EXPECT_EQ(code.severity(), ErrorSeverity::Fatal);
    EXPECT_EQ(code.reserved(), 0x0F);
    EXPECT_EQ(code.mission(), 0x0FFF);
    EXPECT_EQ(code.code(), 0xFFFFFFFFFFULL);
}

// ============================================
// Severity Level Tests
// ============================================

TEST_F(ErrorCodeTest, Severity_Trace_IsNotError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Trace, ServiceId::Core, 0, 1);
    
    EXPECT_FALSE(code.isError());
    EXPECT_FALSE(code.isFatal());
    EXPECT_FALSE(code.isWarning());
}

TEST_F(ErrorCodeTest, Severity_Debug_IsNotError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Debug, ServiceId::Core, 0, 1);
    
    EXPECT_FALSE(code.isError());
}

TEST_F(ErrorCodeTest, Severity_Warning_IsWarning)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Warning, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(code.isWarning());
    EXPECT_FALSE(code.isError());
    EXPECT_FALSE(code.isFatal());
}

TEST_F(ErrorCodeTest, Severity_Error_IsError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(code.isError());
    EXPECT_FALSE(code.isFatal());
}

TEST_F(ErrorCodeTest, Severity_Critical_IsError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Critical, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(code.isError());
    EXPECT_TRUE(code.isCritical());
}

TEST_F(ErrorCodeTest, Severity_Fatal_IsFatal)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Fatal, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(code.isFatal());
    EXPECT_TRUE(code.isError());
}

// ============================================
// Comparison Operator Tests
// ============================================

TEST_F(ErrorCodeTest, EqualityOperator_SameValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code1 = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x100, 0x42);
    auto code2 = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x100, 0x42);
    
    EXPECT_TRUE(code1 == code2);
    EXPECT_FALSE(code1 != code2);
}

TEST_F(ErrorCodeTest, EqualityOperator_DifferentValues)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code1 = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x100, 0x42);
    auto code2 = ErrorCode::create(ErrorSeverity::Warning, ServiceId::Core, 0x100, 0x42);
    
    EXPECT_FALSE(code1 == code2);
    EXPECT_TRUE(code1 != code2);
}

TEST_F(ErrorCodeTest, SpaceshipOperator_Ordering)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code1(0x100);
    ErrorCode code2(0x200);
    
    EXPECT_TRUE(code1 < code2);
    EXPECT_TRUE(code2 > code1);
    EXPECT_TRUE(code1 <= code2);
    EXPECT_TRUE(code2 >= code1);
}

// ============================================
// Boolean Conversion Tests
// ============================================

TEST_F(ErrorCodeTest, BoolConversion_TrueForNonZero)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto errorCode = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(static_cast<bool>(errorCode));
}

TEST_F(ErrorCodeTest, BoolConversion_FalseForZero)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode successCode;
    
    EXPECT_FALSE(static_cast<bool>(successCode));
}

// ============================================
// Hash Support Tests
// ============================================

TEST_F(ErrorCodeTest, Hash_SameValuesSameHash)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code1(0x12345678);
    ErrorCode code2(0x12345678);
    
    std::hash<ErrorCode> hasher;
    EXPECT_EQ(hasher(code1), hasher(code2));
}

TEST_F(ErrorCodeTest, Hash_WorksInUnorderedSet)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    std::unordered_set<ErrorCode> errorSet;
    
    ErrorCode code1(0x100);
    ErrorCode code2(0x200);
    ErrorCode code3(0x100);  // Same as code1
    
    errorSet.insert(code1);
    errorSet.insert(code2);
    errorSet.insert(code3);
    
    EXPECT_EQ(errorSet.size(), 2);
}

// ============================================
// String Conversion Tests
// ============================================

TEST_F(ErrorCodeTest, ToString_NotEmpty)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x42, 0x12345);
    std::string str = code.toString();
    
    EXPECT_FALSE(str.empty());
}

TEST_F(ErrorCodeTest, ToHexString_NotEmpty)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code(0xDEADBEEF);
    std::string hexStr = code.toHexString();
    
    EXPECT_FALSE(hexStr.empty());
}

// ============================================
// Factory Method Tests
// ============================================

TEST_F(ErrorCodeTest, Success_CreatesSuccessCode)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::success();
    
    EXPECT_TRUE(code.isSuccess());
    EXPECT_EQ(code.value(), 0ULL);
}

// ============================================
// Constexpr Validation Tests
// ============================================

TEST_F(ErrorCodeTest, AllOperations_AreConstexpr)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    constexpr auto code = ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0, 1);
    constexpr auto severity = code.severity();
    constexpr auto value = code.value();
    constexpr bool isErr = code.isError();
    
    static_assert(severity == ErrorSeverity::Error);
    static_assert(value != 0);
    static_assert(isErr);
    
    SUCCEED();
}

// ============================================
// Modifier Chain Tests
// ============================================

TEST_F(ErrorCodeTest, WithSeverity_ModifiesSeverity)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    code.withSeverity(ErrorSeverity::Error);
    
    EXPECT_EQ(code.severity(), ErrorSeverity::Error);
}

TEST_F(ErrorCodeTest, WithService_ModifiesService)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    code.withService(ServiceId::Network);
    
    EXPECT_EQ(code.service(), ServiceId::Network);
}

TEST_F(ErrorCodeTest, WithMission_ModifiesMission)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    code.withMission(0x123);
    
    EXPECT_EQ(code.mission(), 0x123);
}

TEST_F(ErrorCodeTest, WithCode_ModifiesCode)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    code.withCode(0xABCDEF);
    
    EXPECT_EQ(code.code(), 0xABCDEF);
}

TEST_F(ErrorCodeTest, ModifierChaining_Works)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code;
    code.withSeverity(ErrorSeverity::Warning)
        .withService(ServiceId::IO)
        .withMission(0x42)
        .withCode(0x100);
    
    EXPECT_EQ(code.severity(), ErrorSeverity::Warning);
    EXPECT_EQ(code.service(), ServiceId::IO);
    EXPECT_EQ(code.mission(), 0x42);
    EXPECT_EQ(code.code(), 0x100);
}

// ============================================
// Bit Field Boundary Tests
// ============================================

TEST_F(ErrorCodeTest, BitField_SeverityMask_ThreeBits)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    for (int i = 0; i <= 7; ++i)
    {
        auto code = ErrorCode::create(static_cast<ErrorSeverity>(i), ServiceId::Core, 0, 0);
        EXPECT_EQ(static_cast<int>(code.severity()), i);
    }
}

TEST_F(ErrorCodeTest, BitField_MissionMask_TwelveBits)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto codeMin = ErrorCode::create(ErrorSeverity::Trace, ServiceId::Core, 0x000, 0);
    auto codeMax = ErrorCode::create(ErrorSeverity::Trace, ServiceId::Core, 0xFFF, 0);
    
    EXPECT_EQ(codeMin.mission(), 0x000);
    EXPECT_EQ(codeMax.mission(), 0xFFF);
}

TEST_F(ErrorCodeTest, BitField_CodeMask_FortyBits)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    constexpr std::uint64_t maxCode = 0xFFFFFFFFFFULL;
    
    auto code = ErrorCode::create(ErrorSeverity::Trace, ServiceId::Core, 0, maxCode);
    EXPECT_EQ(code.code(), maxCode);
}

// ============================================
// Recoverable Status Tests
// ============================================

TEST_F(ErrorCodeTest, IsRecoverable_TrueForRecoverableErrors)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0, 1);
    
    EXPECT_TRUE(code.isRecoverable());
    EXPECT_TRUE(code.isError());
}

TEST_F(ErrorCodeTest, IsRecoverable_FalseForFatal)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto code = ErrorCode::create(ErrorSeverity::Fatal, ServiceId::Core, 0, 1);
    
    EXPECT_FALSE(code.isRecoverable());
    EXPECT_TRUE(code.isFatal());
}

