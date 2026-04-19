/*****************************************************************************
 * Try.cpp
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

//#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Try.hpp>

//namespace Bloomberg::quantum
//{
//    template <class T>
//    Try<T>::operator Try<void>() const {
//        if (hasError()) {
//            return Try<void>(getException());
//        }
//        return Try<void>();
//    }

//    template <class T>
//    Try(T) -> Try<T>;

//    template <typename F, typename... Args>
//    auto makeTryCall(F&& f, Args&&... args) {
//        using T = std::invoke_result_t<F, Args...>;
//        try {
//            if constexpr (std::is_void_v<T>) {
//                std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
//                return Try<void>();
//            } else {
//                return Try<T>(
//                    std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
//            }
//        } catch (...) {
//            return Try<T>(std::current_exception());
//        }
//    }
//}

// Try::Try() noexcept
//{

//}
// Try::~Try() noexcept
//{

//}

// Try::Try()
//{

//}
