/**
 * @file
 * @brief Smart pointers for CUDA memory management
 */
#pragma once

#include <emu/concepts.hpp>
#include <emu/cuda/error.hpp>
#include <emu/scoped.hpp>
#include <emu/cuda/device.hpp>

namespace emu::cuda::memory
{

/**
 * @brief Device memory management
 * @see https://docs.nvidia.com/cuda/cuda-driver-api/group__CUDA__MEM.html
 */
namespace device
{

namespace detail
{
    inline void* allocate(::std::size_t size_bytes)
    {
        void* ptr;

        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaMalloc(&ptr, size_bytes),
                                     "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(device_t device, std::size_t n = 1)
    {
        device.make_current();

        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaFree(ptr),
                                     "Failed to allocate device memory");
    }

    struct Deallocator
    {
        template<typename T>
        void operator()(T* ptr) const {
            deallocate(static_cast<void*>(ptr));
        }
    };

} // namespace detail

    /**
     * @brief Unique pointer for device memory
     * @tparam T The type of the managed object.
     */
    template<typename T>
    using unique_ptr = std::unique_ptr<T, detail::Deallocator>;

    /**
     * @brief Create a unique pointer to device memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param device The device to allocate memory on
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(device_t device, std::size_t n = 1)
    {
        using element_type = std::remove_extent_t<T>;

        return unique_ptr<T>(detail::allocate<element_type>(device, n));
    }

} // namespace device

/**
 * @brief Host memory management
 * @see https://docs.nvidia.com/cuda/cuda-driver-api/group__CUDA__MEM.html
 */
namespace host
{

namespace detail
{
    inline void* allocate(::std::size_t size_bytes)
    {
        void* ptr;
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaMallocHost(&ptr, size_bytes),
                                     "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(device_t device, std::size_t n = 1)
    {
        device.make_current();

        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaFreeHost(ptr),
                                     "Failed to allocate device memory");
    }

    struct Deallocator
    {
        template<typename T>
        void operator()(T* ptr) const {
            deallocate(static_cast<void*>(ptr));
        }
    };

} // namespace detail

    /**
     * @brief Unique pointer for host memory
     * @tparam T The type of the managed object.
     */
    template<typename T>
    using unique_ptr = std::unique_ptr<T, detail::Deallocator>;

    /**
     * @brief Create a unique pointer to host memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param device The device to allocate memory on
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(device_t device, std::size_t n = 1)
    {
        using element_type = std::remove_extent_t<T>;
        return unique_ptr<T>(detail::allocate<element_type>(device, n));
    }

} // namespace host

/**
 * @brief Managed memory management
 * @see https://docs.nvidia.com/cuda/cuda-driver-api/group__CUDA__MEM.html
 */
namespace managed
{

namespace detail
{
    inline void* allocate(::std::size_t size_bytes)
    {
        void* ptr;
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaMallocManaged(&ptr, size_bytes),
                                     "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(device_t device, std::size_t n = 1)
    {
        device.make_current();
        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaFree(ptr),
                                     "Failed to allocate device memory");
    }

    struct Deallocator
    {
        template<typename T>
        void operator()(T* ptr) const {
            deallocate(static_cast<void*>(ptr));
        }
    };

} // namespace detail

    /**
     * @brief Unique pointer for managed memory
     * @tparam T The type of the managed object.
     */
    template<typename T>
    using unique_ptr = std::unique_ptr<T, detail::Deallocator>;

    /**
     * @brief Create a unique pointer to managed memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param device The device to allocate memory on
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(device_t device, std::size_t n = 1)
    {
        using element_type = std::remove_extent_t<T>;
        return unique_ptr<T>(detail::allocate<element_type>(device, n));
    }

} // namespace managed

} // namespace emu::cuda::memory
