
#ifndef COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_COLLECTION_HPP
#define COM_GITHUB_DOEVELOPPER_NIGHT_OWL_OBSERVER_COLLECTION_HPP

#include <atomic>
#include <type_traits>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::night::owl::observer
{
    /*!
     * @BRIEF Thread-safe collection that can store arbitrary items and apply a functor over
     *       them.
     *
     * @details All methods of the collection can be safely called in parallel, from multiple
     *      threads.
     *
     * @warning The order of elements inside the collection is unspecified.
     *
     *
     * @tparam ValueType Type of the elements that will be stored inside the
     *      collection. This type must be at least move constructible.
     */
    template <typename ValueType>
    class Collection final
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
 public:
        /*!
         * @brief Identifier for an element that has been inserted. You can use this id to
         * @note remove a previously inserted element.
         */
        using ID = std::size_t;

        Collection() noexcept;
        Collection(const Collection&) noexcept = delete;
        Collection(Collection&&) noexcept =  default;
        Collection& operator=(const Collection&) noexcept = delete;
        Collection& operator=(Collection&&) noexcept = default;
        virtual ~Collection() noexcept;

        /*!
         * @brief Insert a new element into the collection.
         *
         * @param[in] element The object to be inserted.
         * @tparam ValueTypeInserted Type of the inserted element. Must be convertible to
         *       the collection's ValueType.
         *
         * @return An @ref id that can be used to remove the inserted element.
         *      This @ref id is stable; you can use it after modifying the collection.
         *
         * @note Any apply() call running concurrently with an insert() that has
         *       already called its functor for at least one element, is guaranteed
         *       to not call the functor for this newly inserted element.
         */

        template <typename ValueTypeInserted>
        auto insert(ValueTypeInserted && element);
        auto remove(ID const & element_id) noexcept;
        template <typename UnaryFunctor>
        void apply(UnaryFunctor && fun) const
            noexcept(noexcept(fun(std::declval<ValueType>())));
        auto empty() const noexcept;
    protected:
    private:
        void deleteAnyNodesMarkedAsDeleted() noexcept;

        struct Node
        {
            Node * next { nullptr };
            ValueType element;
            std::atomic<bool> deleted { false };
            ID node_id;
        };

        struct DeleterBlocker
        {
            explicit DeleterBlocker(Collection<ValueType> const * c) noexcept
            : m_collection{ c }
            {
                ++m_collection->m_blockGc;
                while(m_collection->m_isGcActive.load()) ;
            }

            ~DeleterBlocker() noexcept
            {
                --m_collection->m_blockGc;
            }

            DeleterBlocker() =delete;
            DeleterBlocker(DeleterBlocker const &) =delete;
            auto operator=(DeleterBlocker const &) -> DeleterBlocker & =delete;
            DeleterBlocker(DeleterBlocker &&) =default;
            auto operator=(DeleterBlocker &&) -> DeleterBlocker & =default;

        private:
            Collection<ValueType> const * m_collection;
        };

        std::atomic<Node *> m_head { nullptr };
        mutable std::atomic<std::size_t> m_blockGc { 0 };
        std::atomic<bool> m_isGcActive { false };
        std::atomic<ID> m_lastId { 0 };
    };
}
#endif
