/*****************************************************************************
 * SimplestType .hpp
 *
 * Created: 31/10/2023 2023 by hroland
 *
 * Copyright 2023 hroland. All rights reserved.
 *
 * This file may be distributed under the terms of GNU Public License version
 * 3 (GPL v3) as defined by the Free Software Foundation (FSF). A copy of the
 * license should have been included with this file, or the project in which
 * this file belongs to. You may also find the details of GPL v3 at:
 * http://www.gnu.org/licenses/gpl-3.0.txt
 *
 * If you have any questions regarding the use of this file, feel free to
 * contact the author of this file, or the owner of the project in which
 * this file belongs to.
 *****************************************************************************/
#ifndef SIMPLESTTYPE_HPP
#define SIMPLESTTYPE_HPP

/*!
 * @brief SimplestType plays the role of a simplest type in case we couldn't
 *    use void directly.
 *    User shouldn't use this directly.
 */
namespace Bloomberg::quantum
{
    class SimplestType
    {
    public:

        SimplestType();

        constexpr bool operator==(const SimplestType &) const
        {
            return true;
        }

        constexpr bool operator!=(const SimplestType &) const
        {
            return false;
        }
    };
}
#endif // SIMPLESTTYPE_HPP
