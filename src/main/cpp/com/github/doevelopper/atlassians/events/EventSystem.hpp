
#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_EVENTSYSTEM_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_EVENTSYSTEM_HPP

#include <unordered_map>
#include <string>
#include <functional>
#include <mutex>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::events
{

    class EventSystem
    {

    LOG4CXX_DECLARE_STATIC_LOGGER

    public:

        using EventHandler = std::function<void ()>;

        EventSystem();
        virtual ~EventSystem();

        static void registerEventHandler(std::string event_id, EventHandler* handler);
        static void unregisterEventHandler(std::string event_id, EventHandler* handler);
        static void reportEvent(std::string event_id);
        template <typename ... _args>
        static void raiseEvent(std::string event_id, _args&&... args);

    protected:

    private:

        Q_DEFAULT_COPY_MOVE(EventSystem)
        static std::unordered_map<std::string, std::vector<EventHandler*> > s_handlers;
        static std::mutex s_mutex;
    };

}
#endif // COM_GITHUB_DOEVELOPPER_PREMISSES_EVENTS_EVENTSYSTEM_HPP
