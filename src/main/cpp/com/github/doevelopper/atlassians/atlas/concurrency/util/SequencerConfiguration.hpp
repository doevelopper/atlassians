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
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCER_CONFIGURATION_H
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCER_CONFIGURATION_H

#include <functional>
#include <memory>
#include <stdexcept>
#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace Bloomberg::quantum
{
        struct SequenceKeyData;

        //==============================================================================================
        //                                    class SequencerConfigurationSchemaProvider
        //==============================================================================================
        /*!
        * @class SequencerConfigurationSchemaProvider
        * @brief Provides static accessors to a json schema representing a SequencerConfiguration object
        */
        struct SequencerConfigurationSchemaProvider
        {
            /*!
             * @brief Get the JSON schema corresponding to this configuration object.
             * @return The draft-04 compatible schema.
             */
            static const std::string & getJsonSchema();

            /*!
             * @brief Get the schema URI used to resolve remote JSON references '$ref'.
             * @return The URI.
             */
            static const std::string & getJsonSchemaUri();
        };

        //==============================================================================================
        //                                      class SequencerConfiguration
        //==============================================================================================
        /*!
        * @class SequencerConfiguration.
        * @brief Implementation of a configuration class for Sequencer
        * @tparam SequenceKey Type of the key based that sequenced tasks are associated with in Sequencer
        * @tparam Hash Hash-function used for storing instances of SequenceKey in hash maps in Sequencer
        * @tparam KeyEqual The equal-function used for storing instances of SequenceKey in hash maps in Sequencer
        * @tparam Allocator The allocator used for storing instances of SequenceKey in hash maps in Sequencer
        */
        template <
            class SequenceKey, class Hash = std::hash<SequenceKey>, class KeyEqual = std::equal_to<SequenceKey>,
            class Allocator = std::allocator<std::pair<const SequenceKey, SequenceKeyData>>>
        class SequencerConfiguration : public SequencerConfigurationSchemaProvider
        {
        public:

            /*!
             * @brief Callback for unhandled exceptions in tasks posted to Sequencer
             * @param exception pointer to the thrown exception
             * @param opaque opaque data passed when posting a task
             */
            using ExceptionCallback = std::function<void(std::exception_ptr exception, void * opaque)>;

            /*!
             * @brief Sets the id of the control queue
             * @param controlQueueId the queue id
             * @remark Sequencer typically processes tasks with the lower latency when the control queue is
             *      dedicated for the sequencer control tasks only, and no other tasks are enqueued into it.
             *
             * @return A reference to itself
             */
            SequencerConfiguration & setControlQueueId(int controlQueueId);

            /*!
             * @brief Gets the id of the control queue
             * @return the queue id
             */
            int getControlQueueId() const;

            /*!
             * @brief Sets the minimal number of buckets to be used for the context hash map
             * @param bucketCount the bucket number
             * @return A reference to itself
             */
            SequencerConfiguration & setBucketCount(size_t bucketCount);

            /*!
             * @brief gets the minimal number of buckets to be used for the context hash map
             * @return the bucket number
             */
            size_t getBucketCount() const;

            /*!
             * @brief Sets the hash function to be used for the context hash map
             * @param hash the hash function
             */
            SequencerConfiguration & setHash(const Hash & hash);

            /*!
             * @brief Gets the hash function to be used for the context hash map
             * @return the hash function
             */
            const Hash & getHash() const;

            /*!
             * @brief Sets the comparison function to be used for all SequenceKey comparisons for the context hash map
             * @param keyEqual the comparison function
             */
            SequencerConfiguration & setKeyEqual(const KeyEqual & keyEqual);

            /*!
             * @brief Gets the comparison function to be used for all SequenceKey comparisons for the context hash map
             * @return the comparison function
             */
            const KeyEqual & getKeyEqual() const;

            /*!
             * @brief Sets the allocator for all SequenceKey comparisons for the context hash map
             * @param allocator the allocator
             */
            SequencerConfiguration & setAllocator(const Allocator & allocator);

            /*!
             * @brief Gets the allocator for all SequenceKey comparisons for the context hash map
             * @return the allocator
             */
            const Allocator & getAllocator() const;

            /*!
             * @brief Sets the exception callback for Sequencer
             * @param exceptionCallback the callback to set
             */
            SequencerConfiguration & setExceptionCallback(const ExceptionCallback & exceptionCallback);

            /*!
             * @brief Gets the exception callback for Sequencer
             * @return the current callback
             */
            const ExceptionCallback & getExceptionCallback() const;

        private:

            size_t m_bucketCount {100};
            Hash m_hash;
            KeyEqual m_keyEqual;
            Allocator m_allocator;
            ExceptionCallback m_exceptionCallback;
            int m_controllerQueueId {0};
            LOG4CXX_DECLARE_STATIC_LOGGER
        };

    template <class SequenceKey, class Hash, class KeyEqual, class Allocator>
    inline log4cxx::LoggerPtr SequencerConfiguration<SequenceKey, Hash, KeyEqual, Allocator>::logger =
        log4cxx::Logger::getLogger ( std::string ( "com.github.doevelopper.premisses.atlas.concurrency.SequencerConfiguration" ) );
}

#include <com/github/doevelopper/atlassians/atlas/concurrency/util/SequencerConfiguration.ipp>

#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_CONCURRENCY_SEQUENCER_CONFIGURATION_H
