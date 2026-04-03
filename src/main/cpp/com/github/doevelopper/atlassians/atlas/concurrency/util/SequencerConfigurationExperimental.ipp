/*
** Copyright 2021 Bloomberg Finance L.P.
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

#include <atomic>
#include <list>
#include <memory>
#include <vector>
#include <com/github/doevelopper/atlassians/atlas/concurrency/interface/ICoroContextBase.hpp>
#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequenceKeyStatistics.hpp>

namespace Bloomberg::quantum::experimental
{
            inline const std::string & SequencerConfigurationSchemaProvider::getJsonSchema()
            {
                static std::string schema = R"JSON(
    {
        "$schema" : "http://json-schema.org/draft-04/schema#",
        "$id" : "bloomberg:sequencer.experimental.quantum.json",
        "title": "Quantum sequencer settings",
        "type": "object",
        "properties": {
            "bucketCount": {
                "type": "number",
                "default": 100
            }
        },
        "additionalProperties": false,
        "required": []
    }
    )JSON";
                return schema;
            }

            inline const std::string & SequencerConfigurationSchemaProvider::getJsonSchemaUri()
            {
                static std::string uri = "bloomberg:sequencer.experimental.quantum.json";
                return uri;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator> &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::setBucketCount(size_t bucketCount)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                _bucketCount = bucketCount;
                return *this;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            size_t SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::getBucketCount() const
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return _bucketCount;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator> &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::setHash(const Hash & hash)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                _hash = hash;
                return *this;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            const Hash & SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::getHash() const
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return _hash;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator> &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::setKeyEqual(const KeyEqual & keyEqual)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                _keyEqual = keyEqual;
                return *this;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            const KeyEqual & SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::getKeyEqual() const
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return _keyEqual;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator> &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::setAllocator(const Allocator & allocator)
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                _allocator = allocator;
                return *this;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            const Allocator & SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::getAllocator() const
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return _allocator;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator> &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::setExceptionCallback(
                const ExceptionCallback & exceptionCallback)

            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                _exceptionCallback = exceptionCallback;
                return *this;
            }

            template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
            const typename SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::ExceptionCallback &
            SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::getExceptionCallback() const
            {
                LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
                return _exceptionCallback;
            }
}
