#pragma once

#include <emu/cuda.hpp>

namespace emu::cuda
{

    // using std::allocator;

    template<typename T>
    struct allocator
    {

        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;

        template<typename U>
        struct rebind
        {
            using other = allocator<U>;
        };

        ::cuda::device_t device;

        allocator() noexcept: device(::cuda::device::current::get()) {}

        allocator(::cuda::device_t device) noexcept: device(device) {}

        template<typename U>
        allocator(const allocator<U>& other) noexcept: device(other.device) {}

        pointer allocate(size_type n)
        {
            return cuda::allocate<value_type>(device, n);
        }

        void deallocate(pointer p, size_type n)
        {
            cuda::deallocate(p, n);
        }

        template<typename U>
        bool operator==(const allocator<U>& other) const noexcept
        {
            return device == other.device;
        }

        template<typename U>
        bool operator!=(const allocator<U>& other) const noexcept
        {
            return !(*this == other);
        }

    };

} // namespace emu::cuda
