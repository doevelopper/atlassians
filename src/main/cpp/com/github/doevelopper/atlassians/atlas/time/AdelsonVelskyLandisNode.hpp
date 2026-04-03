/*******************************************************************
 * @file
 * @version  v0.0.
 * @date     22/12/24 W 20:56
 * @brief
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2024 ACME Technology Corp. All rights reserved.
********************************************************************/

#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ADELSONVELSKYLANDISNODE_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_TIME_ADELSONVELSKYLANDISNODE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <tuple>
#include <type_traits>
#include <utility>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::time
{
    template <typename Derived>
    class AdelsonVelskyLandisTree;

    template <typename Derived>
    class AdelsonVelskyLandisNode
    {
        LOG4CXX_DECLARE_STATIC_LOGGER

        template <typename F, typename... Args>
        using invoke_result = std::invoke_result_t<F, Args...>;
        // Tree nodes cannot be copied for obvious reasons.
        Q_DISABLE_COPY(AdelsonVelskyLandisNode)
    public:

        using TreeType    = AdelsonVelskyLandisTree<Derived>;
        using DerivedType = Derived;



        AdelsonVelskyLandisNode(AdelsonVelskyLandisNode&&) = default;
        AdelsonVelskyLandisNode& operator=(AdelsonVelskyLandisNode&&) = default;

        bool isLinked() const noexcept
        {
            return nullptr != up;
        }
        bool isRoot() const noexcept
        {
            return isLinked() && (!up->isLinked());
        }
        auto getParentNode() noexcept -> Derived*
        {
            return isRoot() ? nullptr : down(up);
        }
        auto getParentNode() const noexcept -> const Derived*
        {
            return isRoot() ? nullptr : down(up);
        }
        auto getChildNode(const bool right) noexcept -> Derived*
        {
            return down(lr[right]);
        }
        auto getChildNode(const bool right) const noexcept -> const Derived*
        {
            return down(lr[right]);
        }
        auto getBalanceFactor() const noexcept
        {
            return bf;
        }
        auto getNextInOrderNode(const bool reverse = false) noexcept -> Derived*
        {
            return getNextInOrderNodeImpl<Derived>(this, reverse);
        }
        auto getNextInOrderNode(const bool reverse = false) const noexcept -> const Derived*
        {
            return getNextInOrderNodeImpl<const Derived>(this, reverse);
        }


        /// Find a node for which the predicate returns zero, or nullptr if there is no such node or the tree is empty.
        /// The predicate is invoked with a single argument which is a constant reference to Derived.
        /// The predicate returns POSITIVE if the search target is GREATER than the provided node, negative if smaller.
        /// The predicate should be noexcept.
        template <typename Pre>
        static auto search(AdelsonVelskyLandisNode* const root, const Pre& predicate) noexcept -> Derived*
        {
            return searchImpl<Derived>(root, predicate);
        }
        template <typename Pre>
        static auto search(const AdelsonVelskyLandisNode* const root, const Pre& predicate) noexcept -> const Derived*
        {
            return searchImpl<const Derived>(root, predicate);
        }

        /// This is like the regular search function except that if the node is missing, the factory will be invoked
        /// (without arguments) to construct a new one and insert it into the tree immediately.
        /// The root node (inside the origin) may be replaced in the process.
        /// If this method returns true, the tree is not modified;
        /// otherwise, the factory was (successfully!) invoked and a new node has been inserted into the tree.
        /// The factory does not need to be noexcept (may throw). It may also return nullptr to indicate intentional
        /// refusal to modify the tree, or f.e. in case of out of memory - result will be `(nullptr, true)` tuple.
        template <typename Pre, typename Fac>
        static auto search(AdelsonVelskyLandisNode& origin, const Pre& predicate, const Fac& factory) -> std::tuple<Derived*, bool>;

        /// Remove the specified node from its tree.
        ///
        /// The root node (inside the origin) may be replaced in the process.
        /// No Sonar cpp:S6936 b/c the `remove` method name isolated inside `Node` type (doesn't conflict with C).
        void remove() const noexcept  // NOSONAR cpp:S6936
        {
            removeImpl(this);
        }

        /// This is like the const overload of `remove()`
        /// except that the node pointers are invalidated afterward for safety.
        ///
        /// No Sonar cpp:S6936 b/c the `remove` method name isolated inside `Node` type (doesn't conflict with C).
        void remove() noexcept  // NOSONAR cpp:S6936
        {
            removeImpl(this);
            unlink();
        }

        /// These methods provide very fast retrieval of min/max values, either const or mutable.
        /// They return nullptr iff the tree is empty.
        static auto min(AdelsonVelskyLandisNode* const root) noexcept -> Derived*
        {
            return extremum(root, false);
        }
        static auto max(AdelsonVelskyLandisNode* const root) noexcept -> Derived*
        {
            return extremum(root, true);
        }
        static auto min(const AdelsonVelskyLandisNode* const root) noexcept -> const Derived*
        {
            return extremum(root, false);
        }
        static auto max(const AdelsonVelskyLandisNode* const root) noexcept -> const Derived*
        {
            return extremum(root, true);
        }

    /// In-order or reverse-in-order traversal of the tree; the visitor is invoked with a reference to each node.
    /// If the return type is non-void, then it shall be default-constructable and convertible to bool; in this case,
    /// traversal will stop when the first true value is returned, which is propagated back to the caller; if none
    /// of the calls returned true or the tree is empty, a default value is constructed and returned.
    /// The tree shall not be modified while traversal is in progress, otherwise bad memory access will likely occur.
    template <typename Vis, typename R = invoke_result<Vis, Derived&>>
    static auto traverseInOrder(Derived* const root, const Vis& visitor, const bool reverse = false)  //
        -> std::enable_if_t<!std::is_void<R>::value, R>
    {
        return traverseInOrderImpl<R, AdelsonVelskyLandisNode>(root, visitor, reverse);
    }
    template <typename Vis>
    static auto traverseInOrder(Derived* const root, const Vis& visitor, const bool reverse = false)  //
        -> std::enable_if_t<std::is_void<invoke_result<Vis, Derived&>>::value>
    {
        traverseInOrderImpl<AdelsonVelskyLandisNode>(root, visitor, reverse);
    }
    template <typename Vis, typename R = invoke_result<Vis, const Derived&>>
    static auto traverseInOrder(const Derived* const root, const Vis& visitor, const bool reverse = false)  //
        -> std::enable_if_t<!std::is_void<R>::value, R>
    {
        return traverseInOrderImpl<R, const AdelsonVelskyLandisNode>(root, visitor, reverse);
    }
    template <typename Vis>
    static auto traverseInOrder(const Derived* const root, const Vis& visitor, const bool reverse = false)  //
        -> std::enable_if_t<std::is_void<invoke_result<Vis, const Derived&>>::value>
    {
        traverseInOrderImpl<const AdelsonVelskyLandisNode>(root, visitor, reverse);
    }

    /// @breaf Post-order (or reverse-post-order) traversal of the tree.
    ///
    /// "Post" nature of the traversal guarantees that, once a node reference is passed to the visitor,
    /// traversal won't use or reference this node anymore, so it is safe to modify the node in the visitor -
    /// f.e. deallocate node's memory for an efficient "release whole tree" scenario. But the tree itself
    /// shall not be modified while traversal is in progress, otherwise bad memory access will likely occur.
    ///
    /// @param root The root node of the tree to traverse.
    /// @param visitor The callable object to invoke for each node. The visitor is invoked with a reference
    ///                to each node as a POST-action call, AFTER visiting all of its children.
    /// @param reverse If `false`, the traversal visits first "left" children, then "right" children.
    ///                If `true`, the traversal is performed in reverse order ("right" first, then "left").
    ///                In either case, the current node is visited last (hence the post-order).
    ///
    template <typename Vis>
    static void traversePostOrder(Derived* const root, const Vis& visitor, const bool reverse = false)
    {
        traversePostOrderImpl<AdelsonVelskyLandisNode>(root, visitor, reverse);
    }

    template <typename Vis>
    static void traversePostOrder(const Derived* const root, const Vis& visitor, const bool reverse = false)
    {
        traversePostOrderImpl<const AdelsonVelskyLandisNode>(root, visitor, reverse);
    }

    protected:
        AdelsonVelskyLandisNode() noexcept;
        virtual ~AdelsonVelskyLandisNode() noexcept;
    private:

        void moveFrom(AdelsonVelskyLandisNode& other) noexcept
        {
            //LOG4CXX_ASSERT(!isLinked());  // Should not be part of any tree yet.

            up    = other.up;
            lr[0] = other.lr[0];
            lr[1] = other.lr[1];
            bf    = other.bf;
            other.unlink();

            if (nullptr != up)
            {
                up->lr[up->lr[1] == &other] = this;
            }
            if (nullptr != lr[0])
            {
                lr[0]->up = this;
            }
            if (nullptr != lr[1])
            {
                lr[1]->up = this;
            }
        }


        void rotate(const bool r) noexcept
        {
            LOG4CXX_ASSERT(logger, isLinked(), " ");
            // LOG4CXX_ASSERT((lr[!r] != nullptr) && ((bf >= -1) && (bf <= +1)));
            AdelsonVelskyLandisNode* const z             = lr[!r];
            up->lr[up->lr[1] == this] = z;
            z->up                     = up;
            up                        = z;
            lr[!r]                    = z->lr[r];
            if (lr[!r] != nullptr)
            {
                lr[!r]->up = this;
            }
            z->lr[r] = this;
        }

        auto adjustBalance(const bool increment) noexcept -> AdelsonVelskyLandisNode*;
        auto retraceOnGrowth() noexcept -> AdelsonVelskyLandisNode*;

        template <typename NodeT, typename DerivedT, typename Vis>
        static void traverseInOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse);
        template <typename Result, typename NodeT, typename DerivedT, typename Vis>
        static auto traverseInOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse) -> Result;

        template <typename NodeT, typename DerivedT, typename Vis>
        static void traversePostOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse);

        static void removeImpl(const AdelsonVelskyLandisNode* const node) noexcept;


        template <typename DerivedT, typename NodeT, typename Pre>
        static auto searchImpl(NodeT* const root, const Pre& predicate) noexcept -> DerivedT*
        {
            NodeT* n = root;
            while (n != nullptr)
            {
                //LOG4CXX_ASSERT(nullptr != n->up);

                DerivedT* const derived = down(n);
                const auto      cmp     = predicate(*derived);
                if (0 == cmp)
                {
                    return derived;
                }
                n = n->lr[cmp > 0];
            }
            return nullptr;
        }

        template <typename DerivedT, typename NodeT>
        static auto getNextInOrderNodeImpl(NodeT* const node, const bool reverse) noexcept -> DerivedT*
        {
            if (nullptr != node->lr[!reverse])
            {
                auto* next_down = node->lr[!reverse];
                while (nullptr != next_down->lr[reverse])
                {
                    next_down = next_down->lr[reverse];
                }
                return down(next_down);
            }

            auto*  next_up_child = node;
            NodeT* next_up       = node->getParentNode();
            while ((nullptr != next_up) && (next_up_child == next_up->lr[!reverse]))
            {
                next_up_child = next_up;
                next_up       = next_up->getParentNode();
            }
            return down(next_up);
        }

        void unlink() noexcept
        {
            up    = nullptr;
            lr[0] = nullptr;
            lr[1] = nullptr;
            bf    = 0;
        }

        static auto extremum(AdelsonVelskyLandisNode* const root, const bool maximum) noexcept -> Derived*
        {
            AdelsonVelskyLandisNode* result = nullptr;
            AdelsonVelskyLandisNode* c      = root;
            while (c != nullptr)
            {
                result = c;
                c      = c->lr[maximum];
            }
            return down(result);
        }
        static auto extremum(const AdelsonVelskyLandisNode* const root, const bool maximum) noexcept -> const Derived*
        {
            const AdelsonVelskyLandisNode* result = nullptr;
            const AdelsonVelskyLandisNode* c      = root;
            while (c != nullptr)
            {
                result = c;
                c      = c->lr[maximum];
            }
            return down(result);
        }

        // This is MISRA-compliant as long as we are not polymorphic. The derived class may be polymorphic though.
        static auto down(AdelsonVelskyLandisNode* x) noexcept -> Derived*
        {
            return static_cast<Derived*>(x);
        }
        static auto down(const AdelsonVelskyLandisNode* x) noexcept -> const Derived*
        {
            return static_cast<const Derived*>(x);
        }

        friend class AdelsonVelskyLandisTree<Derived>;

        AdelsonVelskyLandisNode*                up = nullptr;
        std::array<AdelsonVelskyLandisNode*, 2> lr{};
        std::int8_t          bf = 0;
    };

