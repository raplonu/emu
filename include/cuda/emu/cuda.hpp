#pragma once

#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>

#include <cuda/runtime_api.hpp>

namespace emu
{

namespace cuda
{
    namespace cu = ::cuda;

    using stream_ref  =       cu::stream_t &;
    using stream_cref = const cu::stream_t &;

namespace detail
{

    inline ::std::pair<void*, ::std::size_t> allocate_pitch(cu::device_t device, ::std::size_t width_bytes, ::std::size_t height)
    {
        ::std::pair<void*, ::std::size_t> result{nullptr, 0};
    	EMU_CUDA_DEVICE_FOR_THIS_SCOPE(device);
        cu::throw_if_error(cudaMallocPitch(&result.first, &result.second, width_bytes, height));

        return result;
    }

    // using ScopedMemory = emu::scoped_t<id_t, detail::Destroyer>;

} // namespace detail


    template<typename T>
    inline ::std::pair<T*, ::std::size_t> allocate_pitch(cu::device_t device, ::std::size_t width, ::std::size_t height) {
        auto result = detail::allocate_pitch(device, width * sizeof(T), height);
        return ::std::make_pair(static_cast<T*>(result.first), result.second / sizeof(T));
    }

    template<typename T>
    inline T* allocate(cu::device_t device, ::std::size_t size) {
        return static_cast<T*>(cu::memory::device::allocate(device, size * sizeof(T)).start());
    }

    template<typename T>
    inline void deallocate(T* p, ::std::size_t) {
        cu::memory::device::free(p);
    }


namespace detail
{

    inline void copy_2d(void *destination, ::std::size_t d_pitch, const void * source, ::std::size_t s_pitch, ::std::size_t width_bytes, ::std::size_t height) {
        auto result = cudaMemcpy2D(destination, d_pitch, source, s_pitch, width_bytes, height, cudaMemcpyDefault);

    	cu::throw_if_error(result, "Synchronously copying data");
    }

    inline void copy_2d(void *destination, ::std::size_t d_pitch, const void * source, ::std::size_t s_pitch, ::std::size_t width_bytes, ::std::size_t height, cu::stream::handle_t stream_id) {
        auto result = cudaMemcpy2DAsync(destination, d_pitch, source, s_pitch, width_bytes, height, cudaMemcpyDefault, stream_id);

    	cu::throw_if_error(result, "Scheduling a memory copy on stream " + cu::detail_::ptr_as_hex(stream_id));
    }

} // namespace detail

    template<typename T>
    void copy_2d(T *destination, ::std::size_t d_pitch, const T * source, ::std::size_t s_pitch, ::std::size_t width, ::std::size_t height) {
        detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height);
    }

    template<typename T>
    void copy_2d(T *destination, ::std::size_t d_pitch, const T * source, ::std::size_t s_pitch, ::std::size_t width, ::std::size_t height, stream_cref stream) {
        detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height, stream.handle());
    }

    template<typename T>
    void copy(T * destination, const T * source, ::std::size_t count) {
        cu::memory::copy((void*)destination, (const void*)source, count * sizeof(T));
    }

    template<typename T>
    void copy(T * destination, const T * source, ::std::size_t count, stream_cref stream) {
        cu::memory::async::detail_::copy((void*)destination, (const void*)source, count * sizeof(T), stream.handle());
    }

    inline cu::launch_configuration_t make_linear_launch_config(
        const cu::device_t  device,
        size_t                length)
    {
        auto threads_per_block = device.properties().max_threads_per_block();
        cu::grid::dimension_t num_blocks =
            (length / threads_per_block) +
            (length % threads_per_block == 0 ? 0 : 1);
        return cu::make_launch_config(num_blocks, threads_per_block, cu::no_dynamic_shared_memory);
    }

    // template<typename T>
    // using DataContainer = emu::scoped_t<T*, cu::memory::device::detail_::deleter>;

} // namespace cuda

} // namespace emu
