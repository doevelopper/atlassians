/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     22/12/24 W 21:22
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ADELSONVELSKYLANDISTREE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ADELSONVELSKYLANDISTREE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

#include <com/github/doevelopper/atlassians/atlas/time/AdelsonVelskyLandisNode.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    template <typename Derived>
    class AdelsonVelskyLandisTree final
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
        Q_DISABLE_COPY(AdelsonVelskyLandisTree)
    public:
        /// Helper alias of the compatible node type.
        using NodeType    = AdelsonVelskyLandisNode<Derived>;
        using DerivedType = Derived;
        AdelsonVelskyLandisTree() noexcept;

        AdelsonVelskyLandisTree(AdelsonVelskyLandisTree&& rhs) noexcept
            : m_origin_node{std::move(rhs.m_origin_node)}
        {
            //LOG4CXX_ASSERT(!m_traversal_in_progress);        // Cannot modify the tree while it is being traversed.
            //LOG4CXX_ASSERT(!rhs.m_traversal_in_progress);  // Cannot modify the tree while it is being traversed.
        }

        AdelsonVelskyLandisTree& operator=(AdelsonVelskyLandisTree&& rhs) noexcept
        {
            //LOG4CXX_ASSERT(!m_traversal_in_progress);        // Cannot modify the tree while it is being traversed.
            //LOG4CXX_ASSERT(!other.m_traversal_in_progress);  // Cannot modify the tree while it is being traversed.

            m_origin_node = std::move(rhs.m_origin_node);
            return *this;
        }
        virtual ~AdelsonVelskyLandisTree() noexcept;


    /// Wraps NodeType<>::search().
    template <typename Pre>
    auto search(const Pre& predicate) noexcept -> Derived*
    {
        return NodeType::template search<Pre>(getRootNode(), predicate);
    }
    template <typename Pre>
    auto search(const Pre& predicate) const noexcept -> const Derived*
    {
        return NodeType::template search<Pre>(getRootNode(), predicate);
    }
    template <typename Pre, typename Fac>
    auto search(const Pre& predicate, const Fac& factory) -> std::tuple<Derived*, bool>
    {
        //CAVL_ASSERT(!m_traversal_in_progress);  // Cannot modify the tree while it is being traversed.
        return NodeType::template search<Pre, Fac>(m_origin_node, predicate, factory);
    }

    /// The function has no effect if the node pointer is nullptr, or node is not in the tree (aka unlinked).
    /// It is safe to pass the result of search() directly as the node argument:
    ///
    ///     tree.remove(Node<T>::search(root, search_predicate));
    ///
    /// but it could be done also by direct node removal:
    ///
    ///     if (auto* const node = Node<T>::search(root, search_predicate) {
    ///         node->remove();
    ///     }
    ///
    /// Wraps NodeType<>::remove().
    ///
    /// No Sonar cpp:S6936 b/c the `remove` method name isolated inside `Tree` type (doesn't conflict with C).
    void remove(NodeType* const node) noexcept  // NOSONAR cpp:S6936
    {
        //LOG4CXX_ASSERT(!m_traversal_in_progress);  // Cannot modify the tree while it is being traversed.
        if ((node != nullptr) && node->isLinked())
        {
            node->remove();
        }
    }

    /// Wraps NodeType<>::min/max().
    auto min() noexcept -> Derived*
    {
        return NodeType::min(*this);
    }
    auto max() noexcept -> Derived*
    {
        return NodeType::max(*this);
    }
    auto min() const noexcept -> const Derived*
    {
        return NodeType::min(*this);
    }
    auto max() const noexcept -> const Derived*
    {
        return NodeType::max(*this);
    }

    /// Wraps NodeType<>::traverseInOrder().
    template <typename Vis>
    auto traverseInOrder(const Vis& visitor, const bool reverse = false)
    {
        const TraversalIndicatorUpdater upd(*this);
        return NodeType::template traverseInOrder<Vis>(*this, visitor, reverse);
    }
    template <typename Vis>
    auto traverseInOrder(const Vis& visitor, const bool reverse = false) const
    {
        const TraversalIndicatorUpdater upd(*this);
        return NodeType::template traverseInOrder<Vis>(*this, visitor, reverse);
    }

    /// Wraps NodeType<>::traversePostOrder().
    template <typename Vis>
    void traversePostOrder(const Vis& visitor, const bool reverse = false)
    {
        const TraversalIndicatorUpdater upd(*this);
        NodeType::template traversePostOrder<Vis>(*this, visitor, reverse);
    }
    template <typename Vis>
    void traversePostOrder(const Vis& visitor, const bool reverse = false) const
    {
        const TraversalIndicatorUpdater upd(*this);
        NodeType::template traversePostOrder<Vis>(*this, visitor, reverse);
    }

    /// Normally these are not needed except if advanced introspection is desired.
    ///
    /// No linting and Sonar cpp:S1709 b/c implicit conversion by design.
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    operator Derived*() noexcept  // NOSONAR cpp:S1709
    {
        return getRootNode();
    }
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    operator const Derived*() const noexcept  // NOSONAR cpp:S1709
    {
        return getRootNode();
    }

    /// Access i-th element of the tree in linear time. Returns nullptr if the index is out of bounds.
    auto operator[](const std::size_t index) -> Derived*
    {
        std::size_t i = index;
        // No Sonar cpp:S881 b/c this decrement is pretty much straightforward - no maintenance concerns.
        return traverseInOrder([&i](auto& x) { return (i-- == 0) ? &x : nullptr; });  // NOSONAR cpp:S881
    }
    auto operator[](const std::size_t index) const -> const Derived*
    {
        std::size_t i = index;
        // No Sonar cpp:S881 b/c this decrement is pretty much straightforward - no maintenance concerns.
        return traverseInOrder([&i](const auto& x) { return (i-- == 0) ? &x : nullptr; });  // NOSONAR cpp:S881
    }

    /// Beware that this convenience method has linear complexity. Use responsibly.
    auto size() const noexcept
    {
        auto i = 0UL;
        traverseInOrder([&i](auto& /*unused*/) { i++; });
        return i;
    }

    /// Unlike size(), this one is constant-complexity.
    auto empty() const noexcept { return getRootNode() == nullptr; }

    protected:
    private:
        static_assert(!std::is_polymorphic<NodeType>::value, "Internal check: The node type must not be a polymorphic type");
        static_assert(std::is_same<AdelsonVelskyLandisTree<Derived>, typename NodeType::TreeType>::value, "Internal check: Bad type alias");

        class TraversalIndicatorUpdater final
        {
            Q_DISABLE_COPY_MOVE(TraversalIndicatorUpdater)
        public:
            explicit TraversalIndicatorUpdater(const AdelsonVelskyLandisTree& sup) noexcept
                : that(sup)
            {
                that.m_traversal_in_progress = true;
            }
            ~TraversalIndicatorUpdater() noexcept
            {
                that.m_traversal_in_progress = false;
            }


        private:
            const AdelsonVelskyLandisTree& that;
        };

        // root node pointer is stored in the origin_node_ left child.
        auto getRootNode() noexcept -> Derived*
        {
            return m_origin_node.getChildNode(false);
        }

        auto getRootNode() const noexcept -> const Derived*
        {
            return m_origin_node.getChildNode(false);
        }

        AdelsonVelskyLandisNode<Derived> m_origin_node{};
        // No Sonar cpp:S4963 b/c of implicit modification by the `TraversalIndicatorUpdater` RAII class,
        // even for `const` instance of the `Tree` class (hence the `mutable volatile` keywords).
        mutable volatile bool m_traversal_in_progress = false;
    };
}

#endif