template <typename Derived>
template <typename Pre, typename Fac>
auto AdelsonVelskyLandisNode<Derived>::search(AdelsonVelskyLandisNode& origin, const Pre& predicate, const Fac& factory) -> std::tuple<Derived*, bool>
{
    LOG4CXX_ASSERT(logger, !origin.isLinked(), " ");
    AdelsonVelskyLandisNode*& root = origin.lr[0];

    AdelsonVelskyLandisNode* out = nullptr;
    AdelsonVelskyLandisNode* up  = root;
    AdelsonVelskyLandisNode* n   = root;
    bool  r   = false;
    while (n != nullptr)
    {
        LOG4CXX_ASSERT(logger, n->isLinked(), " ");

        const auto cmp = predicate(*down(n));
        if (0 == cmp)
        {
            out = n;
            break;
        }
        r  = cmp > 0;
        up = n;
        n  = n->lr[r];
        LOG4CXX_ASSERT(logger, (nullptr == n) || (n->up == up), " ");
    }
    if (nullptr != out)
    {
        return std::make_tuple(down(out), true);
    }

    out = factory();
    //LOG4CXX_ASSERT(out != &origin);
    if (nullptr == out)
    {
        return std::make_tuple(nullptr, true);
    }
    out->unlink();

    if (up != nullptr)
    {
        //LOG4CXX_ASSERT(up->lr[r] == nullptr);
        up->lr[r] = out;
        out->up   = up;
    }
    else
    {
        root    = out;
        out->up = &origin;
    }
    if (AdelsonVelskyLandisNode* const rt = out->retraceOnGrowth())
    {
        root = rt;
    }
    return std::make_tuple(down(out), false);
}

