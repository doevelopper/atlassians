
#include <com/github/doevelopper/atlassians/logging/Demangler.hpp>

using namespace com::github::doevelopper::atlassians::logging;

log4cxx::LoggerPtr Demangler::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.logging.Demangler"));

Demangler::Demangler() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

Demangler::~Demangler() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

template <typename T>
std::string Demangler::typeName()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    using TR = typename std::remove_reference<T>::type;

    std::unique_ptr<char, void (*)(void *)> own(
        abi::__cxa_demangle(typeid(TR).name(), nullptr, nullptr, nullptr), free);

    std::string r = own != nullptr ? own.get() : typeid(TR).name();

    if (std::is_const<TR>::value)
        r += " const";
    if (std::is_volatile<TR>::value)
        r += " volatile";
    if (std::is_lvalue_reference<T>::value)
        r += "&";
    else if (std::is_rvalue_reference<T>::value)
        r += "&&";
    return r;
}

template <class T>
std::string Demangler::typeName(T & val)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    return this->typeName<decltype(val)>();
}
