#pragma once

#include <emu/concepts.hpp>
#include <emu/cuda/error.hpp>
#include <emu/scoped.hpp>
#include <emu/cuda/device.hpp>

namespace emu::cuda::memory
{

namespace device
{

namespace detail
{
    inline void* allocate(::std::size_t size_bytes)
    {
        void* ptr;

        EMU_CUDA_CHECK_THROW_ERROR(cudaMalloc(&ptr, size_bytes));
        return ptr;
    }

    inline void deallocate(void* ptr)
    {
        EMU_CUDA_CHECK_THROW_ERROR(cudaFree(ptr));
    }

    struct Destroyer
    {
        template<typename T>
        void operator()(T* ptr) const {
            deallocate(static_cast<void*>(ptr));
        }
    };

} // namespace detail

    template<typename T>
    using scoped_ptr = scoped<T*, detail::Destroyer>;

    template<typename T>
    scoped_ptr<T> allocate(device_t device, std::size_t n = 1)
    {
        device.make_current();

        auto ptr = detail::allocate(n * sizeof(T));
        return scoped_ptr<T>(static_cast<T*>(ptr));
    }

    template<typename T>
    struct allocator
    {
        using value_type = T;

        allocator() = default;

        template<typename U>
        allocator(const allocator<U>&) noexcept {}

        T* allocate(std::size_t n)
        {
            return static_cast<T*>(detail::allocate(n * sizeof(T)));
        }

        void deallocate(T* p, std::size_t) noexcept
        {
            detail::deallocate(static_cast<void*>(p));
        }
    };

    template<emu::cpts::array T>
    using unique_ptr = std::unique_ptr<T, detail::Destroyer>;

    template<emu::cpts::array T>
    unique_ptr<T> make_unique(device_t device, std::size_t n = 1)
    {
        using element_type = std::remove_extent_t<T>;

        auto scoped_ptr = allocate<element_type>(device, n);

        return unique_ptr<T>(static_cast<element_type*>(scoped_ptr.release()));
    }

} // namespace device

} // namespace emu::cuda::memory
