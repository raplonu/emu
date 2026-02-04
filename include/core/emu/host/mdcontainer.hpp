#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/host/accessor.hpp>

namespace emu::host
{

    template<
        typename T, typename Extents,
        typename LayoutPolicy = layout_right,
        typename AccessorPolicy = default_accessor<T>
    >
    struct mdcontainer
        : emu::detail::basic_mdcontainer<
            T,
            Extents,
            LayoutPolicy,
            host::accessor<AccessorPolicy>
        >
    {
        using base = emu::detail::basic_mdcontainer<T, Extents, LayoutPolicy, host::accessor<AccessorPolicy>>;

        using base::base;

        constexpr mdcontainer() noexcept = default;

        friend constexpr void swap(mdcontainer& x, mdcontainer& y) noexcept {
            swap(static_cast<base&>(x), static_cast<base&>(y));
        }
    };

    EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES(mdcontainer);
    EMU_DEFINE_MDCONTAINER_ALIAS;

} // namespace emu::host
