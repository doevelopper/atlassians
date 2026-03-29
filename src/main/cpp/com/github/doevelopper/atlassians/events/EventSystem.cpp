
#include <com/github/doevelopper/atlassians/events/EventSystem.hpp>

using namespace com::github::doevelopper::atlassians::events;

log4cxx::LoggerPtr EventSystem::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.premisses.events.EventSystem"));

std::unordered_map<std::string, std::vector<EventSystem::EventHandler*> > EventSystem::s_handlers;
std::mutex EventSystem::s_mutex;

EventSystem::EventSystem()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

EventSystem::~EventSystem()
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
}

void EventSystem::registerEventHandler(std::string eventId, EventHandler* handler)
{

    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    if (s_handlers.count(eventId) == 0) {
        s_handlers.insert(std::make_pair(eventId, std::vector<EventHandler*>()));
    }

    std::vector<EventHandler*>& handlers = s_handlers.at(eventId);
    s_mutex.lock();
    handlers.push_back(handler);
    s_mutex.unlock();
}

void EventSystem::unregisterEventHandler(std::string eventId, EventHandler* handler)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);

    std::vector<EventHandler*>& handlers = s_handlers.at(eventId);
    s_mutex.lock();
    LOG4CXX_WARN(logger, "FIXME/ Unregistering event handler for event [" << eventId << "].");
    // handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());//cannot convert ‘std::vector<std::function<void()>*>::iterator’ to ‘const char*’
    s_mutex.unlock();
}
