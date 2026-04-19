
#include <com/github/doevelopper/atlassians/events/observer/Collection.hpp>

#include <atomic>
using namespace com::github::doevelopper::night::owl::observer;

template <typename ValueType>
log4cxx::LoggerPtr Collection<ValueType>::logger =
    log4cxx::Logger::getLogger(std::string("com.github.doevelopper.night.owl.observer.Collection"));

template <typename ValueType>
Collection<ValueType>::Collection() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__ );
}

template <typename ValueType>
Collection<ValueType>::~Collection() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    for(auto n = this->m_head.load(); n; n = n->next)
        n->deleted.store(true);
    this->deleteAnyNodesMarkedAsDeleted();
}

template <typename ValueType>
template <typename ValueTypeInserted>
auto Collection<ValueType>::insert(ValueTypeInserted && element)
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const i = ++m_lastId;
    auto n       = std::make_unique<Node>();
    n->node_id   = i;
    n->element   = std::forward<ValueTypeInserted>(element);
    {
        auto const deleterBlocker = DeleterBlocker {this};
        n->next                   = this->m_head.load();
        while (!this->m_head.compare_exchange_weak(n->next, n.get()))
            ;
        n.release();
    }
    this->deleteAnyNodesMarkedAsDeleted();
    return i;
}

template <typename ValueType>
auto Collection<ValueType>::remove(ID const & element_id) noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto deleted = false;
    {
        auto const deleterBlocker = DeleterBlocker {this};

        for (auto n = this->m_head.load(); n; n = n->next)
        {
            if (n->node_id != element_id)
                continue;

            deleted = !n->deleted.exchange(true);
            break;
        }
    }

    this->deleteAnyNodesMarkedAsDeleted();
    return deleted;
}

template <typename ValueType>
template <typename UnaryFunctor>
void Collection<ValueType>::apply(UnaryFunctor && fun) const noexcept(noexcept(fun(std::declval<ValueType>())))
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const deleterBlocker = DeleterBlocker {this};

    for (auto n = this->m_head.load(); n; n = n->next)
    {
        if (n->deleted.load())
            continue;

        fun(n->element);
    }
}

template <typename ValueType>
auto Collection<ValueType>::empty() const noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    auto const deleterBlocker = DeleterBlocker {this};
    auto const h = this->m_head.load();
    return !h || h->deleted;
}

template <typename ValueType>
void Collection<ValueType>::deleteAnyNodesMarkedAsDeleted() noexcept
{
    LOG4CXX_TRACE(logger, __LOG4CXX_FUNC__);
    if(this->m_blockGc.load() >0 || this->m_isGcActive.exchange(true))
    {
        LOG4CXX_WARN(logger, "Not market for deletion");
        return;
    }

    auto head = this->m_head.load();

    for(auto p = &head; *p && this->m_blockGc == 0;)
    {
        if(!(*p)->deleted.load())
        {
            p = &((*p)->next);
            continue;
        }

        auto d = *p;
        *p = (*p)->next;
        delete d;

        if(p == &head)
            this->m_head.store(*p);
    }

    this->m_isGcActive.store(false);
}
