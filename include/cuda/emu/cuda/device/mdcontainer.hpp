#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/cuda/device/accessor.hpp>

namespace emu::cuda::device
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdcontainer = emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, cuda::device::accessor<AccessorPolicy>
    >;

    EMU_DEFINE_MDCONTAINER_ALIAS;

} // namespace emu::cuda::device
