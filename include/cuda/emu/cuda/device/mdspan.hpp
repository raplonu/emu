#pragma once

#include <emu/detail/basic_mdspan.hpp>
#include <emu/cuda/device/accessor.hpp>

namespace emu::cuda::device
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdspan = stdex::mdspan<T, Extents, LayoutPolicy, cuda::device::accessor<AccessorPolicy>>;

    EMU_DEFINE_MDSPAN_ALIAS;

} // namespace emu::cuda::device
