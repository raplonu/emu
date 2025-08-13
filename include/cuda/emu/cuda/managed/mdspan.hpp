#pragma once
#include <emu/type_traits.hpp>
#include <emu/detail/basic_mdspan.hpp>
#include <emu/cuda/managed/location_policy.hpp>

namespace emu::cuda::managed
{

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdspan : emu::detail::basic_mdspan<
        T, Extents, LayoutPolicy, AccessorPolicy, cuda::managed_location_policy,
        mdspan<T, Extents, LayoutPolicy, AccessorPolicy>
    >
    {
        using base = emu::detail::basic_mdspan< T, Extents, LayoutPolicy, AccessorPolicy, cuda::managed_location_policy, mdspan >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md);
        }
    };

    EMU_DEFINE_MDSPAN_DEDUCTION_GUIDES

    EMU_DEFINE_MDSPAN_ALIAS

} // namespace emu::cuda::managed
