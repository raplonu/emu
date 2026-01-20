#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/cuda/managed/accessor.hpp>

namespace emu::cuda::managed
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    using mdcontainer = emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, cuda::managed::accessor<AccessorPolicy>
    >;

    EMU_DEFINE_MDCONTAINER_ALIAS;

} // namespace emu::cuda::managed
