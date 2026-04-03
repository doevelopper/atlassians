#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_OUTOFMEMORYALLOCATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_OUTOFMEMORYALLOCATOR_HPP

#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::quota
{
    /*!
     * @brief An allocator adapter that throws `bad_alloc` whenever some boolean flag is
     *      set, and unsets the flag when it's done. By having the user control when
     *      that flag is set, it's possible to test allocators in artificial
     *      out-of-memory conditions. After a `bad_alloc` has been thrown, the
     *      flag is reset to `false` so that a user can observe that a `bad_alloc`
     *      was indeed thrown.
     */

    template <typename Allocator>
    class OutOfMemoryAllocator
    {
        Allocator m_allocator;
        bool & m_oom_flag;

        using AllocatorTraits = std::allocator_traits<Allocator>;

        template <typename>
        friend class OutOfMemoryAllocator;
        LOG4CXX_DECLARE_STATIC_LOGGER
    public:

        OutOfMemoryAllocator() noexcept;
        OutOfMemoryAllocator(const OutOfMemoryAllocator &) noexcept             = default;
        OutOfMemoryAllocator(OutOfMemoryAllocator &&) noexcept                  = default;
        OutOfMemoryAllocator & operator=(const OutOfMemoryAllocator &) noexcept = default;
        OutOfMemoryAllocator & operator=(OutOfMemoryAllocator &&) noexcept      = default;
        virtual ~OutOfMemoryAllocator() noexcept;

        OutOfMemoryAllocator(Allocator allocator, bool & oom_flag);
        explicit OutOfMemoryAllocator(bool & oom_flag);

        template <
            typename OtherAllocator,
            typename = std::enable_if_t<std::is_constructible<Allocator, OtherAllocator const &>::value>>
        OutOfMemoryAllocator(OutOfMemoryAllocator<OtherAllocator> const & other);

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
            using other = OutOfMemoryAllocator<typename AllocatorTraits::template rebind_alloc<T>>;
        };

        template <typename... Args>
        void construct(pointer p, Args &&... args);
        void destroy(pointer p);
        pointer allocate(size_type n);
        void deallocate(pointer p, size_type n);
    };
}
#endif
