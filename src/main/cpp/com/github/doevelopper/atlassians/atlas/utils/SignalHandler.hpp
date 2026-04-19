#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_SIGNALHANDLER_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_UTILS_SIGNALHANDLER_HPP

#include <sys/types.h>
#include <sys/wait.h>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

namespace com::github::doevelopper::atlassians::atlas::utils
{
    class SignalHandler
    {
        using SignalHandlerCallBack = std::function<void(const boost::system::error_code&, int)>;
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        SignalHandler() noexcept;
        SignalHandler(const SignalHandler &)             = default;
        SignalHandler(SignalHandler &&)                  = default;
        SignalHandler & operator=(const SignalHandler &) = default;
        SignalHandler & operator=(SignalHandler &&)      = default;
        virtual ~SignalHandler() noexcept;

        SignalHandler(const std::string& moduleName,
                const SignalHandlerCallBack& exitHandler) noexcept;
        boost::asio::io_context& io() noexcept;
        void run();
        void setDefaultSignalHandler(const SignalHandlerCallBack& value);
    protected:
    private:
        void registSignal();
        void handleSignal(const boost::system::error_code& e, int signalNumber);

        // boost::asio::io_context m_io;
        // boost::asio::signal_set m_signalGroup;
        //
        // const std::string m_moduleName;
        // SignalHandlerCallBack m_exitHandler{nullptr};
        // SignalHandlerCallBack m_defaultSignalHandler{nullptr};
    };
} // namespace com::github::doevelopper::atlassians::atlas::utils
#endif