template <typename Derived>
void AdelsonVelskyLandisNode<Derived>::removeImpl(const AdelsonVelskyLandisNode* const node) noexcept
{
    //LOG4CXX_ASSERT(node != nullptr);
    //LOG4CXX_ASSERT(node->isLinked());

    AdelsonVelskyLandisNode* p = nullptr;  // The lowest parent node that suffered a shortening of its subtree.
    bool  r = false;    // Which side of the above was shortened.
    // The first step is to update the topology and remember the node where to start the retracing from later.
    // Balancing is not performed yet, so we may end up with an unbalanced tree.
    if ((node->lr[0] != nullptr) && (node->lr[1] != nullptr))
    {
        AdelsonVelskyLandisNode* const re = min(node->lr[1]);
        //LOG4CXX_ASSERT((re != nullptr) && (nullptr == re->lr[0]) && (re->up != nullptr));
        re->bf        = node->bf;
        re->lr[0]     = node->lr[0];
        re->lr[0]->up = re;
        if (re->up != node)
        {
            p = re->getParentNode();  // Retracing starts with the ex-parent of our replacement node.
            //LOG4CXX_ASSERT(p->lr[0] == re);
            p->lr[0] = re->lr[1];  // Reducing the height of the left subtree here.
            if (p->lr[0] != nullptr)
            {
                p->lr[0]->up = p;
            }
            re->lr[1]     = node->lr[1];
            re->lr[1]->up = re;
            r             = false;
        }
        else  // In this case, we are reducing the height of the right subtree, so r=1.
        {
            p = re;    // Retracing starts with the replacement node itself as we are deleting its parent.
            r = true;  // The right child of the replacement node remains the same, so we don't bother relinking it.
        }
        re->up                            = node->up;
        re->up->lr[re->up->lr[1] == node] = re;  // Replace link in the parent of node.
    }
    else  // Either or both of the children are nullptr.
    {
        p             = node->up;
        const bool rr = node->lr[1] != nullptr;
        if (node->lr[rr] != nullptr)
        {
            node->lr[rr]->up = p;
        }

        r        = p->lr[1] == node;
        p->lr[r] = node->lr[rr];
        if (p->lr[r] != nullptr)
        {
            p->lr[r]->up = p;
        }
    }
    // Now that the topology is updated, perform the retracing to restore balance. We climb up adjusting the
    // balance factors until we reach the root or a parent whose balance factor becomes plus/minus one, which
    // means that that parent was able to absorb the balance delta; in other words, the height of the outer
    // subtree is unchanged, so upper balance factors shall be kept unchanged.
    if (p->isLinked())
    {
        for (;;)  // NOSONAR cpp:S5311
        {
            AdelsonVelskyLandisNode* const c = p->adjustBalance(!r);
            //LOG4CXX_ASSERT(nullptr != c);
            p = c->getParentNode();
            if ((c->bf != 0) || (nullptr == p))
            {
                // Reached the root or the height difference is absorbed by `c`.
                //LOG4CXX_ASSERT(nullptr != c->up);
                if ((nullptr == p) && (nullptr != c->up))  // NOSONAR cpp:S134
                {
                    c->up->lr[0] = c;
                }
                break;
            }
            r = p->lr[1] == c;
        }
    }
}

