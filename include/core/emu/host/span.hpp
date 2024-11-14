#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_span.hpp>
#include <emu/host/mdspan.hpp>
#include <emu/mdalgo.hpp>

namespace emu
{

namespace host
{

    template <typename ElementType, size_t Extent>
    struct span : emu::detail::basic_span<ElementType, Extent, host::location_policy, span<ElementType, Extent> >
    {
        using base = emu::detail::basic_span<ElementType, Extent, host::location_policy, span >;

        using base::base;

        template<typename OT, size_t OExtent>
        constexpr auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return span<OT, OExtent>(sp.data(), sp.size());
        }
    };

    EMU_DEFINE_SPAN_DEDUCTION_GUIDES

} // namespace host

    template<typename T, std::size_t Extent>
    struct spe::md_converter<host::span<T, Extent>>
    {
        static constexpr auto convert(host::span<T, Extent> s) noexcept {
            return host::mdspan<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
        }
    };

} // namespace emu
