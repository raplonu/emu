#ifndef EMU_THRUST_CUH
#define EMU_THRUST_CUH

#include <thrust/device_ptr.h>

namespace emu
{
    template<typename T>
    constexpr thrust::device_ptr<T> device_ptr(T* ptr) noexcept {
        return thrust::device_ptr<T>(ptr);
    }

    template<typename T>
    constexpr thrust::device_ptr<const T> device_ptr(const T* ptr) noexcept {
        return thrust::device_ptr<const T>(ptr);
    }
} // namespace emu

#endif //EMU_THRUST_CUH