template <typename Derived>
auto AdelsonVelskyLandisNode<Derived>::adjustBalance(const bool increment) noexcept -> AdelsonVelskyLandisNode*
{
    //LOG4CXX_ASSERT(isLinked());
    //LOG4CXX_ASSERT(((bf >= -1) && (bf <= +1)));
    AdelsonVelskyLandisNode*      out    = this;
    const auto new_bf = static_cast<std::int8_t>(bf + (increment ? +1 : -1));
    if ((new_bf < -1) || (new_bf > 1))
    {
        const bool        r    = new_bf < 0;   // bf<0 if left-heavy --> right rotation is needed.
        const std::int8_t sign = r ? +1 : -1;  // Positive if we are rotating right.
        AdelsonVelskyLandisNode* const       z    = lr[!r];
        //LOG4CXX_ASSERT(z != nullptr);  // Heavy side cannot be empty. NOLINTNEXTLINE(clang-analyzer-core.NullDereference)
        if ((z->bf * sign) <= 0)    // Parent and child are heavy on the same side or the child is balanced.
        {
            out = z;
            rotate(r);
            if (0 == z->bf)
            {
                bf    = static_cast<std::int8_t>(-sign);
                z->bf = static_cast<std::int8_t>(+sign);
            }
            else
            {
                bf    = 0;
                z->bf = 0;
            }
        }
        else  // Otherwise, the child needs to be rotated in the opposite direction first.
        {
            AdelsonVelskyLandisNode* const y = z->lr[r];
            //LOG4CXX_ASSERT(y != nullptr);  // Heavy side cannot be empty.
            out = y;
            z->rotate(!r);
            rotate(r);
            if ((y->bf * sign) < 0)
            {
                bf    = static_cast<std::int8_t>(+sign);
                y->bf = 0;
                z->bf = 0;
            }
            else if ((y->bf * sign) > 0)
            {
                bf    = 0;
                y->bf = 0;
                z->bf = static_cast<std::int8_t>(-sign);
            }
            else
            {
                bf    = 0;
                z->bf = 0;
            }
        }
    }
    else
    {
        bf = new_bf;  // Balancing not needed, just update the balance factor and call it a day.
    }
    //LOG4CXX_ASSERT(nullptr != out);
    return out;
}

