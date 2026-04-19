
#include <com/github/doevelopper/atlassians/atlas/utils/SignalHandler.hpp>

using namespace com::github::doevelopper::atlassians::atlas::utils;

log4cxx::LoggerPtr SignalHandler::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.atlas.utils.SignalHandler"));

SignalHandler::SignalHandler() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

SignalHandler::~SignalHandler() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

SignalHandler::SignalHandler(const std::string & moduleName, const SignalHandlerCallBack & exitHandler) noexcept
    // : m_moduleName(moduleName)
    // , m_exitHandler(exitHandler)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

boost::asio::io_context & SignalHandler::io() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void SignalHandler::run()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    // LOG4CXX_DEBUG_FMT(logger, "[{}] Signal svc start.", m_moduleName);
    // boost::system::error_code ec;
    // const auto num = m_io.run(ec);
    // if (ec)
    // {
    //     LOG4CXX_ERROR_FMT("[{}] IO context run failed. [errno = {}, errmsg = {}]", m_moduleName, ec.value(), ec.message());
    //     return;
    // }
    // LOG4CXX_DEBUG_FMT("[{}] IO context run successful. [errno = {}, errmsg = {}]", m_moduleName, ec.value(), ec.message());
}

void SignalHandler::setDefaultSignalHandler(const SignalHandlerCallBack & value)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void SignalHandler::registSignal()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void SignalHandler::handleSignal(const boost::system::error_code & e, int signalNumber)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}
