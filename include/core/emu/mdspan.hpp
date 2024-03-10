#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/span.hpp>
#include <emu/type_name.hpp>

#include <experimental/mdspan>

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string_view>

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

    template<std::size_t N>
    using _nd = dextents<std::size_t, N>;

    using _0d = _nd<0>;
    using _1d = _nd<1>;
    using _2d = _nd<2>;
    using _3d = _nd<3>;
    using _4d = _nd<4>;

    using stdex::mdspan;

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

    template<typename LayoutPolicy> inline const char* layout_name() { return "unknow"; }
    template<> inline const char* layout_name<layout_right>() { return "C/right"; }
    template<> inline const char* layout_name<layout_left>() { return "F/left"; }
    template<> inline const char* layout_name<layout_stride>() { return "stride"; }

    // not sure about that...
    template <typename T>
    constexpr auto as_mdspan(T& t) noexcept {
        return mdspan_0d<T>{&t};
    }

    template<typename T, std::size_t Extent>
    constexpr auto as_mdspan(span<T, Extent> s) noexcept {
        return mdspan_1d<T>{s.data(), s.size()};
    }

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    constexpr auto as_mdspan(mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> md) noexcept {
        return md;
    }

namespace detail
{

    template<typename Extent>
    struct static_extent_t{
        std::string_view sep;
    };

    template<typename Mapping>
    struct extent_t{
        const Mapping& m;
        std::string_view sep;
    };

    template<typename Mapping>
    struct stride_t{
        const Mapping& m;
        std::string_view sep;
    };

} // namespace detail

    template<typename Map>
    auto static_extent(std::string_view sep = ", ") {
        return detail::static_extent_t<typename Map::extents_type>{sep};
    }

    template<cpts::mdspan MdSpan>
    auto static_extent(std::string_view sep = ", ") {
        return detail::static_extent_t<typename MdSpan::extents_type>{sep};
    }

    template<typename Map>
    auto extent(const Map& m, std::string_view sep = ", ") {
        return detail::extent_t<Map>{m, sep};
    }

    template<cpts::mdspan MdSpan>
    auto extent(const MdSpan& m, std::string_view sep = ", ") {
        return detail::extent_t{m.mapping(), sep};
    }

    template<typename Map>
    auto stride(const Map& m, std::string_view sep = ", ") {
        return detail::stride_t{m, sep};
    }

    template<cpts::mdspan MdSpan>
    auto stride(const MdSpan& m, std::string_view sep = ", ") {
        return detail::stride_t{m.mapping(), sep};
    }

namespace detail
{
    template<cpts::mdspan MdSpan>
    auto c_contigous(const MdSpan& mdspan) {
        std::size_t size = 1;
        for (std::size_t i = MdSpan::rank(); i == 0; --i) {
            if (mdspan.stride(i - 1) != size) {
                return false;
            }
            size *= mdspan.extent(i - 1);
        }
        return true;
    }

    template<cpts::mdspan MdSpan>
    auto f_contigous(const MdSpan& mdspan) {
        std::size_t size = 1;
        for (std::size_t i = 0; i < MdSpan::rank(); ++i) {
            if (mdspan.stride(i) != size) {
                return false;
            }
            size *= mdspan.extent(i);
        }
        return true;
    }

} // namespace detail

namespace spe
{
    template<cpts::mapping Mapping>
        requires (std::same_as<typename Mapping::layout_type, layout_right> or std::same_as<typename Mapping::layout_type, layout_left>)
    struct info_t< Mapping > {

        constexpr auto format_type(fmt::format_context::iterator it) const {
            return fmt::format_to(it, "[{}], {}", emu::static_extent<Mapping>(), layout_name<typename Mapping::layout_type>());
        }

        constexpr auto format_value(const Mapping &m, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "[{}]", emu::extent(m));
        }
    };

    template<cpts::mapping Mapping>
        requires (std::same_as<typename Mapping::layout_type, layout_stride>)
    struct info_t< Mapping > {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            return fmt::format_to(it, "[{}], {}", emu::static_extent<Mapping>(), layout_name<typename Mapping::layout_type>());
        }

        constexpr auto format_value(const Mapping &m, fmt::format_context::iterator it) const {
            // perform both compile time and runtime check for exhaustive mapping.
            if constexpr (m.is_always_exhaustive()) /* and */ if (m.is_exhaustive())
                    return fmt::format_to(it, "[{}]", emu::extent(m));

            // otherwise, the mapping is not exhaustive, so we print the strides.
            return fmt::format_to(it, "[{}, {}]", emu::extent(m), emu::stride(m));
        }
    };

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct info_t< mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> >
    {
        using type = mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        using mapping_type = typename type::mapping_type;

        info_t< mapping_type > mapping_info;

        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "mdspan<{}, ", emu::type_name<ElementType>);
            it = mapping_info.format_type(it);
            return fmt::format_to(it, ">");
        }

        constexpr auto format_value(const type &t, fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "@ {}, ", fmt::ptr(t.data_handle()));
            it = mapping_info.format_value(t.mapping(), it);
            return it;
        }
    };

} // namespace spe

    /**
     * @brief Select a submdspan from a mdspan.
     *
     * Contrary to stdex::submdspan this function does not requires to provide the full_extent for the non selected dimensions.
     *
     * @tparam ElementType
     * @tparam Extents
     * @tparam LayoutPolicy
     * @tparam AccessorPolicy
     * @tparam SliceSpecifiers
     * @param src The source mdspan.
     * @param slices The slices to select.
     * @return A new mdspan with the selected slices.
     */
    template <class ElementType, class Extents, class LayoutPolicy,
          class AccessorPolicy, class... SliceSpecifiers>
    constexpr auto submdspan(
        const mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> &src,
        SliceSpecifiers... slices)
    {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return stdex::submdspan(src, slices..., ((void)Is, full_extent)...);
        } (std::make_index_sequence<Extents::rank() - sizeof...(SliceSpecifiers)>{});
    }

} // namespace emu

template<typename Extent>
struct fmt::formatter<emu::detail::static_extent_t<Extent>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const emu::detail::static_extent_t<Extent>& s, FormatContext& ctx) {
        auto it = ctx.out();
        if constexpr (Extent::rank() > 0) {
            it = emu::detail::format_extent(it, Extent::static_extent(0));
            for (auto i = 1; i < Extent::rank(); ++i) {
                it = fmt::format_to(it, "{}", s.sep);
                it = emu::detail::format_extent(it, Extent::static_extent(i));
            }
        }
        return it;
    }
};


template<typename Mapping>
struct fmt::formatter<emu::detail::extent_t<Mapping>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const emu::detail::extent_t<Mapping>& s, FormatContext& ctx) {
        auto it = ctx.out();
        if constexpr (Mapping::extents_type::rank() > 0) {
            it = fmt::format_to(it, "{}", s.m.extents().extent(0));
            for (auto i = 1; i < Mapping::extents_type::rank(); ++i)
                it = fmt::format_to(it, "{}{}", s.sep, s.m.extents().extent(i));
        }
        return it;
    }
};

template<typename Mapping>
struct fmt::formatter<emu::detail::stride_t<Mapping>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    auto format(const emu::detail::stride_t<Mapping>& s, FormatContext& ctx) {
        auto it = ctx.out();
        if constexpr (Mapping::extents_type::rank() > 0) {
            it = fmt::format_to(it, "{}", s.m.stride(0));
            for (auto i = 1; i < Mapping::extents_type::rank(); ++i)
                it = fmt::format_to(it, "{}{}", s.sep, s.m.stride(i));
        }
        return it;
    }
};
