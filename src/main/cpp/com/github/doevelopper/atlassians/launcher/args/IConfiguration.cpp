/*******************************************************************
 * @file   IConfiguration.cpp
 * @version  v1.0.0
 * @date     2024/12/30
 * @brief    Implementation of IConfiguration interface
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/launcher/args/IConfiguration.hpp>

using namespace com::github::doevelopper::atlassians::launcher::args;

log4cxx::LoggerPtr IConfiguration::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.launcher.args.IConfiguration"));

IConfiguration::IConfiguration() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

IConfiguration::~IConfiguration() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
