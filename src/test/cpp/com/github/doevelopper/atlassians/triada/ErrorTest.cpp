/**
 * @file ErrorTest.cpp
 * @brief Unit tests for Error and Result classes implementation
 * @version 1.0.0
 * @date 2024/12/31
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 */

#include <com/github/doevelopper/premisses/triada/ErrorTest.hpp>

#include <sstream>

using namespace com::github::doevelopper::premisses::triada;
using namespace com::github::doevelopper::premisses::triada::test;

log4cxx::LoggerPtr ErrorTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.test.ErrorTest"));

log4cxx::LoggerPtr ResultTest::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.triada.test.ResultTest"));

// ============================================
// ErrorTest Implementation
// ============================================

ErrorTest::ErrorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ErrorTest::~ErrorTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ErrorTest::createTestError() -> Error
{
    return Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x100),
        "Test error message"
    );
}

auto ErrorTest::createTestErrorWithChain() -> Error
{
    auto innerError = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Storage, 0x02, 0x200),
        "Inner error"
    );
    
    auto outerError = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x100),
        "Outer error"
    );
    outerError.withCause(std::move(innerError));
    
    return outerError;
}

// ============================================
// Error Construction Tests
// ============================================

TEST_F(ErrorTest, DefaultConstructor_CreatesSuccessfulError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Error error;
    
    EXPECT_TRUE(error.code().isSuccess());
    EXPECT_TRUE(error.message().empty());
}

TEST_F(ErrorTest, Create_CreatesValidError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Network, 0x10, 0x42),
        "Network connection failed"
    );
    
    EXPECT_TRUE(error.code().isError());
    EXPECT_EQ(error.message(), "Network connection failed");
    EXPECT_EQ(error.code().service(), ServiceId::Network);
}

TEST_F(ErrorTest, CreateFromErrorCode_Works)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    ErrorCode code = ErrorCode::create(ErrorSeverity::Warning, ServiceId::IO, 0x01, 0x01);
    Error error(code, "IO Warning");
    
    EXPECT_EQ(error.code().severity(), ErrorSeverity::Warning);
    EXPECT_EQ(error.code().service(), ServiceId::IO);
}

TEST_F(ErrorTest, FromStdError_Works)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::fromStdError(
        std::make_error_code(std::errc::connection_refused),
        "Connection refused"
    );
    
    EXPECT_FALSE(error.message().empty());
}

// ============================================
// Error Chaining Tests
// ============================================

TEST_F(ErrorTest, ErrorChaining_SingleCause)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestErrorWithChain();
    
    EXPECT_FALSE(error.chain().empty());
}

TEST_F(ErrorTest, RootCause_ReturnsDeepestError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestErrorWithChain();
    const auto& root = error.rootCause();
    
    EXPECT_FALSE(root.message().empty());
}

TEST_F(ErrorTest, RootCause_ReturnsSelfWhenNoCause)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestError();
    const auto& root = error.rootCause();
    
    EXPECT_EQ(&root, &error);
}

// ============================================
// Error Formatting Tests
// ============================================

TEST_F(ErrorTest, ToString_ContainsMessage)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestError();
    std::string str = error.toString();
    
    EXPECT_NE(str.find("Test error message"), std::string::npos);
}

TEST_F(ErrorTest, ToJson_ValidJsonFormat)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestError();
    std::string json = error.toJson();
    
    EXPECT_FALSE(json.empty());
    EXPECT_NE(json.find("{"), std::string::npos);
    EXPECT_NE(json.find("}"), std::string::npos);
}

// ============================================
// Error Severity Tests
// ============================================

TEST_F(ErrorTest, IsRecoverable_FalseForFatal)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Fatal, ServiceId::Core, 0x01, 0x01),
        "Fatal error"
    );
    
    EXPECT_FALSE(error.isRecoverable());
}

TEST_F(ErrorTest, IsRecoverable_TrueForRecoverable)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Recoverable, ServiceId::Core, 0x01, 0x01),
        "Recoverable error"
    );
    
    EXPECT_TRUE(error.isRecoverable());
}

// ============================================
// Error Exception Tests
// ============================================

TEST_F(ErrorTest, ErrorException_ThrowsWithError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = createTestError();
    
    EXPECT_THROW(
        throw ErrorException(error),
        ErrorException
    );
}

TEST_F(ErrorTest, ErrorException_CatchAsStdException)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    try
    {
        throw ErrorException(createTestError());
    }
    catch (const std::exception& e)
    {
        EXPECT_NE(std::string(e.what()).find("Test error message"), std::string::npos);
    }
}

TEST_F(ErrorTest, ErrorException_PreservesErrorDetails)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    try
    {
        throw ErrorException(createTestError());
    }
    catch (const ErrorException& e)
    {
        EXPECT_EQ(e.error().message(), "Test error message");
        EXPECT_TRUE(e.error().code().isError());
    }
}

// ============================================
// Rule of Five Tests
// ============================================

TEST_F(ErrorTest, CopyConstructor_CopiesAllFields)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto original = createTestError();
    Error copy(original);
    
    EXPECT_EQ(copy.message(), original.message());
    EXPECT_EQ(copy.code().value(), original.code().value());
}

TEST_F(ErrorTest, MoveConstructor_TransfersOwnership)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto original = createTestError();
    std::string originalMessage(original.message());
    
    Error moved(std::move(original));
    
    EXPECT_EQ(moved.message(), originalMessage);
}

TEST_F(ErrorTest, CopyAssignment_CopiesAllFields)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto original = createTestError();
    Error copy;
    copy = original;
    
    EXPECT_EQ(copy.message(), original.message());
}

TEST_F(ErrorTest, MoveAssignment_TransfersOwnership)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto original = createTestError();
    std::string originalMessage(original.message());
    
    Error moved;
    moved = std::move(original);
    
    EXPECT_EQ(moved.message(), originalMessage);
}

