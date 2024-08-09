#pragma once

#include <emu/type_traits.hpp>
#include <emu/location_policy.hpp>
#include <emu/detail/basic_span.hpp>
#include <emu/mdspan.hpp>
#include <emu/info.hpp>
#include <emu/type_name.hpp>

#include <span>

namespace emu
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct span : emu::detail::basic_span<ElementType, Extent, no_location_policy, span<ElementType, Extent> >
    {
        using base = emu::detail::basic_span<ElementType, Extent, no_location_policy, span >;

        using base::base;

        template<typename OT, size_t OExtent>
        constexpr auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return span<OT, OExtent>(sp.data(), sp.size());
        }
    };

    EMU_DEFINE_SPAN_DEDUCTION_GUIDES

    template<typename T, std::size_t Extent>
    constexpr auto as_md(span<T, Extent> s) noexcept {
        return mdspan<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
    }

namespace spe
{
    template <typename T, size_t Extent>
    struct info_t< span<T, Extent> >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "std::span<{}, ", type_name<T>);
            it = detail::format_extent(it, Extent);
            return fmt::format_to(it, ">");
        }

        constexpr auto format_value(const span<T, Extent> &t, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "@{}[{}]", fmt::ptr(t.data()), t.size());
        }
    };

} // namespace spe

} // namespace emu
