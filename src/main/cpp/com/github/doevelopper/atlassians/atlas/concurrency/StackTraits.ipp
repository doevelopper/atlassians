/*
** Copyright 2018 Bloomberg Finance L.P.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/
// NOTE: DO NOT INCLUDE DIRECTLY

// ##############################################################################################
// #################################### IMPLEMENTATIONS #########################################
// ##############################################################################################

namespace Bloomberg::quantum
{
        inline bool & StackTraits::isUnbounded()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static bool isUnbounded = boost::context::stack_traits::is_unbounded();
            return isUnbounded;
        }

        inline size_t & StackTraits::pageSize()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static size_t pageSize = boost::context::stack_traits::page_size();
            return pageSize;
        }

        inline size_t & StackTraits::defaultSize()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static size_t defaultSize = boost::context::stack_traits::default_size();
            return defaultSize;
        }

        inline size_t & StackTraits::minimumSize()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static size_t minimumSize = boost::context::stack_traits::minimum_size();
            return minimumSize;
        }

        inline size_t & StackTraits::maximumSize()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static size_t maximumSize = boost::context::stack_traits::maximum_size();
            return maximumSize;
        }
}