template <typename Derived>
auto AdelsonVelskyLandisNode<Derived>::retraceOnGrowth() noexcept -> AdelsonVelskyLandisNode*
{
    //LOG4CXX_ASSERT(0 == bf);
    AdelsonVelskyLandisNode* c = this;                   // Child
    AdelsonVelskyLandisNode* p = this->getParentNode();  // Parent
    while (p != nullptr)
    {
        const bool r = p->lr[1] == c;  // c is the right child of parent
        //LOG4CXX_ASSERT(p->lr[r] == c);
        c = p->adjustBalance(r);
        p = c->getParentNode();
        if (0 == c->bf)
        {           // The height change of the subtree made this parent perfectly balanced (as all things should be),
            break;  // hence, the height of the outer subtree is unchanged, so upper balance factors are unchanged.
        }
    }
    //LOG4CXX_ASSERT(c != nullptr);
    return (nullptr == p) ? c : nullptr;  // New root or nothing.
}

// No Sonar cpp:S134 b/c this is the main in-order traversal tool - maintainability is not a concern here.
template <typename Derived>
template <typename NodeT, typename DerivedT, typename Vis>
void AdelsonVelskyLandisNode<Derived>::traverseInOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse)
{
    NodeT* node = root;
    NodeT* prev = nullptr;

    while (nullptr != node)
    {
        NodeT* next = node->getParentNode();

        // Did we come down to this node from `prev`?
        if (prev == next)
        {
            if (auto* const left = node->lr[reverse])
            {
                next = left;
            }
            else
            {
                visitor(*down(node));

                if (auto* const right = node->lr[!reverse])  // NOSONAR cpp:S134
                {
                    next = right;
                }
            }
        }
        // Did we come up to this node from the left child?
        else if (prev == node->lr[reverse])
        {
            visitor(*down(node));

            if (auto* const right = node->lr[!reverse])
            {
                next = right;
            }
        }
        else
        {
            // next has already been set to the parent node.
        }

        prev = std::exchange(node, next);
    }
}

