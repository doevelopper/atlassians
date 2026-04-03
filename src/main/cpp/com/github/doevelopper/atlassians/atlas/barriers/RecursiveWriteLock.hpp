/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     17/06/24 W 20:23
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_BARRIERS_RECURSIVEWRITELOCK_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_BARRIERS_RECURSIVEWRITELOCK_HPP

#include <com/github/doevelopper/atlassians/atlas/barriers/RecursiveReadWriteMutex.hpp>


namespace com::github::doevelopper::atlassians::atlas::barriers
{
    class RecursiveWriteLock
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        RecursiveWriteLock() noexcept;
        RecursiveWriteLock(const RecursiveWriteLock&) = default;
        RecursiveWriteLock(RecursiveWriteLock&&) = default;
        RecursiveWriteLock& operator=(const RecursiveWriteLock&) = default;
        RecursiveWriteLock& operator=(RecursiveWriteLock&&) = default;
        virtual ~RecursiveWriteLock() noexcept;

        explicit RecursiveWriteLock(RecursiveReadWriteMutex& mutex);
    protected:
    private:
        RecursiveReadWriteMutex& m_mutex;
    };
}

#endif
