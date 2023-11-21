#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/span.hpp>

#include <experimental/mdspan>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/core/demangle.hpp>

namespace emu
{

    namespace stdex = std::experimental;

    using stdex::layout_right;
    using stdex::layout_left;
    using stdex::layout_stride;

    using layout_c = layout_right;
    using layout_f = layout_left;

    using layout_row    = layout_right;
    using layout_column = layout_left;

    using stdex::default_accessor;

    using stdex::dextents;

    using stdex::extents;

    using stdex::full_extent_t;
    using stdex::full_extent;

    using stdex::mdspan;

    template<std::size_t N>
    using _nd = dextents<std::size_t, N>;

    using _0d = _nd<0>;
    using _1d = _nd<1>;
    using _2d = _nd<2>;
    using _3d = _nd<3>;

    template<typename ElementType> using mdspan_0d   = mdspan<ElementType, _0d>;

    template<typename ElementType> using mdspan_1d   = mdspan<ElementType, _1d>;
    template<typename ElementType> using mdspan_2d   = mdspan<ElementType, _2d>;
    template<typename ElementType> using mdspan_3d   = mdspan<ElementType, _3d>;

    template<typename ElementType> using mdspan_1d_c = mdspan<ElementType, _1d>;
    template<typename ElementType> using mdspan_2d_c = mdspan<ElementType, _2d>;
    template<typename ElementType> using mdspan_3d_c = mdspan<ElementType, _3d>;

    template<typename ElementType> using mdspan_1d_f = mdspan<ElementType, _1d, layout_f>;
    template<typename ElementType> using mdspan_2d_f = mdspan<ElementType, _2d, layout_f>;
    template<typename ElementType> using mdspan_3d_f = mdspan<ElementType, _3d, layout_f>;

    template<typename ElementType> using mdspan_1d_s = mdspan<ElementType, _1d, layout_stride>;
    template<typename ElementType> using mdspan_2d_s = mdspan<ElementType, _2d, layout_stride>;
    template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, _3d, layout_stride>;

    using stdex::submdspan;

    template<typename LayoutPolicy> inline const char* layout_name();
    template<> inline const char* layout_name<layout_right>() { return "C/right"; }
    template<> inline const char* layout_name<layout_left>() { return "F/left"; }
    template<> inline const char* layout_name<layout_stride>() { return "stride"; }

    template<typename T, std::size_t Extent>
    constexpr auto as_mdspan(span<T, Extent> s) noexcept {
        return mdspan_1d<T>{s.data(), s.size()};
    }

    // template<typename T, std::size_t Extent>
    // constexpr auto as_span(mdspan<T, Extent> s) noexcept {
    //     return mdspan_1d<T>{s.data(), s.size()};
    // }

namespace spe
{
    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct info_t< mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> >
    {
        using type = mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        constexpr auto name(fmt::format_context::iterator it) const {
            return []<std::size_t...Is>(fmt::format_context::iterator it, std::index_sequence<Is...>) {
                it = fmt::format_to(it, "mdspan<{}, [", boost::core::demangle(typeid(ElementType).name()));

                bool first = true;
                for (auto e : std::array{type::static_extent(Is)...}) {
                    if (not first) it = fmt::format_to(it, ", "); else first = false;
                    it = detail::format_extent(it, e);
                }

                return fmt::format_to(it, "], {}>", layout_name<LayoutPolicy>());

            }(it, std::make_index_sequence<Extents::rank()>{});
        }

        constexpr auto format_to(const type &t, fmt::format_context::iterator it) const {
            return []<std::size_t... Is>(const type &t, fmt::format_context::iterator it, std::index_sequence<Is...>) {
                it = fmt::format_to(it, "@{}{}", fmt::ptr(t.data_handle()), std::array{t.extent(Is)...});
                if constexpr (std::same_as<LayoutPolicy, layout_stride>) {
                    it = fmt::format_to(it, "[{}]", std::array{t.stride(Is)...});
                }
                return it;
            }(t, it, std::make_index_sequence<Extents::rank()>{});
        }
    };

} // namespace spe

} // namespace emu
