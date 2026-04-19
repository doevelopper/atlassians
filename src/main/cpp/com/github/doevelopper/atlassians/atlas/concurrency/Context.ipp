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
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>

namespace Bloomberg::quantum
{
        //==============================================================================================
        //                       class IThreadContext (fwd to implementation)
        //==============================================================================================
        template <class RET>
        template <class V>
        NonBufferRetType<V> IThreadContext<RET>::get()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->get();
        }

        template <class RET>
        template <class V>
        const NonBufferRetType<V> & IThreadContext<RET>::getRef() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getRef();
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> IThreadContext<RET>::getAt(int num)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->template getAt<OTHER_RET>(num);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & IThreadContext<RET>::getRefAt(int num) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->template getRefAt<OTHER_RET>(num);
        }

        template <class RET>
        template <class V, class>
        int IThreadContext<RET>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->set(std::forward<V>(value));
        }

        template <class RET>
        template <class V, class>
        void IThreadContext<RET>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            static_cast<Impl *>(this)->push(std::forward<V>(value));
        }

        template <class RET>
        template <class V>
        BufferRetType<V> IThreadContext<RET>::pull(bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->pull(isBufferClosed);
        }

        template <class RET>
        template <class V, class>
        int IThreadContext<RET>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->closeBuffer();
        }

        template <class RET>
        int IThreadContext<RET>::getNumCoroutineThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getNumCoroutineThreads();
        }

        template <class RET>
        int IThreadContext<RET>::getNumIoThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getNumIoThreads();
        }

        template <class RET>
        const std::pair<int, int> & IThreadContext<RET>::getCoroQueueIdRangeForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getCoroQueueIdRangeForAny();
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::then(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template then<Ret>(std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::then2(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template then2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::onError(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template onError<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::onError2(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template onError2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::finally(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template finally<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto IThreadContext<RET>::finally2(FUNC && func, ARGS &&... args) -> ThreadContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template finally2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        typename IThreadContext<RET>::Ptr IThreadContext<RET>::end()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->end();
        }

        //==============================================================================================
        //                       class ICoroContext (fwd to implementation)
        //==============================================================================================
        template <class RET>
        template <class V>
        NonBufferRetType<V> ICoroContext<RET>::get(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->get(sync);
        }

        template <class RET>
        template <class V>
        const NonBufferRetType<V> & ICoroContext<RET>::getRef(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getRef(sync);
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> ICoroContext<RET>::getPrev()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Impl> ctx = static_cast<Impl *>(this)->shared_from_this();
            return ctx->template getPrev<OTHER_RET>(ctx);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & ICoroContext<RET>::getPrevRef()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Impl> ctx = static_cast<Impl *>(this)->shared_from_this();
            return ctx->template getPrevRef<OTHER_RET>(ctx);
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> ICoroContext<RET>::getAt(int num, ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->template getAt<OTHER_RET>(num, sync);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & ICoroContext<RET>::getRefAt(int num, ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->template getRefAt<OTHER_RET>(num, sync);
        }

        template <class RET>
        template <class V, class>
        int ICoroContext<RET>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Impl> ctx = static_cast<Impl *>(this)->shared_from_this();
            return ctx->set(ctx, std::forward<V>(value));
        }

        template <class RET>
        template <class V, class>
        void ICoroContext<RET>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Impl> ctx = static_cast<Impl *>(this)->shared_from_this();
            ctx->push(ctx, std::forward<V>(value));
        }

        template <class RET>
        template <class V>
        BufferRetType<V> ICoroContext<RET>::pull(ICoroSync::Ptr sync, bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->pull(sync, isBufferClosed);
        }

        template <class RET>
        template <class V, class>
        int ICoroContext<RET>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Impl> ctx = static_cast<Impl *>(this)->shared_from_this();
            return ctx->closeBuffer(ctx);
        }

        template <class RET>
        int ICoroContext<RET>::getNumCoroutineThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getNumCoroutineThreads();
        }

        template <class RET>
        int ICoroContext<RET>::getNumIoThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getNumIoThreads();
        }

        template <class RET>
        const std::pair<int, int> & ICoroContext<RET>::getCoroQueueIdRangeForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<const Impl *>(this)->getCoroQueueIdRangeForAny();
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::post(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template post<Ret>(std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::post2(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template post2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::post(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template post<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::post2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template post2<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postFirst(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template postFirst<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postFirst2(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template postFirst2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postFirst(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template postFirst<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postFirst2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template postFirst2<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::then(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template then<Ret>(std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::then2(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template then2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::onError(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template onError<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::onError2(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template onError2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::finally(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(coroResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template finally<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::finally2(FUNC && func, ARGS &&... args) -> CoroContextPtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template finally2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        typename ICoroContext<RET>::Ptr ICoroContext<RET>::end()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return static_cast<Impl *>(this)->end();
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postAsyncIo(FUNC && func, ARGS &&... args) -> CoroFuturePtr<decltype(ioResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(ioResult(func));
            return static_cast<Impl *>(this)->template postAsyncIo<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postAsyncIo2(FUNC && func, ARGS &&... args) -> CoroFuturePtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template postAsyncIo2<Ret>(
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postAsyncIo(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroFuturePtr<decltype(ioResult(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(ioResult(func));
            return static_cast<Impl *>(this)->template postAsyncIo<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        auto ICoroContext<RET>::postAsyncIo2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
            -> CoroFuturePtr<decltype(resultOf2(func))>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(resultOf2(func));
            return static_cast<Impl *>(this)->template postAsyncIo2<Ret>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC, class>
        auto ICoroContext<RET>::forEach(INPUT_IT first, INPUT_IT last, FUNC && func)
            -> CoroContextPtr<std::vector<decltype(coroResult(func))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template forEach<Ret>(first, last, std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC>
        auto ICoroContext<RET>::forEach(INPUT_IT first, size_t num, FUNC && func)
            -> CoroContextPtr<std::vector<decltype(coroResult(func))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template forEach<Ret>(first, num, std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC, class>
        auto ICoroContext<RET>::forEachBatch(INPUT_IT first, INPUT_IT last, FUNC && func)
            -> CoroContextPtr<std::vector<std::vector<decltype(coroResult(func))>>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template forEachBatch<Ret>(first, last, std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC>
        auto ICoroContext<RET>::forEachBatch(INPUT_IT first, size_t num, FUNC && func)
            -> CoroContextPtr<std::vector<std::vector<decltype(coroResult(func))>>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Ret = decltype(coroResult(func));
            return static_cast<Impl *>(this)->template forEachBatch<Ret>(first, num, std::forward<FUNC>(func));
        }

        template <class RET>
        template <
            class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class MAPPER_FUNC, class REDUCER_FUNC, class INPUT_IT,
            class>
        auto ICoroContext<RET>::mapReduce(INPUT_IT first, INPUT_IT last, MAPPER_FUNC mapper, REDUCER_FUNC reducer)
            -> CoroContextPtr<std::map<decltype(mappedKeyOf(mapper)), decltype(reducedTypeOf(reducer))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Key         = decltype(mappedKeyOf(mapper));
            using MappedType  = decltype(mappedTypeOf(mapper));
            using ReducedType = decltype(reducedTypeOf(reducer));
            return static_cast<Impl *>(this)->template mapReduce<Key, MappedType, ReducedType>(
                first, last, std::move(mapper), std::move(reducer));
        }

        template <class RET>
        template <
            class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class MAPPER_FUNC, class REDUCER_FUNC, class INPUT_IT,
            class>
        auto ICoroContext<RET>::mapReduce(INPUT_IT first, size_t num, MAPPER_FUNC mapper, REDUCER_FUNC reducer)
            -> CoroContextPtr<std::map<decltype(mappedKeyOf(mapper)), decltype(reducedTypeOf(reducer))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Key         = decltype(mappedKeyOf(mapper));
            using MappedType  = decltype(mappedTypeOf(mapper));
            using ReducedType = decltype(reducedTypeOf(reducer));
            return static_cast<Impl *>(this)->template mapReduce<Key, MappedType, ReducedType>(
                first, num, std::move(mapper), std::move(reducer));
        }

        template <class RET>
        template <
            class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class MAPPER_FUNC, class REDUCER_FUNC, class INPUT_IT,
            class>
        auto ICoroContext<RET>::mapReduceBatch(INPUT_IT first, INPUT_IT last, MAPPER_FUNC mapper, REDUCER_FUNC reducer)
            -> CoroContextPtr<std::map<decltype(mappedKeyOf(mapper)), decltype(reducedTypeOf(reducer))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Key         = decltype(mappedKeyOf(mapper));
            using MappedType  = decltype(mappedTypeOf(mapper));
            using ReducedType = decltype(reducedTypeOf(reducer));
            return static_cast<Impl *>(this)->template mapReduceBatch<Key, MappedType, ReducedType>(
                first, last, std::move(mapper), std::move(reducer));
        }

        template <class RET>
        template <
            class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class MAPPER_FUNC, class REDUCER_FUNC, class INPUT_IT,
            class>
        auto ICoroContext<RET>::mapReduceBatch(INPUT_IT first, size_t num, MAPPER_FUNC mapper, REDUCER_FUNC reducer)
            -> CoroContextPtr<std::map<decltype(mappedKeyOf(mapper)), decltype(reducedTypeOf(reducer))>>
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using Key         = decltype(mappedKeyOf(mapper));
            using MappedType  = decltype(mappedTypeOf(mapper));
            using ReducedType = decltype(reducedTypeOf(reducer));
            return static_cast<Impl *>(this)->template mapReduceBatch<Key, MappedType, ReducedType>(
                first, num, std::move(mapper), std::move(reducer));
        }

//==============================================================================================
//                                     class Context
//==============================================================================================
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
    #ifdef __QUANTUM_ALLOCATE_POOL_FROM_HEAP
        using ContextAllocator = HeapAllocator<Context<int>>;
    #else
        using ContextAllocator = StackAllocator<Context<int>, __QUANTUM_CONTEXT_ALLOC_SIZE>;
    #endif
#else
        using ContextAllocator = StlAllocator<Context<int>>;
#endif

        template <class RET>
        Context<RET>::Context(DispatcherCore & dispatcher)
            : m_promises(1, PromisePtr<RET>(new Promise<RET>(), Promise<RET>::deleter))
            , m_dispatcher(&dispatcher)
            , m_terminated(false)
            , m_signal(-1)
            , m_yield(nullptr)
            , m_sleepDuration(0)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
        }

        template <class RET>
        template <class OTHER_RET>
        Context<RET>::Context(Context<OTHER_RET> & other)
            : m_promises(other.m_promises)
            , m_dispatcher(other.m_dispatcher)
            , m_terminated(false)
            , m_signal(-1)
            , m_yield(nullptr)
            , m_sleepDuration(0)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_promises.emplace_back(PromisePtr<RET>(new Promise<RET>(), Promise<RET>::deleter)); // append a new promise
        }

        template <class RET>
        Context<RET>::~Context()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            terminate();
        }

        template <class RET>
        void Context<RET>::terminate()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            bool value {false};
            if (m_terminated.compare_exchange_strong(value, true))
            {
                m_promises.back()->terminate();

                // unlink task ptr
                m_task.reset();
            }
        }

        template <class RET>
        TaskId Context<RET>::taskId() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_task->getTaskId();
        }

        template <class RET>
        bool Context<RET>::validAt(int num) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_promises[index(num)]->valid();
        }

        template <class RET>
        bool Context<RET>::valid() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return validAt(-1);
        }

        template <class RET>
        int Context<RET>::setException(std::exception_ptr ex)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_promises.back()->setException(this->shared_from_this(), ex);
        }

        template <class RET>
        bool Context<RET>::isBlocked() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_signal == 0;
        }

        template <class RET>
        bool Context<RET>::isSleeping(bool updateTimer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_sleepDuration.count() > 0)
            {
                if (!updateTimer)
                {
                    return true;
                }
                auto now     = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - m_sleepTimestamp);
                if (elapsed <= m_sleepDuration)
                {
                    return true;
                }
                // reset value since we have expired
                m_sleepDuration  = std::chrono::microseconds::zero();
                m_sleepTimestamp = std::chrono::steady_clock::time_point {};
            }
            return false;
        }

        template <class RET>
        int Context<RET>::index(int num) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if ((num < -1) || (num >= (int)m_promises.size()))
            {
                ThrowFutureException(FutureState::NoState);
            }
            return (num == -1) ? m_promises.size() - 1 : num;
        }

        template <class RET>
        void Context<RET>::validateTaskType(ITask::Type type) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_task)
            {
                throw std::invalid_argument("Invalid task pointer");
            }

            bool isValid = true;
            switch (type)
            {
            case ITask::Type::Continuation:
            case ITask::Type::ErrorHandler:
                if ((m_task->getType() != ITask::Type::First) && (m_task->getType() != ITask::Type::Continuation))
                {
                    isValid = false;
                }
                break;
            case ITask::Type::Final:
                if ((m_task->getType() != ITask::Type::First) && (m_task->getType() != ITask::Type::Continuation)
                    && (m_task->getType() != ITask::Type::ErrorHandler))
                {
                    isValid = false;
                }
                break;
            case ITask::Type::Termination:
                if ((m_task->getType() != ITask::Type::First) && (m_task->getType() != ITask::Type::Continuation)
                    && (m_task->getType() != ITask::Type::ErrorHandler) && (m_task->getType() != ITask::Type::Final))
                {
                    isValid = false;
                }
                break;
            default:
                break;
            }

            if (!isValid)
            {
                throw std::runtime_error("Restricted continuation method");
            }
        }

        template <class RET>
        void Context<RET>::validateContext(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (static_cast<const ICoroSync *>(this) == sync.get())
            {
                throw std::runtime_error("Must use different synchronization object");
            }
        }

        template <class RET>
        void Context<RET>::setTask(ITask::Ptr task)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_task = task;
        }

        template <class RET>
        ITask::Ptr Context<RET>::getTask() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_task;
        }

        template <class RET>
        void Context<RET>::setYieldHandle(Traits::Yield & yield)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_yield = &yield;
        }

        template <class RET>
        Traits::Yield & Context<RET>::getYieldHandle()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (!m_yield)
            {
                throw std::runtime_error("Yield handle is null");
            }
            return *m_yield;
        }

        template <class RET>
        void Context<RET>::yield()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            getYieldHandle()();
        }

        template <class RET>
        std::atomic_int & Context<RET>::signal()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_signal;
        }

        template <class RET>
        void Context<RET>::sleep(const std::chrono::milliseconds & timeMs)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            sleep(std::chrono::duration_cast<std::chrono::microseconds>(timeMs));
        }

        template <class RET>
        void Context<RET>::sleep(const std::chrono::microseconds & timeUs)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            m_sleepDuration  = timeUs;
            m_sleepTimestamp = std::chrono::steady_clock::now();
            if (isSleeping())
            {
                yield();
            }
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::thenImpl(ITask::Type type, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using FirstArg = decltype(firstArgOf(func));
            auto ctx       = ContextPtr<OTHER_RET>(new Context<OTHER_RET>(*this), Context<OTHER_RET>::deleter);
            auto task      = Task::Ptr(
                new Task(
                    Traits::IsVoidContext<FirstArg> {}, ctx,
                    m_task->getQueueId(), // keep current queueId
                    m_task->isHighPriority(), // keep current priority
                    type, std::forward<FUNC>(func), std::forward<ARGS>(args)...),
                Task::deleter);
            ctx->setTask(task);

            // Chain tasks
            std::static_pointer_cast<ITaskContinuation>(m_task)->setNextTask(task);
            task->setPrevTask(std::static_pointer_cast<ITaskContinuation>(m_task));
            return ctx;
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::then(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Previous task must either be First or Continuation types
            validateTaskType(ITask::Type::Continuation);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::Continuation, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::then2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // Previous task must either be First or Continuation types
            validateTaskType(ITask::Type::Continuation);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::Continuation, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::onError(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateTaskType(ITask::Type::ErrorHandler);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::ErrorHandler, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::onError2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateTaskType(ITask::Type::ErrorHandler);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::ErrorHandler, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::finally(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateTaskType(ITask::Type::Final);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::Final, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::finally2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateTaskType(ITask::Type::Final);
            return thenImpl<OTHER_RET, FUNC, ARGS...>(
                ITask::Type::Final, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        typename Context<RET>::Ptr Context<RET>::end()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateTaskType(ITask::Type::Termination);
            // Async post to next available queue since the previous task in the chain already terminated
            auto task =
                std::static_pointer_cast<Task>(std::static_pointer_cast<ITaskContinuation>(getTask())->getFirstTask());
            m_dispatcher->post(task);
            return dynamic_cast<Context<RET> *>(this)->shared_from_this();
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        CoroFuturePtr<OTHER_RET> Context<RET>::postAsyncIo(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postAsyncIoImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        CoroFuturePtr<OTHER_RET> Context<RET>::postAsyncIo2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postAsyncIoImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        CoroFuturePtr<OTHER_RET>
        Context<RET>::postAsyncIo(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postAsyncIoImpl<OTHER_RET>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        CoroFuturePtr<OTHER_RET>
        Context<RET>::postAsyncIo2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postAsyncIoImpl<OTHER_RET>(
                queueId, isHighPriority, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        CoroFuturePtr<OTHER_RET>
        Context<RET>::postAsyncIoImpl(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using FirstArg = decltype(firstArgOf(func));
            if (queueId < static_cast<int>(IQueue::QueueId::Any))
            {
                throw std::out_of_range(std::string {"Invalid coroutine queue id: "} + std::to_string(queueId));
            }
            auto promise = PromisePtr<OTHER_RET>(new Promise<OTHER_RET>(), Promise<OTHER_RET>::deleter);
            auto task    = IoTask::Ptr(
                new IoTask(
                    Traits::IsThreadPromise<FirstArg> {}, promise, queueId, isHighPriority, std::forward<FUNC>(func),
                    std::forward<ARGS>(args)...),
                IoTask::deleter);
            m_dispatcher->postAsyncIo(task);
            return promise->getICoroFuture();
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC, class>
        ContextPtr<std::vector<OTHER_RET>> Context<RET>::forEach(INPUT_IT first, INPUT_IT last, FUNC && func)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return forEach<OTHER_RET>(first, std::distance(first, last), std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC>
        ContextPtr<std::vector<OTHER_RET>> Context<RET>::forEach(INPUT_IT first, size_t num, FUNC && func)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return post2<std::vector<OTHER_RET>>(
                Util::forEachCoro<OTHER_RET, INPUT_IT, FUNC &&>, INPUT_IT {first}, size_t {num},
                std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC, class>
        ContextPtr<std::vector<std::vector<OTHER_RET>>>
        Context<RET>::forEachBatch(INPUT_IT first, INPUT_IT last, FUNC && func)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return forEachBatch<OTHER_RET>(first, std::distance(first, last), std::forward<FUNC>(func));
        }

        template <class RET>
        template <class OTHER_RET, class INPUT_IT, class FUNC>
        ContextPtr<std::vector<std::vector<OTHER_RET>>>
        Context<RET>::forEachBatch(INPUT_IT first, size_t num, FUNC && func)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return post2<std::vector<std::vector<OTHER_RET>>>(
                Util::forEachBatchCoro<OTHER_RET, INPUT_IT, FUNC &&>, INPUT_IT {first}, size_t {num},
                std::forward<FUNC>(func), getNumCoroutineThreads());
        }

        template <class RET>
        template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT, class>
        ContextPtr<std::map<KEY, REDUCED_TYPE>> Context<RET>::mapReduce(
            INPUT_IT first, INPUT_IT last, Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> mapper,
            Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> reducer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return mapReduce(first, std::distance(first, last), std::move(mapper), std::move(reducer));
        }

        template <class RET>
        template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT>
        ContextPtr<std::map<KEY, REDUCED_TYPE>> Context<RET>::mapReduce(
            INPUT_IT first, size_t num, Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> mapper,
            Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> reducer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using ReducerOutput = std::map<KEY, REDUCED_TYPE>;
            return post2<ReducerOutput>(
                Util::mapReduceCoro<KEY, MAPPED_TYPE, REDUCED_TYPE, INPUT_IT>, INPUT_IT {first}, size_t {num},
                Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> {std::move(mapper)},
                Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> {std::move(reducer)});
        }

        template <class RET>
        template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT, class>
        ContextPtr<std::map<KEY, REDUCED_TYPE>> Context<RET>::mapReduceBatch(
            INPUT_IT first, INPUT_IT last, Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> mapper,
            Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> reducer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return mapReduceBatch(first, std::distance(first, last), std::move(mapper), std::move(reducer));
        }

        template <class RET>
        template <class KEY, class MAPPED_TYPE, class REDUCED_TYPE, class INPUT_IT>
        ContextPtr<std::map<KEY, REDUCED_TYPE>> Context<RET>::mapReduceBatch(
            INPUT_IT first, size_t num, Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> mapper,
            Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> reducer)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using ReducerOutput = std::map<KEY, REDUCED_TYPE>;
            return post2<ReducerOutput>(
                Util::mapReduceBatchCoro<KEY, MAPPED_TYPE, REDUCED_TYPE, INPUT_IT>, INPUT_IT {first}, size_t {num},
                Functions::MapFunc<KEY, MAPPED_TYPE, INPUT_IT> {std::move(mapper)},
                Functions::ReduceFunc<KEY, MAPPED_TYPE, REDUCED_TYPE> {std::move(reducer)});
        }

        template <class RET>
        template <class V, class>
        void Context<RET>::push(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            push(nullptr, std::forward<V>(value));
        }

        template <class RET>
        template <class V, class>
        void Context<RET>::push(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::static_pointer_cast<Promise<RET>>(m_promises.back())->push(sync, std::forward<V>(value));
        }

        template <class RET>
        template <class V>
        BufferRetType<V> Context<RET>::pull(bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return pull(nullptr, isBufferClosed);
        }

        template <class RET>
        template <class V>
        BufferRetType<V> Context<RET>::pull(ICoroSync::Ptr sync, bool & isBufferClosed)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return std::static_pointer_cast<Promise<RET>>(m_promises.back())
                ->getICoroFuture()
                ->pull(sync, isBufferClosed);
        }

        template <class RET>
        template <class V, class>
        int Context<RET>::closeBuffer()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return closeBuffer(nullptr);
        }

        template <class RET>
        template <class V, class>
        int Context<RET>::closeBuffer(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return std::static_pointer_cast<Promise<RET>>(m_promises.back())->closeBuffer(sync);
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> Context<RET>::getAt(int num)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getAt<OTHER_RET>(num, nullptr);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & Context<RET>::getRefAt(int num) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getRefAt<OTHER_RET>(num, nullptr);
        }

        template <class RET>
        template <class V>
        NonBufferRetType<V> Context<RET>::get()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return get(nullptr);
        }

        template <class RET>
        template <class V>
        const NonBufferRetType<V> & Context<RET>::getRef() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getRef(nullptr);
        }

        template <class RET>
        void Context<RET>::waitAt(int num) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            waitAt(num, nullptr);
        }

        template <class RET>
        std::future_status Context<RET>::waitForAt(int num, std::chrono::milliseconds timeMs) const
        {
            return waitForAt(num, nullptr, timeMs);
        }

        template <class RET>
        void Context<RET>::wait() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            wait(nullptr);
        }

        template <class RET>
        std::future_status Context<RET>::waitFor(std::chrono::milliseconds timeMs) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitFor(nullptr, timeMs);
        }

        template <class RET>
        void Context<RET>::waitAll() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            waitAll(nullptr);
        }

        template <class RET>
        template <class V, class>
        int Context<RET>::set(V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return set(nullptr, std::forward<V>(value));
        }

        template <class RET>
        template <class V, class>
        int Context<RET>::set(ICoroSync::Ptr sync, V && value)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return std::static_pointer_cast<Promise<RET>>(m_promises.back())->set(sync, std::forward<V>(value));
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> Context<RET>::getAt(int num, ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateContext(sync);
            return std::static_pointer_cast<Promise<OTHER_RET>>(m_promises[index(num)])->getICoroFuture()->get(sync);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & Context<RET>::getRefAt(int num, ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateContext(sync);
            return std::static_pointer_cast<Promise<OTHER_RET>>(m_promises[index(num)])->getICoroFuture()->getRef(sync);
        }

        template <class RET>
        template <class V>
        NonBufferRetType<V> Context<RET>::get(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getAt<RET>(-1, sync);
        }

        template <class RET>
        template <class V>
        const NonBufferRetType<V> & Context<RET>::getRef(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return getRefAt<RET>(-1, sync);
        }

        template <class RET>
        template <class OTHER_RET>
        NonBufferRetType<OTHER_RET> Context<RET>::getPrev(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_promises.size() < 2)
            {
                ThrowFutureException(FutureState::NoState);
            }
            return std::static_pointer_cast<Promise<OTHER_RET>>(m_promises[index(m_promises.size() - 2)])
                ->getICoroFuture()
                ->get(sync);
        }

        template <class RET>
        template <class OTHER_RET>
        const NonBufferRetType<OTHER_RET> & Context<RET>::getPrevRef(ICoroSync::Ptr sync)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_promises.size() < 2)
            {
                ThrowFutureException(FutureState::NoState);
            }
            return std::static_pointer_cast<Promise<OTHER_RET>>(m_promises[index(m_promises.size() - 2)])
                ->getICoroFuture()
                ->getRef(sync);
        }

        template <class RET>
        int Context<RET>::getNumCoroutineThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_dispatcher->getNumCoroutineThreads();
        }

        template <class RET>
        int Context<RET>::getNumIoThreads() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_dispatcher->getNumIoThreads();
        }

        template <class RET>
        const std::pair<int, int> & Context<RET>::getCoroQueueIdRangeForAny() const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return m_dispatcher->getCoroQueueIdRangeForAny();
        }

        template <class RET>
        void Context<RET>::waitAt(int num, ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateContext(sync);
            m_promises[index(num)]->getICoroFutureBase()->wait(sync);
        }

        template <class RET>
        std::future_status Context<RET>::waitForAt(int num, ICoroSync::Ptr sync, std::chrono::milliseconds timeMs) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            validateContext(sync);
            return m_promises[index(num)]->getICoroFutureBase()->waitFor(sync, timeMs);
        }

        template <class RET>
        void Context<RET>::wait(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            waitAt(-1, sync);
        }

        template <class RET>
        std::future_status Context<RET>::waitFor(ICoroSync::Ptr sync, std::chrono::milliseconds timeMs) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return waitForAt(-1, sync, timeMs);
        }

        template <class RET>
        void Context<RET>::waitAll(ICoroSync::Ptr sync) const
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            for (auto && promise : m_promises)
            {
                try
                {
                    promise->getICoroFutureBase()->wait(sync);
                }
                catch (const Traits::CoroutineStackUnwind &)
                {
                    // allow coroutine stack to unwind properly
                    throw;
                }
                catch (...) // catch all broken promises or any other exception
                {
                }
            }
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::post(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, ITask::Type::Standalone, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::post2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, ITask::Type::Standalone, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::post(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                queueId, isHighPriority, ITask::Type::Standalone, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::post2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                queueId, isHighPriority, ITask::Type::Standalone, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::postFirst(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, ITask::Type::First, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::postFirst2(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                static_cast<int>(IQueue::QueueId::Any), false, ITask::Type::First, std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::postFirst(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                queueId, isHighPriority, ITask::Type::First, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET> Context<RET>::postFirst2(int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return postImpl<OTHER_RET>(
                queueId, isHighPriority, ITask::Type::First, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class RET>
        template <class OTHER_RET, class FUNC, class... ARGS>
        ContextPtr<OTHER_RET>
        Context<RET>::postImpl(int queueId, bool isHighPriority, ITask::Type type, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            using FirstArg = decltype(firstArgOf(func));
            if (queueId < static_cast<int>(IQueue::QueueId::Same))
            {
                throw std::out_of_range(std::string {"Invalid coroutine queue id: "} + std::to_string(queueId));
            }
            auto ctx  = ContextPtr<OTHER_RET>(new Context<OTHER_RET>(*m_dispatcher), Context<OTHER_RET>::deleter);
            auto task = Task::Ptr(
                new Task(
                    Traits::IsVoidContext<FirstArg> {}, ctx,
                    (queueId == static_cast<int>(IQueue::QueueId::Same)) ? m_task->getQueueId() : queueId, isHighPriority, type,
                    std::forward<FUNC>(func), std::forward<ARGS>(args)...),
                Task::deleter);
            ctx->setTask(task);
            if (type == ITask::Type::Standalone)
            {
                m_dispatcher->post(task);
            }
            return ctx;
        }

        template <class RET>
        void * Context<RET>::operator new(size_t)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return Allocator<ContextAllocator>::instance(AllocatorTraits::contextAllocSize()).allocate();
        }

        template <class RET>
        void Context<RET>::operator delete(void * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            Allocator<ContextAllocator>::instance(AllocatorTraits::contextAllocSize())
                .deallocate(static_cast<Context<int> *>(p));
        }

        template <class RET>
        void Context<RET>::deleter(Context<RET> * p)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
#ifndef __QUANTUM_USE_DEFAULT_ALLOCATOR
            Allocator<ContextAllocator>::instance(AllocatorTraits::contextAllocSize())
                .dispose(reinterpret_cast<Context<int> *>(p));
#else
            delete p;
#endif
        }
}
