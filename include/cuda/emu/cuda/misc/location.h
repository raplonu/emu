#ifndef EMU_CUDA_MISC_LOCATION_H
#define EMU_CUDA_MISC_LOCATION_H

#include <emu/misc/location.h>
#include <emu/cuda.h>

namespace emu
{

namespace location
{

    struct cuda_t {

        ::cuda::device_t device;

        cuda_t(): device(::cuda::device::current::get()) {}

        cuda_t(::cuda::device_t device) noexcept: device(device) {}

    };

} // namespace location

    template<>
    struct is_location<location::cuda_t> : std::true_type {};

} // namespace emu

#endif //EMU_CUDA_MISC_LOCATION_H