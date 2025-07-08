#pragma once

#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>
#include <emu/cuda/event.hpp>
#include <emu/cuda/memory.hpp>

#include <string_view>

namespace emu::cuda
{

namespace detail
{
    // inline void* allocate(device_cref device, ::std::size_t size_bytes)
    // {
    //     device.make_current();
    //     void* ptr;
    //     throw_if_error(cudaMalloc(&ptr, size_bytes));
    //     return ptr;
    // }

    // inline void deallocate(void* ptr)
    // {
    //     throw_if_error(cudaFree(ptr));
    // }

    // inline ::std::pair<void*, ::std::size_t> allocate_pitch(device_cref device, ::std::size_t width_bytes, ::std::size_t height)
    // {
    //     device.make_current();
    //     ::std::pair<void*, ::std::size_t> result{nullptr, 0};
    //     throw_if_error(cudaMallocPitch(&result.first, &result.second, width_bytes, height));
    //     return result;
    // }

} // namespace detail

    // template<typename T>
    // inline ::std::pair<T*, ::std::size_t> allocate_pitch(device_cref device, ::std::size_t width, ::std::size_t height) {
    //     auto result = detail::allocate_pitch(device, width * sizeof(T), height);
    //     return ::std::make_pair(static_cast<T*>(result.first), result.second / sizeof(T));
    // }

    // template<typename T>
    // inline T* allocate(device_cref device, ::std::size_t size) {
    //     return static_cast<T*>(detail::allocate(device, size * sizeof(T)));
    // }

    // template<typename T>
    // inline void deallocate(T* p, ::std::size_t) {
    //     detail::deallocate(p);
    // }


// namespace detail
// {

//     inline void copy_2d(void *destination, ::std::size_t d_pitch, const void * source, ::std::size_t s_pitch, ::std::size_t width_bytes, ::std::size_t height) {
//         throw_if_error(cudaMemcpy2D(destination, d_pitch, source, s_pitch, width_bytes, height, cudaMemcpyDefault));
//     }

//     inline void copy_2d(void *destination, ::std::size_t d_pitch, const void * source, ::std::size_t s_pitch, ::std::size_t width_bytes, ::std::size_t height, stream::handle_t stream_id) {
//         throw_if_error(cudaMemcpy2DAsync(destination, d_pitch, source, s_pitch, width_bytes, height, cudaMemcpyDefault, stream_id));
//     }

// } // namespace detail

//     template<typename T>
//     void copy_2d(T *destination, ::std::size_t d_pitch, const T * source, ::std::size_t s_pitch, ::std::size_t width, ::std::size_t height) {
//         detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height);
//     }

//     template<typename T>
//     void copy_2d(T *destination, ::std::size_t d_pitch, const T * source, ::std::size_t s_pitch, ::std::size_t width, ::std::size_t height, stream_cref stream) {
//         detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height, stream.handle());
//     }

//     template<typename T>
//     void copy(T * destination, const T * source, ::std::size_t count) {
//         throw_if_error(cudaMemcpy((void*)destination, (const void*)source, count * sizeof(T), cudaMemcpyDefault));
//     }

//     template<typename T>
//     void copy(T * destination, const T * source, ::std::size_t count, stream_cref stream) {
//         throw_if_error(cudaMemcpyAsync((void*)destination, (const void*)source, count * sizeof(T), cudaMemcpyDefault, stream.handle()));
//     }

    /*
    inline cu::launch_configuration_t make_linear_launch_config(
        const cu::device_t&  device, size_t length
    ) {
        // auto threads_per_block = device.properties().max_threads_per_block();
        // cu::grid::dimension_t num_blocks =
        //     (length / threads_per_block) +
        //     (length % threads_per_block == 0 ? 0 : 1);

        return cu::launch_config_builder()
            .device(device)
            .overall_size(length)
            .use_maximum_linear_block()
            .build();
    }
    */

} // namespace emu::cuda
