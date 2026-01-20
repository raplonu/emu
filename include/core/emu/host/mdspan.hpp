#pragma once

#include <emu/detail/basic_mdspan.hpp>
#include <emu/host/accessor.hpp>

namespace emu::host
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdspan = stdex::mdspan<T, Extents, LayoutPolicy, host::accessor<AccessorPolicy>>;

    EMU_DEFINE_MDSPAN_ALIAS;

} // namespace emu::host
