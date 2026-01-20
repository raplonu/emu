#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/host/accessor.hpp>

namespace emu::host
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdcontainer = emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, host::accessor<AccessorPolicy>
    >;

    EMU_DEFINE_MDCONTAINER_ALIAS;

} // namespace emu::host
