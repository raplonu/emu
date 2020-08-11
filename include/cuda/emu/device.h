#ifndef EMU_DEVICE_H
#define EMU_DEVICE_H

#include "cuda/api/device.hpp"

namespace emu
{
    inline void make_current(cuda::device::id_t device_id)
    {
        cuda::device::current::set(cuda::device::get(device_id));
    }

    template<typename T>
    T set_and_forward(cuda::device_t device, T&& t)
    {
        cuda::device::current::set(device);
        return FWD(t);
    }

    template<typename T>
    T set_and_forward(cuda::device::id_t device_id, T&& t)
    {
        make_current(device_id);
        return FWD(t);
    }

    template<typename F>
    decltype(auto) set_and_invoke(cuda::device_t device, F&& f)
    {
        cuda::device::current::set(device);
        return FWD(f)();
    }

    template<typename F>
    decltype(auto) set_and_invoke(cuda::device::id_t device_id, F&& f)
    {
        make_current(device_id);
        return FWD(f)();
    }


}

#endif //EMU_DEVICE_H