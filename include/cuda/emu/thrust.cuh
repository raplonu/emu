#pragma once

#include <thrust/device_ptr.h>

namespace emu
{

namespace thrust
{

    /**
     * @brief Cast a raw pointer into a thrust::device_ptr.
     */
    template<typename T>
    constexpr ::thrust::device_ptr<T> device_ptr(T* ptr) noexcept {
        return ::thrust::device_ptr<T>(ptr);
    }

    /**
     * @brief Cast a constant raw pointer into a thrust::device_ptr.
     */
    template<typename T>
    constexpr ::thrust::device_ptr<const T> device_ptr(const T* ptr) noexcept {
        return ::thrust::device_ptr<const T>(ptr);
    }

} // namespace thrust

} // namespace emu
