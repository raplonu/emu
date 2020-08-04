#ifndef EMU_DEVICE_H
#define EMU_DEVICE_H

#include "cuda/api/device.hpp"

namespace emu
{

    template<typename T>
    T set_and_forward(cuda::device_t device, T&& t)
    {
        cuda::device::current::set(device);
        return FWD(t);
    }

    template<typename T>
    T set_and_forward(uda::device::id_t device, T&& t)
    {
        cuda::device::current::set(device);
        return FWD(t);
    }

    void make_current(cuda::device::id_t device_id)
    {
        cuda::device::current::set(cuda::device::get(id_t));
    }

}

#endif //EMU_DEVICE_H