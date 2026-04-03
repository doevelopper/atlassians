#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_BOUNDEDALLOCATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_BOUNDEDALLOCATOR_HPP

#include <atomic>
#include <type_traits>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::quota
{
    /*!
     * @brief An allocator adapter that throws `bad_alloc` whenever the number of live
     *      allocations reaches a certain limit. The number of live allocations is
     *      shared by all copies of the allocator, and it is provided by the user of
     *      this class so that it can be observed from the outside.
     */
    template <typename Allocator>
    class BoundedAllocator
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        using AllocatorTraits    = std::allocator_traits<Allocator>;
        using pointer            = typename AllocatorTraits::pointer;
        using const_pointer      = typename AllocatorTraits::const_pointer;
        using void_pointer       = typename AllocatorTraits::void_pointer;
        using const_void_pointer = typename AllocatorTraits::const_void_pointer;
        using size_type          = typename AllocatorTraits::size_type;
        using difference_type    = typename AllocatorTraits::difference_type;
        using value_type         = typename AllocatorTraits::value_type;

        template <typename T>
        struct rebind
        {
            using other = BoundedAllocator<typename AllocatorTraits::template rebind_alloc<T>>;
        };

        template <typename>
        friend class BoundedAllocator;

        BoundedAllocator() noexcept;
        BoundedAllocator(const BoundedAllocator &) noexcept             = default;
        BoundedAllocator(BoundedAllocator &&) noexcept                  = default;
        BoundedAllocator & operator=(const BoundedAllocator &) noexcept = default;
        BoundedAllocator & operator=(BoundedAllocator &&) noexcept      = default;
        virtual ~BoundedAllocator() noexcept;

        BoundedAllocator(Allocator allocator, std::size_t max_live_allocations, std::size_t & live_allocations);
        explicit BoundedAllocator(std::size_t max_live_allocations, std::size_t & live_allocations);

        template <
            typename OtherAllocator,
            typename = std::enable_if_t<std::is_constructible<Allocator, OtherAllocator const &>::value>>
        BoundedAllocator(BoundedAllocator<OtherAllocator> const & other);

        template <typename... Args>
        void construct(pointer p, Args &&... args);

        void destroy(pointer p);
        pointer allocate(size_type n);
        void deallocate(pointer p, size_type n);

    protected:
    private:

        Allocator m_allocator;
        std::size_t const m_max_live_allocations;
        std::size_t & m_live_allocations;
    };
}
#endif
