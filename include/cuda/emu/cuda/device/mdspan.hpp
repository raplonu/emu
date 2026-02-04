#pragma once

#include <emu/detail/basic_mdspan.hpp>

#include <cuda/mdspan>

namespace emu::cuda::device
{

    // TODO: Find which is better: inheriting or using aliasing

    template<
        typename T, typename Extents,
        typename LayoutPolicy = layout_right,
        typename AccessorPolicy = default_accessor<T>
    >
    struct mdspan : stdex::mdspan<T, Extents, LayoutPolicy, device::accessor<AccessorPolicy>>
    {
        using base = stdex::mdspan<T, Extents, LayoutPolicy, device::accessor<AccessorPolicy>>;

        using base::base;

        constexpr mdspan() noexcept = default;

        friend constexpr void swap(mdspan& x, mdspan& y) noexcept {
            swap(static_cast<base&>(x), static_cast<base&>(y));
        }
    };

    EMU_DEFINE_MDSPAN_DEDUCTION_GUIDES(mdspan);
    EMU_DEFINE_MDSPAN_ALIAS;

} // namespace emu::cuda::device