// No Sonar cpp:S134 b/c this is the main in-order returning traversal tool - maintainability is not a concern here.
template <typename Derived>
template <typename Result, typename NodeT, typename DerivedT, typename Vis>
auto AdelsonVelskyLandisNode<Derived>::traverseInOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse) -> Result
{
    NodeT* node = root;
    NodeT* prev = nullptr;

    while (nullptr != node)
    {
        NodeT* next = node->getParentNode();

        // Did we come down to this node from `prev`?
        if (prev == next)
        {
            if (auto* const left = node->lr[reverse])
            {
                next = left;
            }
            else
            {
                // NOLINTNEXTLINE(*-qualified-auto)
                if (auto t = visitor(*down(node)))  // NOSONAR cpp:S134
                {
                    return t;
                }

                if (auto* const right = node->lr[!reverse])  // NOSONAR cpp:S134
                {
                    next = right;
                }
            }
        }
        // Did we come up to this node from the left child?
        else if (prev == node->lr[reverse])
        {
            if (auto t = visitor(*down(node)))  // NOLINT(*-qualified-auto)
            {
                return t;
            }

            if (auto* const right = node->lr[!reverse])
            {
                next = right;
            }
        }
        else
        {
            // next has already been set to the parent node.
        }

        prev = std::exchange(node, next);
    }
    return Result{};
}

template <typename Derived>
template <typename NodeT, typename DerivedT, typename Vis>
void AdelsonVelskyLandisNode<Derived>::traversePostOrderImpl(DerivedT* const root, const Vis& visitor, const bool reverse)
{
    NodeT* node = root;
    NodeT* prev = nullptr;

    while (nullptr != node)
    {
        NodeT* next = node->getParentNode();

        // Did we come down to this node from `prev`?
        if (prev == next)
        {
            if (auto* const left = node->lr[reverse])
            {
                next = left;
            }
            else if (auto* const right = node->lr[!reverse])
            {
                next = right;
            }
            else
            {
                visitor(*down(node));
            }
        }
        // Did we come up to this node from the left child?
        else if (prev == node->lr[reverse])
        {
            if (auto* const right = node->lr[!reverse])
            {
                next = right;
            }
            else
            {
                visitor(*down(node));
            }
        }
        // We came up to this node from the right child.
        else
        {
            visitor(*down(node));
        }

        prev = std::exchange(node, next);
    }
}

}

#endif
