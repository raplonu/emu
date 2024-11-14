#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/host/location_policy.hpp>

namespace emu
{
namespace host
{

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer : emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, AccessorPolicy, host::location_policy,
        mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
    >
    {
        using base = emu::detail::basic_mdcontainer< T, Extents, LayoutPolicy, AccessorPolicy, host::location_policy, mdcontainer >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_MDCONTAINER_ALIAS

    EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES

} // namespace host

    // template<typename T, std::size_t Extent>
    // constexpr auto as_md(host::container<T, Extent> s) noexcept {
    //     return host::mdspan<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
    // }

} // namespace emu
