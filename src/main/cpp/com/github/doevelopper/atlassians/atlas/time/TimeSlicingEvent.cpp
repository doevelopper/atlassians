#include <com/github/doevelopper/atlassians/atlas/time/TimeSlicingEvent.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

template <typename TimePoint>
log4cxx::LoggerPtr TimeSlicingEvent<TimePoint>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.atlas.time.TimeSlicingEvent"));

template <typename TimePoint>
TimeSlicingEvent<TimePoint>::TimeSlicingEvent() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename TimePoint>
TimeSlicingEvent<TimePoint>::TimeSlicingEvent(TimeSlicingEvent&& rhs) noexcept
    : AdelsonVelskyLandisNode<TimeSlicingEvent>{std::move(static_cast<AdelsonVelskyLandisNode<TimeSlicingEvent>&>(rhs))}
    , m_deadline{std::exchange(rhs.m_deadline, TimePoint::min())}
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename TimePoint>
TimeSlicingEvent<TimePoint>::~TimeSlicingEvent() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    cancel();
    // assert(m_deadline == TimePoint::min());
    // assert((this->getChildNode(false) == nullptr)
    //     && (this->getChildNode(true) == nullptr)
    //     && (this->getParentNode() == nullptr));

}
