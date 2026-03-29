#include <com/github/doevelopper/atlassians/atlas/time/TimeFixture.hpp>

using namespace com::github::doevelopper::atlassians::atlas::time;

log4cxx::LoggerPtr TimeFixture::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.time.TimeFixture"));

unsigned TimeFixture::m_timerId;

// TimeFixture::TimeFixture() noexcept
// {
//     LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
// }

TimeFixture::~TimeFixture() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    std::chrono::time_point< std::chrono::system_clock > end = std::chrono::system_clock::now();
    std::chrono::duration< double > elapsed_seconds = end - m_start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    char mbstr[100];
    std::strftime(mbstr, sizeof(mbstr), "%X", std::localtime(&end_time));

    m_timeInformation << "TIMER END [" << m_number << "] " << m_name << " -  completed @ " << mbstr
                      << " elapsed time: " << elapsed_seconds.count() << "s";
    if (m_unit)
    {
        double per_unit = elapsed_seconds.count() / m_unit;
        m_timeInformation << " @" << per_unit << " s/cycle freq: " << 1. / per_unit << "/s";
    }
    LOG4CXX_DEBUG(logger, m_timeInformation.str());
}

TimeFixture::TimeFixture(const char *name, std::size_t unit)
  : m_start(std::chrono::system_clock::now())
  , m_name(name)
  , m_number(m_timerId++)
  , m_unit(unit)
  , m_timeInformation()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
    m_timeInformation << "TIMER START [" << m_number << "] - " << m_name;
}


TimeFixture::TimepointT TimeFixture::nowPoint() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  return (std::chrono::system_clock::now());
}

const TimeFixture::TimepointT TimeFixture::startPoint() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  return (m_start);
}

template <typename T>
const T TimeFixture::seconds() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - startPoint();
  return std::chrono::duration_cast< std::chrono::duration< T, std::ratio< 1, 1 > > >(duration).count();
}

template <typename T>
const T TimeFixture::milliSeconds() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - startPoint();
  return std::chrono::duration_cast< std::chrono::duration< T, std::milli > >(duration).count();
}

template <typename T>
const T TimeFixture::microSeconds() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - startPoint();
  return std::chrono::duration_cast< std::chrono::duration< T, std::micro > >(duration).count();
}

template <typename T>
const T TimeFixture::nanoSeconds() const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - startPoint();
  return std::chrono::duration_cast< std::chrono::duration< T, std::nano > >(duration).count();
}

template <typename T>
const T TimeFixture::seconds(const TimepointT &endPoint) const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - endPoint;
  return std::chrono::duration_cast< std::chrono::duration< T, std::ratio< 1, 1 > > >(duration).count();
}

template <typename T>
const T TimeFixture::milliSeconds(const TimepointT &endPoint) const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - endPoint;
  return std::chrono::duration_cast< std::chrono::duration< T, std::milli > >(duration).count();
}

template <typename T>
const T TimeFixture::microSseconds(const TimepointT &endPoint) const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - endPoint;
  return std::chrono::duration_cast< std::chrono::duration< T, std::micro > >(duration).count();
}

template <typename T>
const T TimeFixture::nanoSeconds(const TimepointT &endPoint) const
{
  LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
  auto duration = nowPoint() - endPoint;
  return std::chrono::duration_cast< std::chrono::duration< T, std::nano > >(duration).count();
}
