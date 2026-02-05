#pragma once

#include <emu/cuda/error.hpp>

#include <cuda/devices>

namespace emu::cuda
{
    using ::cuda::device_ref;
    using ::cuda::devices;

    using device_id = int;

namespace detail
{

    inline device_id get_current()
    {
        device_id device_id;
        EMU_CHECK_OR_THROW_WHAT(cudaGetDevice(&device_id),
                                "Failed to get current CUDA device ID");
        return device_id;
    }

    inline void set_current(device_id device_id)
    {
        EMU_CHECK_OR_THROW_WHAT(cudaSetDevice(device_id),
                                "Failed to set current CUDA device ID");
    }

} // namespace detail


    inline device_ref current_device() {
        return device_ref(detail::get_current());
    }

    inline void set_current(device_ref device) {
        detail::set_current(device.get());
    }

    template<typename Fn>
    auto set_current_and_invoke(device_ref device, Fn&& fn) -> decltype(fn()) {
        set_current(device);
        return EMU_FWD(fn)();
    }

} // namespace emu::cuda

#define EMU_CUDA_SET_DEVICE_AND_ASSIGN(DEVICE_REF, VALUE) \
    ::emu::cuda::set_current_and_invoke(DEVICE_REF, [&] { return VALUE; })
