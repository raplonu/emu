#pragma once

#include <cuda/runtime_api.hpp>

namespace emu::cuda::stream
{
    inline ::cuda::stream_t wrap(
	    ::cuda::device::id_t       device_id,
        ::cuda::stream::handle_t   stream_handle,
        bool                       take_ownership = false) noexcept
    {
        auto pc = ::cuda::device::get(device_id).primary_context(::cuda::do_not_hold_primary_context_refcount_unit);
        ::cuda::device::primary_context::detail_::increase_refcount(device_id);
        return ::cuda::stream::wrap(device_id, pc.handle(), stream_handle, take_ownership, ::cuda::do_hold_primary_context_refcount_unit);
    }

} // namespace emu::cuda::stream
