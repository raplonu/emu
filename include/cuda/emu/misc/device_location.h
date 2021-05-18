#ifndef EMU_MISC_DEVICE_LOCATION_H
#define EMU_MISC_DEVICE_LOCATION_H

#include <emu/misc/location.h>
#include <emu/cuda.h>

namespace emu
{

namespace location
{
    struct cuda_t {

        cuda::device_t device;

        cuda_t(): device(cuda::device::current::get()) {}

        cuda_t(cuda::device_t device) noexcept: device(device) {}

    };

} // namespace location

} // namespace emu

#endif //EMU_MISC_DEVICE_LOCATION_H