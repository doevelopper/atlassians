#ifndef COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_NULLALLOCATOR_HPP
#define COM_GITHUB_DOEVELOPPER_PREMISSES_ATLAS_QUOTA_NULLALLOCATOR_HPP

#include <com/github/doevelopper/atlassians/logging/LogMacros.hpp>

namespace com::github::doevelopper::atlassians::atlas::quota
{
    /*!
     * @brief Allocator returning nullptr.
     * @ingroup quota
     * @details The NullAllocator class template represents an implementation of the allocator concept of
     *      the standard library and acts as a stand-in for situation where no memory allocation is
     *      required. The NullAllocator will never allocate any memory and will always return nullptr.
    */
    template< typename T >
    class NullAllocator
    {
        LOG4CXX_DECLARE_STATIC_LOGGER
     public:

        using ValueType      = T;          //!< Type of the allocated values.
        using SizeType       = size_t;     //!< Size type of the null allocator.
        using DifferenceType = ptrdiff_t;  //!< Difference type of the null allocator.

        // STL allocator requirements
        using value_type      = ValueType;       //!< Type of the allocated values.
        using size_type       = SizeType;        //!< Size type of the null allocator.
        using difference_type = DifferenceType;  //!< Difference type of the null allocator.

        /*!
         * @brief Implementation of the NullAllocator rebind mechanism.
        */
        template< typename U >
        struct rebind
        {
            using other = NullAllocator<U>;  //!< Type of the other allocator.
        };

        /*!
         * @name Constructors
         * @{
         */
        NullAllocator() noexcept;
        NullAllocator(const NullAllocator&) noexcept = default;
        NullAllocator(NullAllocator&&) noexcept =  default;
        NullAllocator& operator=(const NullAllocator&) noexcept = default;
        NullAllocator& operator=(NullAllocator&&) noexcept = default;
        virtual ~NullAllocator() noexcept;

        /*!
         * @brief Conversion constructor from different NullAllocator instances.
         * @param allocator The foreign null allocator to be copied.
        */
        template< typename U >
        inline NullAllocator( [[maybe_unused]] const NullAllocator<U>& );
        /*!
         * @}
         */
        /*!
         * @name Allocation functions
         * @{
         */
        /*!
         * @brief Performs no memory allocation and returns nullptr.
         *
         * @param numObjects The number of objects to be allocated.
         * @return Pointer to the newly allocated memory.
         *
         * @warning This function does not perform any memory allocation and always returns nullptr.
        */
        inline T*   allocate  ( [[maybe_unused]] std::size_t numObjects );
        inline void deallocate( [[maybe_unused]] T* ptr, [[maybe_unused]] std::size_t numObjects ) noexcept;
        /*!
         * @}
         */
    protected:
    private:
    };

    /*!
     * @name NullAllocator operators
     * @{
     */
    template< typename T1, typename T2 >
    inline bool operator==( [[maybe_unused]] const NullAllocator<T1>& lhs, [[maybe_unused]] const NullAllocator<T2>& rhs ) noexcept;

    template< typename T1, typename T2 >
    inline bool operator!=( [[maybe_unused]] const NullAllocator<T1>& lhs, [[maybe_unused]] const NullAllocator<T2>& rhs ) noexcept;
    /*!
     * @}
     */

    /*!
     * @brief Equality comparison between two NullAllocator objects.
     *
     * @param lhs The left-hand side null allocator.
     * @param rhs The right-hand side null allocator.
     * @return @a true.
    */
    template< typename T1    // Type of the left-hand side null allocator
            , typename T2 >  // Type of the right-hand side null allocator
    inline bool operator==( [[maybe_unused]] const NullAllocator<T1>& lhs, [[maybe_unused]] const NullAllocator<T2>& rhs ) noexcept
    {
        return true;
    }

    /*!
     * @brief Inequality comparison between two NullAllocator objects.
     *
     * @param lhs The left-hand side null allocator.
     * @param rhs The right-hand side null allocator.
     * @return @a false.
    */
    template< typename T1    // Type of the left-hand side null allocator
            , typename T2 >  // Type of the right-hand side null allocator
    inline bool operator!=( [[maybe_unused]] const NullAllocator<T1>& lhs, [[maybe_unused]] const NullAllocator<T2>& rhs ) noexcept
    {
        return false;
    }
}
#endif
