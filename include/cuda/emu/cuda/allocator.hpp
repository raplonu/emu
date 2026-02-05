#pragma once

#include <emu/cuda/memory.hpp>

/**
 * @file allocator.hpp
 * @brief CUDA device memory allocator implementation
 *
 * This file provides a standard library compatible allocator for CUDA device memory.
 * The allocator manages memory allocation and deallocation on specific CUDA devices.
 */

namespace emu::cuda::device
{

    /**
     * @brief Standard library compatible allocator for CUDA device memory
     *
     * This allocator provides a standard interface for allocating and deallocating
     * memory on CUDA devices. It maintains device affinity and ensures that memory
     * operations are performed on the correct device context.
     *
     * @tparam T The type of objects to allocate
     *
     * @note This allocator is stateful and stores the target device. Two allocators
     *       are equal if and only if they target the same device.
     */
    template<typename T>
    struct allocator
    {
        /// @brief The type of objects allocated by this allocator
        using value_type = T;
        /// @brief Unsigned integer type for sizes
        using size_type = std::size_t;
        /// @brief Signed integer type for pointer differences
        using difference_type = std::ptrdiff_t;

        /// @brief Pointer to allocated objects
        using pointer = T*;
        /// @brief Pointer to const allocated objects
        using const_pointer = const T*;
        /// @brief Reference to allocated objects
        using reference = T&;
        /// @brief Reference to const allocated objects
        using const_reference = const T&;

        /**
         * @brief Rebind allocator to different type
         *
         * Provides a mechanism to obtain an allocator for a different type
         * while maintaining the same allocation strategy and device affinity.
         *
         * @tparam U The new type to allocate
         */
        template<typename U>
        struct rebind
        {
            using other = allocator<U>;
        };

        /// @brief The CUDA device this allocator targets
        device_ref device;

        /**
         * @brief Default constructor
         *
         * Constructs an allocator that targets the current CUDA device.
         *
         * @note This constructor is noexcept
         */
        allocator() noexcept: device(current_device()) {}

        /**
         * @brief Device-specific constructor
         *
         * Constructs an allocator that targets the specified CUDA device.
         *
         * @param device The CUDA device to target for allocations
         *
         * @note This constructor is noexcept
         */
        allocator(device_ref device) noexcept: device(device) {}

        /**
         * @brief Copy constructor from different type
         *
         * Constructs an allocator by copying the device affinity from another
         * allocator that may allocate objects of a different type.
         *
         * @tparam U The type of the source allocator
         * @param other The source allocator to copy from
         *
         * @note This constructor is noexcept
         */
        template<typename U>
        allocator(const allocator<U>& other) noexcept: device(other.device) {}

        /**
         * @brief Allocate memory for n objects
         *
         * Allocates memory on the target CUDA device for n objects of type T.
         * The device context is made current before allocation.
         *
         * @param n The number of objects to allocate space for
         * @return Pointer to the allocated memory
         *
         * @throws May throw CUDA-related exceptions if allocation fails
         *
         * @note The returned memory is uninitialized
         */
        pointer allocate(size_type n)
        {
            set_current(device);

            return emu::cuda::device::detail::allocate<value_type>(n);
        }

        /**
         * @brief Deallocate previously allocated memory
         *
         * Deallocates memory that was previously allocated by this allocator.
         * The size parameter is ignored as it's not needed for CUDA memory deallocation.
         *
         * @param p Pointer to the memory to deallocate
         * @param size_type Unused parameter (kept for standard compatibility)
         *
         * @note This function does not throw exceptions
         */
        void deallocate(pointer p, size_type )
        {
            detail::deallocate(p);
        }

        /**
         * @brief Equality comparison operator
         *
         * Two allocators are equal if they target the same CUDA device.
         * This determines whether memory allocated by one can be deallocated by the other.
         *
         * @tparam U The type of the other allocator
         * @param other The allocator to compare with
         * @return true if both allocators target the same device, false otherwise
         *
         * @note This operator is noexcept
         */
        template<typename U>
        bool operator==(const allocator<U>& other) const noexcept
        {
            return device == other.device;
        }

        /**
         * @brief Inequality comparison operator
         *
         * Two allocators are not equal if they target different CUDA devices.
         *
         * @tparam U The type of the other allocator
         * @param other The allocator to compare with
         * @return true if allocators target different devices, false otherwise
         *
         * @note This operator is noexcept
         */
        template<typename U>
        bool operator!=(const allocator<U>& other) const noexcept
        {
            return !(*this == other);
        }

    };

} // namespace emu::cuda::device
