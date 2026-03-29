
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_SEQUENCERTESTDATA_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_QUANTUM_SEQUENCERTESTDATA_HPP

#include <com/github/doevelopper/premisses/atlas/concurrency/Fixture.hpp>
#include <com/github/doevelopper/premisses/atlas/concurrency/quantum.h>

namespace com::github::doevelopper::premisses::atlas::concurrency::test
{

    class SequencerTestData
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

    public:
        SequencerTestData( ) noexcept;
        SequencerTestData(const SequencerTestData&) = default;
        SequencerTestData(SequencerTestData&&) = default;
        SequencerTestData& operator=(const SequencerTestData&) = default;
        SequencerTestData& operator=(SequencerTestData&&) = default;
        virtual ~SequencerTestData() noexcept;

        using SequenceKey = int;
        using TaskId = int;
        using TaskSequencer = Bloomberg::quantum::Sequencer<SequenceKey>;
        using TaskSequencerConfiguration = Bloomberg::quantum::SequencerConfiguration<SequenceKey>;

        struct TaskResult
        {
            std::chrono::steady_clock::time_point startTime;
            std::chrono::steady_clock::time_point endTime;
        };

        using TaskResultMap = std::unordered_map<TaskId, TaskResult>;
        using SequenceKeyMap = std::unordered_map<SequenceKey, std::vector<TaskId>>;

        void ensureOrder(TaskId beforeTaskId, TaskId afterTaskId);
        std::function<int(Bloomberg::quantum::VoidContextPtr)> makeTask(TaskId taskId);
        std::function<int(Bloomberg::quantum::VoidContextPtr)> makeTaskWithBlock(TaskId taskId, std::atomic<bool>* blockFlag);
        std::function<int(Bloomberg::quantum::VoidContextPtr)> makeTaskWithException(TaskId taskId, std::string error);
        TaskResultMap& results();
        void sleep(int periodCount = 1);
        void taskFunc(Bloomberg::quantum::VoidContextPtr ctx, TaskId id, std::atomic<bool>* blockFlag, std::string error);
    protected:

    private:

        // Q_DISABLE_COPY_MOVE ( SequencerTestData )
        TaskResultMap m_results;
        Bloomberg::quantum::Mutex m_resultMutex;
    };

}
#endif
