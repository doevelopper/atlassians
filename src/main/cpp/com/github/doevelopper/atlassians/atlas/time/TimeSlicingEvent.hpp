/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     22/12/24 W 22:25
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_ATLAS_TIME_TIMESLICINGEVENT_HPP
#define COM_GITHUB_DOEVELOPPER_ATLAS_TIME_TIMESLICINGEVENT_HPP

#include <com/github/doevelopper/atlassians/atlas/time/AdelsonVelskyLandisNode.hpp>
#include <com/github/doevelopper/atlassians/atlas/time/AdelsonVelskyLandisTree.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    template <typename TimePoint>
    class TimeSlicingEvent;

    /// Instances of this type are passed as the only argument to the user callbacks.
    template <typename TimePoint>
    struct Arg final
    {
        /// Reference to the event that is being executed.
        TimeSlicingEvent<TimePoint>& event;
        /// Time when the event was scheduled to be executed (always in the past).
        TimePoint deadline;
        /// An approximation of the current time such that the following holds: deadline <= approx_now <= Clock::now().
        TimePoint approx_now;
    };


    /// This information is returned by the spin() method to allow the caller to decide what to do next
    /// and assess the temporal performance of the event loop.
    template <typename Clock>
    struct SpinResult final
    {
        /// The deadline of the next event to run (which is never in the past excepting a very short lag),
        /// or time_point::max() if there are no events.
        /// This can be used to let the application sleep when there are no events pending.
        typename Clock::time_point next_deadline;

        /// An approximation of the maximum lateness observed during the spin() call
        /// (the real slack may be worse than the approximation).
        /// This is always non-negative.
        typename Clock::duration worst_lateness;

        /// An approximation of the current time such that (approx_now <= Clock::now()).
        /// This is helpful because the time may be expensive to sample.
        typename Clock::time_point approx_now;
    };

    template <typename TimePoint>
    class TimeSlicingEvent : private AdelsonVelskyLandisNode<TimeSlicingEvent<TimePoint>>
    {
        friend class AdelsonVelskyLandisNode<TimeSlicingEvent>;  // This friendship is required for the AVL tree implementation,
        friend class AdelsonVelskyLandisTree<TimeSlicingEvent>;  // otherwise, we would have to use public inheritance, which is undesirable.
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:
        TimeSlicingEvent(const TimeSlicingEvent&)            = default;
        TimeSlicingEvent& operator=(const TimeSlicingEvent&) = default;
        TimeSlicingEvent& operator=(TimeSlicingEvent&&)      = default;


        /// Cease future activities. No-op if not scheduled.
        /// It is safe to call this method more than once.
        /// It is safe to call this method from the callbacks.
        /// This method is automatically invoked by the destructor.
        void cancel() noexcept
        {
            /// It is guaranteed that while an event resides in the tree, it has a valid deadline set.
            if (m_deadline != TimePoint::min())
            {
                // Removing a non-existent node from the tree is an UB that may lead to memory corruption,
                // so we have to check first if the event is actually registered.
                remove();
                // This is used to mark the event as unregistered so we don't double-remove it.
                // This can only be done after the event is removed from the tree.
                m_deadline = TimePoint::min();
            }
        }


        /// Diagnostic accessor for testing. Not intended for normal use.
        /// The deadline is `TimePoint::min()` if the event is not scheduled or canceled.
        /// It is guaranteed that while an event resides in the tree, it has a valid deadline set.
        [[nodiscard]] TimePoint getDeadline() const noexcept
        {
            return m_deadline;
        }

        // This method is necessary to store an Event in cetl::unbounded_variant
        static constexpr std::array<std::uint8_t, 16> _get_type_id_() noexcept
        {
            return {0xB6, 0x87, 0x48, 0xA6, 0x7A, 0xDB, 0x4D, 0xF1, 0xB3, 0x1D, 0xA9, 0x8D, 0x50, 0xA7, 0x82, 0x47};
        }

    protected:
        using Tree = AdelsonVelskyLandisTree<TimeSlicingEvent>;
        using AdelsonVelskyLandisNode<TimeSlicingEvent>::remove;

        TimeSlicingEvent() noexcept;
        TimeSlicingEvent(TimeSlicingEvent&& rhs) noexcept;
        ~TimeSlicingEvent() noexcept override;

        /// Ensure the event is in the tree and set the deadline to the specified absolute time point.
        /// If the event is already scheduled, the old deadline is overwritten.
        /// It is guaranteed that while an event resides in the tree, it has a valid deadline set.
        void schedule(const TimePoint dead, Tree& tree) noexcept
        {
            cancel();
            m_deadline               = dead;  // The deadline shall be set before the event is inserted into the tree.
            const auto ptr_existing = tree.search(
                [dead](const TimeSlicingEvent& other) {
                    /// No two deadlines compare equal, which allows us to have multiple nodes with the same deadline in
                        /// the tree. With two nodes sharing the same deadline, the one added later is considered to be later.
                    return (dead >= other.m_deadline) ? +1 : -1;
                },
                [this] { return this; });
            assert(std::get<0>(ptr_existing) == this);
            assert(!std::get<1>(ptr_existing));
            (void) ptr_existing;
        }

        /// The execution handler shall either reschedule or cancel the event
        /// (otherwise it will keep firing in a tight loop).
        /// If the event needs to be re-scheduled automatically, it must be done before the user callback is invoked,
        /// because the user may choose to cancel the event from the callback.
        virtual void execute(const Arg<TimePoint>& args, Tree& tree) = 0;
    private:
        TimePoint m_deadline = TimePoint::min();
    };
}
#endif
