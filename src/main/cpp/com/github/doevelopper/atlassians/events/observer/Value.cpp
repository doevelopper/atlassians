
#include <com/github/doevelopper/night/owl/observer/Value.hpp>
using namespace com::github::doevelopper::night::owl::observer;

template <typename ValueType>
log4cxx::LoggerPtr Value<ValueType>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.Value"));

template <typename ValueType>
Value<ValueType>::Value() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename ValueType>
Value<ValueType>::~Value() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

