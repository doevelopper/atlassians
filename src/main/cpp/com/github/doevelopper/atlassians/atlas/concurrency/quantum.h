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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_H

#define __NATIVESTD__
#include <com/github/doevelopper/atlassians/atlas/concurrency/Allocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/AllocatorTraits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Auxiliary.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Buffer.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Capture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ConditionVariable.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Configuration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Context.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ContiguousPoolManager.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/CoroutinePoolAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Dispatcher.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/DispatcherCore.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Functions.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Future.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/FutureState.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/HeapAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/IoQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/IoTask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Local.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Macros.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Mutex.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Promise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/QueueStatistics.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ReadWriteMutex.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ReadWriteSpinlock.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/SharedState.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Spinlock.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/SpinlockTraits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/StackAllocator.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/StackTraits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Task.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskId.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/TaskStateHandler.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/ThreadTraits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/Traits.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/YieldingThread.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IContextBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroContextBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroFutureBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroPromise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroSync.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IPromise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IPromiseBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueue.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IQueueStatistics.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITask.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITaskAccessor.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITaskContinuation.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ITerminate.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadContext.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadContextBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadFutureBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/IThreadPromise.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/DrainGuard.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/FutureJoiner.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/GenericFuture.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/LocalVariableGuard.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequenceKeyStatistics.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Sequencer.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequencerConfiguration.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequencerConfigurationExperimental.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequencerExperimental.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequencerTaskExperimental.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SimplestType.cpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SimplestType.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SpinlockUtil.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Try.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/Util.hpp>

#endif //COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_H
