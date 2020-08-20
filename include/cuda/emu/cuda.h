#ifndef EMU_CUDA_H
#define EMU_CUDA_H

#include <cuda/api/error.hpp>
#include <cuda/api/stream.hpp>
#include <cuda/api/device.hpp>

#include <cuda_runtime_api.h>

namespace emu
{

namespace cuda
{

namespace detail
{

    inline std::pair<void*, std::size_t> allocate_pitch(std::size_t width_bytes, std::size_t height_bytes)
    {
        void* allocated = nullptr;
        std::size_t pitch;
        ::cuda::throw_if_error(cudaMallocPitch(&allocated, &pitch, width_bytes, height_bytes));

        return std::make_pair(allocated, pitch);
    }

    // using ScopedMemory = emu::scoped_t<id_t, detail::Destroyer>;

} // namespace detail


    template<typename T>
    inline std::pair<T*, std::size_t> allocate_pitch(std::size_t width, std::size_t height) {
        auto result = detail::allocate_pitch(width * sizeof(T), height * sizeof(T));
        return std::make_pair(static_cast<T*>(result.first), result.second);
    }

    template<typename T>
    inline T* allocate(::cuda::device_t device, std::size_t size) {
        return ::cuda::memory::device::allocate(device, size * sizeof(T));
    }


namespace detail
{

    inline void copy_2d(void *destination, std::size_t d_pitch, const void * source, std::size_t s_pitch, std::size_t width, std::size_t height) {
        auto result = cudaMemcpy2D(destination, d_pitch, source, s_pitch, width, height, cudaMemcpyDefault);

    	::cuda::throw_if_error(result, "Synchronously copying data");
    }

    inline void copy_2d(void *destination, std::size_t d_pitch, const void * source, std::size_t s_pitch, std::size_t width, std::size_t height, ::cuda::stream::id_t stream_id) {
        auto result = cudaMemcpy2DAsync(destination, d_pitch, source, s_pitch, width, height, cudaMemcpyDefault, stream_id);

    	::cuda::throw_if_error(result, "Scheduling a memory copy on stream " + ::cuda::detail::ptr_as_hex(stream_id));
    }

} // namespace detail


template<typename T>
void copy_2d(T *destination, std::size_t d_pitch, const T * source, std::size_t s_pitch, std::size_t width, std::size_t height) {
    detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height * sizeof(T));
}

template<typename T>
void copy_2d(T *destination, std::size_t d_pitch, const T * source, std::size_t s_pitch, std::size_t width, std::size_t height, const ::cuda::stream_t & stream) {
    detail::copy_2d(destination, d_pitch * sizeof(T), source, s_pitch * sizeof(T), width * sizeof(T), height * sizeof(T), stream.id());
}

} // namespace cuda


} // namespace emu

#endif //EMU_CUDA_H