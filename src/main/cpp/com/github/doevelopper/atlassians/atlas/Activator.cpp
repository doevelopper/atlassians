
#include <com/github/doevelopper/atlassians/atlas/Activator.hpp>

using namespace com::github::doevelopper::atlassians::atlas::utils;

template < class T >
log4cxx::LoggerPtr Activator< T >::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.utils.Activator"));

template < class T >
Activator< T >::Activator() : m_value(T())
    , m_isValid(false)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template < class T >
Activator< T >::~Activator()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template < class T >
void Activator< T >::value(const T &value)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_value = value;
    this->m_isValid = true;
}

template < class T >
T Activator< T >::value() const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return (this->m_value);
}

template < class T >
const bool Activator< T >::isValid() const
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return (this->m_isValid);
}

template < class T >
void Activator< T >::isValid(bool value)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    this->m_isValid = value;
}
