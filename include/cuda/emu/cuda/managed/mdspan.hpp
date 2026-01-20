#pragma once

#include <emu/detail/basic_mdspan.hpp>
#include <emu/cuda/managed/accessor.hpp>

namespace emu::cuda::managed
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdspan = stdex::mdspan<T, Extents, LayoutPolicy, cuda::managed::accessor<AccessorPolicy>>;

    EMU_DEFINE_MDSPAN_ALIAS;

} // namespace emu::cuda::managed
