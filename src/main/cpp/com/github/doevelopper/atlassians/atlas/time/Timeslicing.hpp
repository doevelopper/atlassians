/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     22/12/24 W 22:16
 * @brief    
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/


#ifndef COM_GITHUB_DOEVELOPPER_ATLAS_TIME_TIMESLICING_HPP
#define COM_GITHUB_DOEVELOPPER_ATLAS_TIME_TIMESLICING_HPP

#include <cassert>
#include <chrono>
#include <concepts>
#include <optional>
#include <utility>
#include <variant>

#include <com/github/doevelopper/atlassians/atlas/time/TimeSlicingEvent.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{

    /// User callbacks shall satisfy this concept.
    template <typename Fun, typename TimePoint>
    concept Callback = std::invocable<Fun, const Arg<TimePoint>&>;

    /// The event loop is used to execute activities at the specified time, either once or periodically.
    /// The event handler callbacks are invoked with one argument of type Arg<Clock::time_point>.
    /// The event loop shall outlive the events it manages.
    /// Each factory method returns an event object by value,
    /// which can be used to cancel the event by destroying it.
    /// The time complexity of all operations is logarithmic of the number of registered events.

    template <typename Clock>
    class Timeslicing final
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
        Q_DISABLE_COPY_MOVE(Timeslicing)
        using time_point = typename Clock::time_point;
        using duration   = typename Clock::duration;
        /// This proxy is needed to expose the protected execute() method to the event loop.
        /// An alternative would be to make this class a friend of the Event class,
        /// or to make the execute() method public, which is undesirable.
        class EventProxy : public TimeSlicingEvent<time_point>
        {
        public:
            using TimeSlicingEvent<time_point>::execute;
            using TimeSlicingEvent<time_point>::_get_type_id_;
        };
    public:

        Timeslicing() noexcept;
        virtual ~Timeslicing() noexcept;

        /// The provided handler will be invoked with the specified interval starting from (now + period);
        /// if you also need to invoke it immediately, consider using defer().
        /// Returns the event handle on success, empty if out of memory or invalid inputs.
        template <Callback<time_point> Fun>
        [[nodiscard]] auto repeat(const duration period, Fun&& handler)
        {
            class Impl final : public EventProxy
            {
            public:
                Impl(Timeslicing& owner, const duration per, Fun&& fun)
                : m_period{per}
                , m_handler{std::move(fun)}
                {
                    this->schedule(Clock::now() + m_period, owner.m_tree);
                }

                using EventProxy::_get_type_id_;

            private:
                void execute(const Arg<time_point>& args, Tree& tree) override
                {
                    this->schedule(args.deadline + m_period, tree);  // Strict period advancement, no phase error growth.
                    handler_(args);
                }

                const duration m_period;
                const Fun      m_handler;
            };
            assert(period > duration::zero());
            return Impl{*this, period, std::forward<Fun>(handler)};
        }

    /// This is like repeat() with one crucial difference: the next deadline is defined not as (deadline+period),
    /// but as (approx_now+period), where (deadline <= approx_now <= now()).
    /// Therefore, the actual period is likely to be greater than requested and the phase error grows unboundedly.
    /// This mode is intended for tasks that do not require strict timing adherence but are potentially time-consuming,
    /// like interface polling or similar. Because the scheduler is allowed to let the phase slip, this type of event
    /// will automatically reduce the activation rate if the scheduler is CPU-starved, thus providing load regulation.
    template <Callback<time_point> Fun>
    [[nodiscard]] auto poll(const duration min_period, Fun&& handler)
    {
        class Impl final : public EventProxy
        {
        public:
            Impl(Timeslicing& owner, const duration per, Fun&& fun) : min_period_{per}, handler_{std::move(fun)}
            {
                this->schedule(Clock::now() + min_period_, owner.m_tree);
            }

            using EventProxy::_get_type_id_;

        private:
            void execute(const Arg<time_point>& args, Tree& tree) override
            {
                this->schedule(args.approx_now + min_period_, tree);  // Accumulate phase error intentionally.
                handler_(args);
            }

            const duration min_period_;
            const Fun      handler_;
        };
        assert(min_period > duration::zero());
        return Impl{*this, min_period, std::forward<Fun>(handler)};
    }

    /// Like repeat() but the handler will be invoked only once and the event is canceled afterward.
    /// The deadline may be in the past, in which case the event will fire ASAP.
    template <Callback<time_point> Fun>
    [[nodiscard]] auto defer(const time_point deadline, Fun&& handler)
    {
        class Impl final : public EventProxy
        {
        public:
            Impl(Timeslicing& owner, const time_point deadline, Fun&& fun) : handler_{std::move(fun)}
            {
                this->schedule(deadline, owner.m_tree);
            }

            using EventProxy::_get_type_id_;

        private:
            void execute(const Arg<time_point>& args, Tree&) override
            {
                this->cancel();
                handler_(args);
            }

            const Fun handler_;
        };
        return Impl{*this, deadline, std::forward<Fun>(handler)};
    }

    /// Execute pending events strictly in the order of their deadlines until there are no pending events left.
    /// This method should be invoked regularly to pump the event loop.
    /// The method is optimized to make as few calls to Clock::now() as possible, as they may be expensive.
    ///
    /// The execution monitor is constructed when the event execution is commenced and destroyed when it is finished;
    /// it can be used to drive a test pad or some other timing fixture.
    template <typename ExecutionMonitor = std::monostate>
    [[nodiscard]] SpinResult<Clock> spin()
    {
        SpinResult<Clock> result{.next_deadline  = time_point::max(),
                                 .worst_lateness = duration::zero(),
                                 .approx_now     = time_point::min()};
        if (m_tree.empty()) [[unlikely]]
        {
            result.approx_now = Clock::now();
            return result;
        }

        while (auto* const evt = static_cast<EventProxy*>(m_tree.min()))
        {
            // The deadline is guaranteed to be set because it is in the tree.
            const auto deadline = evt->getDeadline();
            if (result.approx_now < deadline)  // Too early -- either we need to sleep or the time sample is obsolete.
            {
                result.approx_now = Clock::now();  // The number of calls to Clock::now() is minimized.
                if (result.approx_now < deadline)  // Nope, even with the latest time sample we are still early -- exit.
                {
                    result.next_deadline = deadline;
                    break;
                }
            }
            {
                ExecutionMonitor monitor{};  // RAII indication of the start and end of the event execution.
                // Execution will remove the event from the tree and then possibly re-insert it with a new deadline.
                evt->execute({.event = *evt, .deadline = deadline, .approx_now = result.approx_now}, m_tree);
                (void) monitor;
            }
            result.next_deadline  = time_point::max();  // Reset the next deadline to the maximum possible value.
            result.worst_lateness = std::max(result.worst_lateness, result.approx_now - deadline);
        }

        assert(result.approx_now > time_point::min());
        assert(result.worst_lateness >= duration::zero());
        return result;
    }

    /// True if there are no registered events.
    [[nodiscard]] bool isEmpty() const noexcept
    {
        return m_tree.empty();
    }

    /// This intrusive accessor is only needed for testing and advanced diagnostics. Not intended for normal use.
    /// The nodes are ordered such that the earliest deadline is on the left.
    const auto& getTree() const noexcept
    {
        return m_tree;
    }
    protected:
    private:
        using Tree = AdelsonVelskyLandisTree<TimeSlicingEvent<time_point>>;
        Tree m_tree;
    };
}
#endif
