#pragma once

#include <emu/scoped.hpp>

#include <cuda/runtime_api.hpp>

#define EMU_CUDA_DEVICE_FOR_THIS_SCOPE(device) \
    EMU_INVOKE_AT_SCOPE_EXIT([d = ::cuda::device::current::get()]() { ::cuda::device::current::set(d); }); \
    ::cuda::device::current::set(device)

namespace emu
{
    inline void make_current(::cuda::device::id_t device_id)
    {
        ::cuda::device::get(device_id).make_current();
    }

    template<typename T>
    T set_and_forward(::cuda::device_t device, T&& t)
    {
        device.make_current();
        return EMU_FWD(t);
    }

    template<typename T>
    T set_and_forward(::cuda::device::id_t device_id, T&& t)
    {
        make_current(device_id);
        return EMU_FWD(t);
    }

    template<typename F>
    decltype(auto) set_and_invoke(::cuda::device_t device, F&& f)
    {
        ::cuda::device::current::set(device);
        return EMU_FWD(f)();
    }

    template<typename F>
    decltype(auto) set_and_invoke(::cuda::device::id_t device_id, F&& f)
    {
        make_current(device_id);
        return EMU_FWD(f)();
    }


}
