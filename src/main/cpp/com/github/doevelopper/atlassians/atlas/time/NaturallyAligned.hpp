/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     14/07/24 W 10:49
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/


#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_NATURALLYALIGNED_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_NATURALLYALIGNED_HPP

// #include <com/github/doevelopper/atlassians/atlas/time/ShellHandle.hpp>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

#include <atomic>
#include <bit>
#include <limits>
#include <type_traits>

namespace com::github::doevelopper::atlassians::atlas::time
{

    /*
     * @ingroup utils
     *
     *  @brief Ensures the object is naturally aligned to a power of 2 bytes greater
     *       than or equal to its size. `NaturallyAligned` is a wrapper class.
     *
     *  Example:
     *  @code{.cpp}
     *  #include "alignment.h"
     *
     *  std::atomic<NaturallyAligned<std::optional<bool>>> nat_aligned_obj;
     *
     *  @endcode
    */
    template <typename T>
    class [[gnu::aligned(std::bit_ceil(sizeof(T)))]] NaturallyAligned : public T
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        NaturallyAligned() noexcept;
        NaturallyAligned(const NaturallyAligned& t) noexcept;
        NaturallyAligned(NaturallyAligned&&) = default;
        NaturallyAligned& operator=(const NaturallyAligned&) = default;
        NaturallyAligned& operator=(NaturallyAligned&&) = default;
        virtual ~NaturallyAligned() noexcept;

        template <class U>
        NaturallyAligned(const U& u) noexcept;

    protected:
    private:
    };
}

#endif