// ============================================
// ResultTest Implementation
// ============================================

ResultTest::ResultTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

ResultTest::~ResultTest()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ResultTest::SetUp() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

auto ResultTest::TearDown() -> void
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

// ============================================
// Result Construction Tests
// ============================================

TEST_F(ResultTest, ValueConstruction_CreatesSuccessResult)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<int> result(42);
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_FALSE(result.hasError());
    EXPECT_EQ(result.value(), 42);
}

TEST_F(ResultTest, ErrorConstruction_CreatesErrorResult)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Test error"
    );
    
    Result<int> result(std::move(error));
    
    EXPECT_FALSE(result.hasValue());
    EXPECT_TRUE(result.hasError());
}

TEST_F(ResultTest, StringValueConstruction)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<std::string> result(std::string("Hello, World!"));
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value(), "Hello, World!");
}

TEST_F(ResultTest, VectorValueConstruction)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    std::vector<int> data{1, 2, 3, 4, 5};
    Result<std::vector<int>> result(data);
    
    EXPECT_TRUE(result.hasValue());
    EXPECT_EQ(result.value().size(), 5);
}

// ============================================
// Result Value Access Tests
// ============================================

TEST_F(ResultTest, Value_ReturnsValueForSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<double> result(3.14159);
    
    EXPECT_DOUBLE_EQ(result.value(), 3.14159);
}

TEST_F(ResultTest, Value_ThrowsForError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Cannot access value"
    );
    
    Result<int> result(std::move(error));
    
    EXPECT_THROW(result.value(), ErrorException);
}

TEST_F(ResultTest, ValueOr_ReturnsValueForSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<int> result(42);
    
    EXPECT_EQ(result.valueOr(0), 42);
}

TEST_F(ResultTest, ValueOr_ReturnsDefaultForError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Error occurred"
    );
    
    Result<int> result(std::move(error));
    
    EXPECT_EQ(result.valueOr(100), 100);
}

TEST_F(ResultTest, Error_ReturnsErrorForFailure)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Storage, 0x01, 0x01),
        "File not found"
    );
    
    Result<std::string> result(std::move(error));
    
    EXPECT_EQ(result.error().message(), "File not found");
}

// ============================================
// Result Monadic Operations Tests
// ============================================

TEST_F(ResultTest, Map_TransformsValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<int> result(5);
    auto mapped = result.map([](const int& x) { return x * 2; });
    
    EXPECT_TRUE(mapped.hasValue());
    EXPECT_EQ(mapped.value(), 10);
}

TEST_F(ResultTest, Map_PropagatesError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Original error"
    );
    
    Result<int> result(std::move(error));
    auto mapped = result.map([](const int& x) { return x * 2; });
    
    EXPECT_TRUE(mapped.hasError());
    EXPECT_EQ(mapped.error().message(), "Original error");
}

TEST_F(ResultTest, AndThen_ChainsOperations)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto divide = [](const int& x) -> Result<double> {
        if (x == 0)
        {
            return Error::create(
                ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
                "Division by zero"
            );
        }
        return Result<double>(10.0 / x);
    };
    
    Result<int> result(2);
    auto chained = result.andThen(divide);
    
    EXPECT_TRUE(chained.hasValue());
    EXPECT_DOUBLE_EQ(chained.value(), 5.0);
}

TEST_F(ResultTest, AndThen_PropagatesFirstError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "First error"
    );
    
    Result<int> result(std::move(error));
    auto chained = result.andThen([](const int& x) -> Result<int> { 
        return Result<int>(x * 2); 
    });
    
    EXPECT_TRUE(chained.hasError());
    EXPECT_EQ(chained.error().message(), "First error");
}

TEST_F(ResultTest, OrElse_RecoversFromError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Some error"
    );
    
    Result<int> result(std::move(error));
    auto recovered = result.orElse([](const Error&) -> Result<int> {
        return Result<int>(0);  // Default value on error
    });
    
    EXPECT_TRUE(recovered.hasValue());
    EXPECT_EQ(recovered.value(), 0);
}

// ============================================
// Status (Result<void>) Tests
// ============================================

TEST_F(ResultTest, VoidResult_Success)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<void> status;
    
    EXPECT_TRUE(status.hasValue());
    EXPECT_FALSE(status.hasError());
}

TEST_F(ResultTest, VoidResult_Error)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Operation failed"
    );
    
    Result<void> status(std::move(error));
    
    EXPECT_FALSE(status.hasValue());
    EXPECT_TRUE(status.hasError());
}

// ============================================
// Result Boolean Conversion Tests
// ============================================

TEST_F(ResultTest, BoolConversion_TrueForSuccess)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<int> result(42);
    
    EXPECT_TRUE(static_cast<bool>(result));
}

TEST_F(ResultTest, BoolConversion_FalseForError)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    auto error = Error::create(
        ErrorCode::create(ErrorSeverity::Error, ServiceId::Core, 0x01, 0x01),
        "Error"
    );
    
    Result<int> result(std::move(error));
    
    EXPECT_FALSE(static_cast<bool>(result));
}

// ============================================
// Result Move Semantics Tests
// ============================================

TEST_F(ResultTest, MoveConstructor_TransfersValue)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<std::unique_ptr<int>> result1(std::make_unique<int>(42));
    auto result2 = std::move(result1);
    
    EXPECT_TRUE(result2.hasValue());
    EXPECT_EQ(*result2.value(), 42);
}

TEST_F(ResultTest, MoveValue_TransfersOwnership)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    
    Result<std::unique_ptr<int>> result(std::make_unique<int>(100));
    auto ptr = std::move(result).value();
    
    EXPECT_EQ(*ptr, 100);
}

