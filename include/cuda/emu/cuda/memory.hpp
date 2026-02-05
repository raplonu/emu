/**
 * @file
 * @brief Smart pointers for CUDA memory management
 */
#pragma once

#include <emu/assert.hpp>
#include <emu/concepts.hpp>
#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>
#include <emu/detail/basic_span.hpp>
#include <iterator>
#include <memory>
#include <type_traits>

namespace emu::cuda
{

/**
 * @brief Device memory management
 * @see https://docs.nvidia.com/cuda/cuda-driver-api/group__CUDA__MEM.html
 */
namespace device
{

namespace detail
{
    inline void* allocate(size_t size_bytes)
    {
        void* ptr;

        EMU_CHECK_OR_THROW_WHAT(cudaMalloc(&ptr, size_bytes),
                                "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(size_t n)
    {
        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaFree(ptr),
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
    unique_ptr<T> make_unique(device_ref device, size_t n)
    {
        using element_type = std::remove_extent_t<T>;

        set_current(device);

        return unique_ptr<T>(detail::allocate<element_type>(n));
    }


    /**
     * @brief Create a unique pointer to device memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(size_t n)
    {
        using element_type = std::remove_extent_t<T>;

        return unique_ptr<T>(detail::allocate<element_type>(n));
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
    inline void* allocate(size_t size_bytes)
    {
        void* ptr;
        EMU_CHECK_OR_THROW_WHAT(cudaMallocHost(&ptr, size_bytes),
                                "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(size_t n)
    {
        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaFreeHost(ptr),
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
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(device_ref device, size_t n)
    {
        using element_type = std::remove_extent_t<T>;

        set_current(device);

        return unique_ptr<T>(detail::allocate<element_type>(n));
    }

    /**
     * @brief Create a unique pointer to host memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param device The device to allocate memory on
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(size_t n)
    {
        using element_type = std::remove_extent_t<T>;
        return unique_ptr<T>(detail::allocate<element_type>(n));
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
    inline void* allocate(size_t size_bytes)
    {
        void* ptr;
        EMU_CHECK_OR_THROW_WHAT(cudaMallocManaged(&ptr, size_bytes),
                                "Failed to allocate device memory");
        return ptr;
    }

    template<typename T>
    T* allocate(size_t n)
    {
        return static_cast<T*>( allocate(n * sizeof(T)) );
    }

    inline void deallocate(void* ptr)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaFree(ptr),
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
    unique_ptr<T> make_unique(device_ref device, size_t n)
    {
        using element_type = std::remove_extent_t<T>;

        set_current(device);

        return unique_ptr<T>(detail::allocate<element_type>(n));
    }


    /**
     * @brief Create a unique pointer to managed memory
     * @tparam T The type of the managed object. Must be an array type.
     * @param n The number of objects to allocate
     * @return A unique pointer to the allocated memory
     */
    template<emu::cpts::array T>
    unique_ptr<T> make_unique(size_t n)
    {
        using element_type = std::remove_extent_t<T>;

        return unique_ptr<T>(detail::allocate<element_type>(n));
    }

} // namespace managed

namespace detail
{
    template<typename T>
    T* copy_bytes_n_async(const T* first, size_t n, T* result, stream_ref stream)
    {
        EMU_CHECK_OR_THROW_WHAT(
            cudaMemcpyAsync(result,
                            first,
                            n * sizeof(T),
                            cudaMemcpyDefault,
                            stream.get()),
            "Failed to copy memory");
        return result + n;
    }

    template<typename T>
    T* copy_bytes_n_sync(const T* first, size_t n, T* result)
    {
        EMU_CHECK_OR_THROW_WHAT(
            cudaMemcpy(result,
                       first,
                       n * sizeof(T),
                       cudaMemcpyDefault),
            "Failed to copy memory");
        return result + n;
    }

} // namespace detail

    /**
     * @brief Asynchronously copies elements from a source range to a destination range.
     * @tparam InputIt The type of the source iterator.
     * @tparam OutputIt The type of the destination iterator.
     * @param first The beginning of the source range.
     * @param last The end of the source range.
     * @param result The beginning of the destination range.
     * @param stream The CUDA stream to perform the copy on.
     * @return An iterator to the end of the destination range.
     */
    template<typename T>
    T* copy_bytes(const T* first, const T* last, T* result, stream_ref stream)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                    "Value type must be trivially copyable to use memcpy");
        return detail::copy_bytes_n_async(first, std::distance(first, last), result, stream);
    }

    /**
     * @brief Asynchronously copies a number of elements from a source to a destination.
     * @tparam InputIt The type of the source iterator.
     * @tparam OutputIt The type of the destination iterator.
     * @param first The beginning of the source range.
     * @param n The number of elements to copy.
     * @param result The beginning of the destination range.
     * @param stream The CUDA stream to perform the copy on.
     * @return An iterator to the end of the destination range.
     */
    template<typename T>
    T* copy_bytes_n(const T* first, size_t n, T* result, stream_ref stream)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                    "Value type must be trivially copyable to use memcpy");
        return detail::copy_bytes_n_async(first, n, result, stream);
    }

    /**
     * @brief Synchronously copies elements from a source range to a destination range.
     * @tparam InputIt The type of the source iterator.
     * @tparam OutputIt The type of the destination iterator.
     * @param first The beginning of the source range.
     * @param last The end of the source range.
     * @param result The beginning of the destination range.
     * @return An iterator to the end of the destination range.
     */
    template<typename T>
    T* copy_bytes(const T* first, const T* last, T* result)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                    "Value type must be trivially copyable to use memcpy");
        return detail::copy_bytes_n_sync(first, std::distance(first, last), result);
    }

    /**
     * @brief Synchronously copies a number of elements from a source to a destination.
     * @tparam InputIt The type of the source iterator.
     * @tparam OutputIt The type of the destination iterator.
     * @param first The beginning of the source range.
     * @param n The number of elements to copy.
     * @param result The beginning of the destination range.
     * @return An iterator to the end of the destination range.
     */
    template<typename T>
    T* copy_bytes_n(const T* first, size_t n, T* result)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                    "Value type must be trivially copyable to use memcpy");
        return detail::copy_bytes_n_sync(first, n, result);
    }

    template<emu::cpts::span Input, emu::cpts::mutable_span Output>
    auto copy_bytes(Input&& input, Output&& output, stream_ref stream) {
        using std::data;
        using std::size;

        EMU_ASSERT_MSG(size(input) == size(output), "Input and output ranges must have the same size");

        return copy_bytes_n(data(input), size(input), data(output), stream);
    }


} // namespace emu::cuda
