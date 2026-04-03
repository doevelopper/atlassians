/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     17/06/24 W 20:23
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/
#ifndef COM_GITHUB_DOEVELOPPER_ATLAS_BARRIERS_RECURSIVEREADLOCK_HPP
#define COM_GITHUB_DOEVELOPPER_ATLAS_BARRIERS_RECURSIVEREADLOCK_HPP

#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveReadWriteMutex.hpp>


namespace com::github::doevelopper::atlassians::atlas::barriers
{
    class RecursiveReadLock
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        RecursiveReadLock() noexcept;
        RecursiveReadLock(const RecursiveReadLock&) = default;
        RecursiveReadLock(RecursiveReadLock&&) = default;
        RecursiveReadLock& operator=(const RecursiveReadLock&) = default;
        RecursiveReadLock& operator=(RecursiveReadLock&&) = default;
        virtual ~RecursiveReadLock() noexcept;

        explicit RecursiveReadLock(RecursiveReadWriteMutex& mutex);

    protected:
    private:
        RecursiveReadWriteMutex& m_mutex;
    };
}
#endif
