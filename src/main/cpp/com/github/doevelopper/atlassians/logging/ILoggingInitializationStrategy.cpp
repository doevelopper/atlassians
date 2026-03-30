/*******************************************************************
 * @file   ILoggingInitializationStrategy.cpp
 * @version  v1.0.0
 * @date     2024/12/29
 * @brief    Implementation of ILoggingInitializationStrategy protected methods
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
 ********************************************************************/

#include <com/github/doevelopper/atlassians/logging/ILoggingInitializationStrategy.hpp>

#include <log4cxx/logmanager.h>
#include <log4cxx/spi/loggerrepository.h>

namespace com::github::doevelopper::atlassians::logging
{
    bool ILoggingInitializationStrategy::isAlreadyConfigured() const noexcept
    {
        try
        {
            auto repository = ::log4cxx::LogManager::getLoggerRepository();
            return repository && repository->isConfigured();
        }
        catch (...)
        {
            return false;
        }
    }

    void ILoggingInitializationStrategy::markAsConfigured() const
    {
        auto repository = ::log4cxx::LogManager::getLoggerRepository();
        if (repository)
        {
            repository->setConfigured(true);
        }
    }

}  // namespace com::github::doevelopper::atlassians::logging
