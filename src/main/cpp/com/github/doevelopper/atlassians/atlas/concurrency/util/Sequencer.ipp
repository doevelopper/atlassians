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

#include <stdexcept>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Promise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/DrainGuard.hpp>

namespace Bloomberg::quantum
{
        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::Sequencer(
            Dispatcher & dispatcher,
            const typename Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::Configuration & configuration)
            : m_dispatcher(dispatcher)
            , m_drain(false)
            , m_controllerQueueId(configuration.getControlQueueId())
            , m_universalContext()
            , m_contexts(
                  configuration.getBucketCount(), configuration.getHash(), configuration.getKeyEqual(),
                  configuration.getAllocator())
            , m_exceptionCallback(configuration.getExceptionCallback())
            , m_taskStats(std::make_shared<SequenceKeyStatisticsWriter>())
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_controllerQueueId <= static_cast<int>(IQueue::QueueId::Any)
                || m_controllerQueueId >= m_dispatcher.getNumCoroutineThreads())
            {
                throw std::out_of_range(
                    "Allowed range is 0 <= controllerQueueId < _dispatcher.getNumCoroutineThreads()");
            }
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
            const SequenceKey & sequenceKey, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            m_dispatcher.post(
                m_controllerQueueId, false, singleSequenceKeyTaskScheduler<FUNC, ARGS...>, nullptr,
                static_cast<int>(IQueue::QueueId::Any), false, *this, SequenceKey(sequenceKey), std::forward<FUNC>(func),
                std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
            void * opaque, int queueId, bool isHighPriority, const SequenceKey & sequenceKey, FUNC && func,
            ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            if (queueId < static_cast<int>(IQueue::QueueId::Any))
            {
                throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
            }
            m_dispatcher.post(
                m_controllerQueueId, false, singleSequenceKeyTaskScheduler<FUNC, ARGS...>, std::move(opaque),
                std::move(queueId), std::move(isHighPriority), *this, SequenceKey(sequenceKey),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
            const std::vector<SequenceKey> & sequenceKeys, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            m_dispatcher.post(
                m_controllerQueueId, false, multiSequenceKeyTaskScheduler<FUNC, ARGS...>, nullptr,
                static_cast<int>(IQueue::QueueId::Any), false, *this, std::vector<SequenceKey>(sequenceKeys),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueue(
            void * opaque, int queueId, bool isHighPriority, const std::vector<SequenceKey> & sequenceKeys,
            FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            if (queueId < static_cast<int>(IQueue::QueueId::Any))
            {
                throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
            }
            m_dispatcher.post(
                m_controllerQueueId, false, multiSequenceKeyTaskScheduler<FUNC, ARGS...>, std::move(opaque),
                std::move(queueId), std::move(isHighPriority), *this, std::vector<SequenceKey>(sequenceKeys),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueAll(FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            m_dispatcher.post(
                m_controllerQueueId, false, universalTaskScheduler<FUNC, ARGS...>, nullptr, static_cast<int>(IQueue::QueueId::Any),
                false, *this, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        void Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::enqueueAll(
            void * opaque, int queueId, bool isHighPriority, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            if (m_drain)
            {
                throw std::runtime_error("Sequencer is disabled");
            }
            if (queueId < static_cast<int>(IQueue::QueueId::Any))
            {
                throw std::out_of_range(std::string {"Invalid IO queue id: "} + std::to_string(queueId));
            }
            m_dispatcher.post(
                m_controllerQueueId, false, universalTaskScheduler<FUNC, ARGS...>, std::move(opaque), std::move(queueId),
                std::move(isHighPriority), *this, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        size_t Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::trimSequenceKeys()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            auto trimFunc = [this](CoroContextPtr<size_t> ctx) -> int {
                for (auto it = m_contexts.begin(); it != m_contexts.end();)
                {
                    auto trimIt = it++;
                    if (canTrimContext(ctx, trimIt->second._context))
                    {
                        m_contexts.erase(trimIt);
                    }
                }
                return ctx->set(m_contexts.size());
            };
            return m_dispatcher.post(m_controllerQueueId, true, std::move(trimFunc))->get();
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        SequenceKeyStatistics
        Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getStatistics(const SequenceKey & sequenceKey)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            auto statsFunc = [this, sequenceKey](CoroContextPtr<SequenceKeyStatistics> ctx) -> int {
                typename ContextMap::iterator ctxIt = m_contexts.find(sequenceKey);
                if (ctxIt == m_contexts.end())
                {
                    return ctx->set(SequenceKeyStatistics());
                }
                return ctx->set(SequenceKeyStatistics(*ctxIt->second._stats));
            };
            return m_dispatcher.post(m_controllerQueueId, true, std::move(statsFunc))->get();
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        SequenceKeyStatistics Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getStatistics()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return *m_universalContext._stats;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        SequenceKeyStatistics Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getTaskStatistics()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return *m_taskStats;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        size_t Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::getSequenceKeyCount()
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            auto statsFunc = [this](CoroContextPtr<size_t> ctx) -> int { return ctx->set(m_contexts.size()); };
            return m_dispatcher.post(m_controllerQueueId, true, std::move(statsFunc))->get();
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::waitForTwoDependents(
            VoidContextPtr ctx, void * opaque, Sequencer & sequencer, SequenceKeyData && dependent,
            SequenceKeyData && universalDependent, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // wait until all the dependents are done
            if (dependent._context)
            {
                dependent._context->wait(ctx);
            }
            if (universalDependent._context)
            {
                universalDependent._context->wait(ctx);
            }
            int rc = callPosted(ctx, opaque, sequencer, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            // update task stats
            dependent._stats->decrementPendingTaskCount();
            sequencer.m_taskStats->decrementPendingTaskCount();
            return rc;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::waitForDependents(
            VoidContextPtr ctx, void * opaque, Sequencer & sequencer, std::vector<SequenceKeyData> && dependents,
            SequenceKeyData && universalDependent, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // wait until all the dependents are done
            for (const auto & dependent : dependents)
            {
                if (dependent._context)
                {
                    dependent._context->wait(ctx);
                }
            }
            LOG4CXX_INFO(logger, "Wait until the universal dependent is done");
            // wait until the universal dependent is done
            if (universalDependent._context)
            {
                universalDependent._context->wait(ctx);
            }
            int rc = callPosted(ctx, opaque, sequencer, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            LOG4CXX_INFO(logger, "Update task stats");
            // update task stats
            for (const auto & dependent : dependents)
            {
                dependent._stats->decrementPendingTaskCount();
            }
            sequencer.m_taskStats->decrementPendingTaskCount();
            return rc;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::waitForUniversalDependent(
            VoidContextPtr ctx, void * opaque, Sequencer & sequencer, std::vector<SequenceKeyData> && dependents,
            SequenceKeyData && universalDependent, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // wait until all the dependents are done
            for (const auto & dependent : dependents)
            {
                if (dependent._context)
                {
                    dependent._context->wait(ctx);
                }
            }
            LOG4CXX_INFO(logger, "Wait until the universal dependent is done");
            // wait until the universal dependent is done
            if (universalDependent._context)
            {
                universalDependent._context->wait(ctx);
            }
            int rc = callPosted(ctx, opaque, sequencer, std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            LOG4CXX_INFO(logger, "Update task stats");
            // update task stats
            universalDependent._stats->decrementPendingTaskCount();
            sequencer.m_taskStats->decrementPendingTaskCount();
            return rc;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::singleSequenceKeyTaskScheduler(
            VoidContextPtr ctx, void * opaque, int queueId, bool isHighPriority, Sequencer & sequencer,
            SequenceKey && sequenceKey, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // find the dependent or create a new element
            typename ContextMap::iterator contextIt = sequencer.m_contexts.emplace(sequenceKey, SequenceKeyData()).first;
            LOG4CXX_INFO(logger, "Update stats");
            // update stats
            contextIt->second._stats->incrementPostedTaskCount();
            contextIt->second._stats->incrementPendingTaskCount();
            LOG4CXX_INFO(logger, "Update task stats");
            // update task stats
            sequencer.m_taskStats->incrementPostedTaskCount();
            sequencer.m_taskStats->incrementPendingTaskCount();

            LOG4CXX_DEBUG(logger, "Save the context as the last for this sequenceKey.");
            // save the context as the last for this sequenceKey
            contextIt->second._context = ctx->post(
                std::move(queueId), std::move(isHighPriority), waitForTwoDependents<FUNC, ARGS...>, std::move(opaque),
                sequencer, SequenceKeyData(contextIt->second), SequenceKeyData(sequencer.m_universalContext),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            return 0;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::multiSequenceKeyTaskScheduler(
            VoidContextPtr ctx, void * opaque, int queueId, bool isHighPriority, Sequencer & sequencer,
            std::vector<SequenceKey> && sequenceKeys, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // construct the dependent collection
            std::vector<SequenceKeyData> dependents;
            dependents.reserve(sequenceKeys.size());
            dependents.push_back(sequencer.m_universalContext);
            for (const SequenceKey & sequenceKey : sequenceKeys)
            {
                auto taskIt = sequencer.m_contexts.find(sequenceKey);
                if (taskIt != sequencer.m_contexts.end())
                {
                    LOG4CXX_INFO(logger, "Add the dependent and increment stats");
                    // add the dependent and increment stats
                    taskIt->second._stats->incrementPostedTaskCount();
                    taskIt->second._stats->incrementPendingTaskCount();
                    dependents.emplace_back(taskIt->second);
                }
            }
            // update task stats
            sequencer.m_taskStats->incrementPostedTaskCount();
            sequencer.m_taskStats->incrementPendingTaskCount();

            ICoroContextBasePtr newCtx = ctx->post(
                std::move(queueId), std::move(isHighPriority), waitForDependents<FUNC, ARGS...>, std::move(opaque),
                sequencer, std::move(dependents), SequenceKeyData(sequencer.m_universalContext),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);

            LOG4CXX_INFO(logger, "Ssave the context as the last for each sequenceKey");
            // save the context as the last for each sequenceKey
            for (const SequenceKey & sequenceKey : sequenceKeys)
            {
                sequencer.m_contexts[sequenceKey]._context = newCtx;
            }
            return 0;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::universalTaskScheduler(
            VoidContextPtr ctx, void * opaque, int queueId, bool isHighPriority, Sequencer & sequencer, FUNC && func,
            ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // construct the dependent collection
            std::vector<SequenceKeyData> dependents;
            dependents.reserve(sequencer.m_contexts.size());
            for (auto ctxIt = sequencer.m_contexts.begin(); ctxIt != sequencer.m_contexts.end(); ++ctxIt)
            {
                LOG4CXX_INFO(logger, "Check if the context still has a pending task");
                // check if the context still has a pending task
                if (isPendingContext(ctx, ctxIt->second._context))
                {
                    LOG4CXX_INFO(logger, "Need to wait on this context to finish its current running task.");
                    // we will need to wait on this context to finish its current running task
                    dependents.emplace_back(ctxIt->second);
                }
            }
            LOG4CXX_INFO(logger, "Update the universal stats only.");
            // update the universal stats only
            sequencer.m_universalContext._stats->incrementPostedTaskCount();
            sequencer.m_universalContext._stats->incrementPendingTaskCount();
            // update task stats
            sequencer.m_taskStats->incrementPostedTaskCount();
            sequencer.m_taskStats->incrementPendingTaskCount();

            LOG4CXX_INFO(logger, "Post the task and save the context as the last for the universal sequenceKey.");
            // post the task and save the context as the last for the universal sequenceKey
            sequencer.m_universalContext._context = ctx->post(
                std::move(queueId), std::move(isHighPriority), waitForUniversalDependent<FUNC, ARGS...>,
                std::move(opaque), sequencer, std::move(dependents), SequenceKeyData(sequencer.m_universalContext),
                std::forward<FUNC>(func), std::forward<ARGS>(args)...);
            return 0;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        template <class FUNC, class... ARGS>
        int Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::callPosted(
            VoidContextPtr ctx, void * opaque, const Sequencer & sequencer, FUNC && func, ARGS &&... args)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            // make sure the final action is eventually called
            try
            {
                std::forward<FUNC>(func)(ctx, std::forward<ARGS>(args)...);
                return 0;
            }
            catch (const Traits::CoroutineStackUnwind &)
            {
                LOG4CXX_WARN(logger, "Allow coroutine stack to unwind properly.");
                // allow coroutine stack to unwind properly
                throw;
            }
            catch (const std::exception & ex)
            {
                if (sequencer.m_exceptionCallback)
                {
                    sequencer.m_exceptionCallback(std::current_exception(), opaque);
                }
            }
            catch (...)
            {
                if (sequencer.m_exceptionCallback)
                {
                    sequencer.m_exceptionCallback(std::current_exception(), opaque);
                }
            }
            LOG4CXX_ERROR(logger, "Failed to make sure the final action is eventually called.");
            return -1; // error
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        bool Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::canTrimContext(
            const ICoroContextBasePtr & ctx, const ICoroContextBasePtr & ctxToValidate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return !ctxToValidate || !ctxToValidate->valid()
                || ctxToValidate->waitFor(ctx, std::chrono::milliseconds::zero()) == std::future_status::ready;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        bool Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::isPendingContext(
            const ICoroContextBasePtr & ctx, const ICoroContextBasePtr & ctxToValidate)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            return ctxToValidate && ctxToValidate->valid()
                && ctxToValidate->waitFor(ctx, std::chrono::milliseconds::zero()) == std::future_status::timeout;
        }

        template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
        bool Sequencer<SequenceKey, Hash, KeyEqual, Allocator>::drain(std::chrono::milliseconds timeout, bool isFinal)
        {
            LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
            std::shared_ptr<Promise<int>> promise = std::make_shared<Promise<int>>();
            ThreadFuturePtr<int> future           = promise->getIThreadFuture();

            LOG4CXX_TRACE(logger, "Enqueue a universal task and wait.");
            // enqueue a universal task and wait
            enqueueAll([promise](VoidContextPtr ctx) -> int { return promise->set(ctx, 0); });

            DrainGuard guard(m_drain, !isFinal);
            return future->waitFor(timeout) == std::future_status::ready;
        }
}
