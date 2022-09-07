#ifndef EMU_CUDA_MISC_LOCATION_H
#define EMU_CUDA_MISC_LOCATION_H

#include <emu/misc/location.h>
#include <emu/cuda.h>
#include <emu/cuda/allocator.h>

namespace emu
{

namespace location
{

    struct cuda_t
    {
        template<typename T>
        using default_allocator_type = emu::cuda::allocator<T>;

        ::cuda::device_t device;

        cuda_t(): device(::cuda::device::current::get()) {}

        cuda_t(::cuda::device_t device) noexcept: device(device) {}

        template<typename T>
        default_allocator_type<T> make_default_allocator() {
            return default_allocator_type<T>(device);
        }

    };

} // namespace location

    template<>
    struct is_location<location::cuda_t> : std::true_type {};

} // namespace emu

#endif //EMU_CUDA_MISC_LOCATION_H