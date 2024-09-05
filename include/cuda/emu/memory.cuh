#pragma once

#include <emu/cuda.hpp>
#include <emu/macro.cuh>

namespace emu
{

namespace cuda
{

namespace memory
{

#if EMU_DEVICE_CONTEXT

    template<typename T>
    EMU_HODE constexpr T read(const T * ptr)
    {
        return *ptr;
    }

    template<typename T>
    EMU_HODE constexpr T volatile_read(const volatile T * ptr)
    {
        return *ptr;
    }

    template<typename T>
    EMU_HODE constexpr void write(T * ptr, T value)
    {
        *ptr = value;
    }

#else // NOT EMU_DEVICE_CONTEXT

    template<typename T>
    EMU_HODE T read(const T * ptr)
    {
        T res;
        emu::cuda::copy(&res, ptr, 1);
        return res;
    }

    template<typename T>
    EMU_HODE T volatile_read(const T * ptr)
    {
        T res;
        emu::cuda::copy(&res, ptr, 1);
        return res;
    }

    template<typename T>
    EMU_HODE void write(T * ptr, T value)
    {
        emu::cuda::copy(ptr, &value, 1);
    }

#endif // EMU_DEVICE_CONTEXT

    template<typename T>
    T read(const T * ptr, stream_cref stream)
    {
        T res;
        emu::cuda::copy(&res, ptr, 1, stream);
        return res;
    }

    template<typename T>
    void write(T * ptr, T value, stream_cref stream)
    {
        emu::cuda::copy(ptr, &value, 1 ,stream);
    }

} // namespace memory

} // namespace cuda

} // namespace emu
