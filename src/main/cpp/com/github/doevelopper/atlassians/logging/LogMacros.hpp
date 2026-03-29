/*******************************************************************
 * @file   LogMacros.hpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Legacy logging macros for backward compatibility
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGMACROS_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGMACROS_HPP

#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

#include <log4cxx/logger.h>
#include <com/github/doevelopper/atlassians/API_Export.hpp>

// Optional: Include boost for type demangling if available
#if __has_include(<boost/core/demangle.hpp>)
    #include <boost/core/demangle.hpp>
    #define HAS_BOOST_DEMANGLE 1
#endif

#if __has_include(<boost/type_index.hpp>)
    #include <boost/type_index.hpp>
    #define HAS_BOOST_TYPEINDEX 1
#endif

/**
 * @ingroup LoggingService
 * @brief Define static logger as private member.
 * @attention May change the scope of attributes to private if not defined in private section.
 */
#define LOG4CXX_DECLARE_STATIC_LOGGER \
private: \
    static log4cxx::LoggerPtr logger;

/**
 * @ingroup LoggingService
 * @brief Define static logger as protected member.
 * @attention May change the scope of attributes to protected if not defined in protected section.
 */
#define LOG4CXX_DECLARE_STATIC_TEST_LOGGER \
protected: \
    static log4cxx::LoggerPtr logger;

/**
 * @ingroup LoggingService
 * @brief Define logger as private member.
 * @param name Name of logger to be used within the class
 */
#define LOG4CXX_DECLARE_CLASS_LOGGER(name) \
private: \
    log4cxx::LoggerPtr name;

/**
 * @brief Get the root logger
 */
#define DEFAULT_LOGGER() \
    log4cxx::Logger::getRootLogger()

/**
 * @brief Fallback logging macro to console appender.
 * @param stream Standard stream to log to.
 * @return Given @a stream.
 * @example LOG_CA(std::cout) << "streaming to standard output." << std::endl;
 *          LOG_CA(std::cerr) << "streaming to standard error."  << std::endl;
 */
#define LOG_CA(stream) \
    stream << "(" \
           << "processID " \
           << ") " \
           << "[" << logger << "::" << __func__ << "] "

/**
 * @brief Fallback logging macro for function name to console.
 * @param stream Standard stream to log to.
 */
#define LOG_CA_FUNC(stream) \
    stream << __PRETTY_FUNCTION__ << std::endl;

/**
 * @ingroup LoggingService
 * @brief Define logger using demangled type id.
 * @param i Type instance to get logger name from.
 */
#if defined(HAS_BOOST_DEMANGLE)
    #define LOG4CXX_DEFINE_CLASS_LOGGER_BY_ID(i) \
        (boost::core::demangle(typeid(i).name()))
#else
    #define LOG4CXX_DEFINE_CLASS_LOGGER_BY_ID(i) \
        (typeid(i).name())
#endif

/**
 * @ingroup LoggingService
 * @brief Create logger with demangled class name.
 * @param name Type name to demangle.
 */
#if defined(HAS_BOOST_DEMANGLE)
    #define LOG4CXX_DEFINE_CLASS_LOGGER(name) \
        log4cxx::Logger::getLogger(std::string(boost::core::demangle(name)));
#else
    #define LOG4CXX_DEFINE_CLASS_LOGGER(name) \
        log4cxx::Logger::getLogger(std::string(name));
#endif

/**
 * @ingroup LoggingService
 * @brief Define logger using boost::typeindex for pretty name.
 * Uses the runtime type information to get a readable class name.
 */
#if defined(HAS_BOOST_TYPEINDEX)
    #define DEFINE_LOGGER \
        std::string logger {boost::typeindex::type_id<decltype(*this)>().pretty_name()};
#else
    #define DEFINE_LOGGER \
        std::string logger {typeid(*this).name()};
#endif

/**
 * @ingroup LoggingService
 * @brief Macro for tracing entry into a function.
 * @param x Parameters to log.
 */
#define LOG_FUNCTION(x) LOG4CXX_TRACE(logger, __FUNCTION__ << "(" << x << ")");

/**
 * @ingroup LoggingService
 * @brief Macro for tracing exit from a function.
 * @param x Return value or parameters to log.
 */
#define LOG_EXIT_FUNCTION(x) LOG4CXX_TRACE(logger, __FUNCTION__ << " exit(" << x << ")");

/**
 * @ingroup LoggingService
 * @brief Macro for tracing both entry and exit of a function using RAII.
 * Creates a scope guard that logs on construction and destruction.
 */
#define LOG_FUNCTION_SCOPE() \
    struct LogFunctionScopeGuard_ { \
        const char* fn_; \
        log4cxx::LoggerPtr& log_; \
        LogFunctionScopeGuard_(const char* fn, log4cxx::LoggerPtr& log) : fn_(fn), log_(log) { \
            LOG4CXX_TRACE(log_, fn_ << " ENTER"); \
        } \
        ~LogFunctionScopeGuard_() { \
            LOG4CXX_TRACE(log_, fn_ << " EXIT"); \
        } \
    } logFunctionScopeGuard_(__FUNCTION__, logger);

#endif  // COM_GITHUB_DOEVELOPPER_PREMISSES_LOGGING_LOGMACROS_HPP
