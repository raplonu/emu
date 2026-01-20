#pragma once

#include <emu/concepts.hpp>
#include <emu/info.hpp>

#include <span>
#include <experimental/mdspan>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

    using std::dynamic_extent;

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

    using stdex::strided_slice;

    template<std::size_t N>
    using dims = dextents<std::size_t, N>;

    using d0 = dims<0>;
    using d1 = dims<1>;
    using d2 = dims<2>;
    using d3 = dims<3>;
    using d4 = dims<4>;

    template<typename>
    inline constexpr bool is_default_accessor_v = false;

    template<typename ElementType>
    inline constexpr bool is_default_accessor_v<stdex::default_accessor<ElementType>> = true;

namespace cpts
{

    template<typename Accessor>
    concept default_accessor = is_default_accessor_v<Accessor>;

} // namespace cpts

namespace detail
{

    template <typename T>
    struct is_extents : std::false_type {};

    template <typename IndexType, IndexType... Extents>
    struct is_extents<stdex::extents<IndexType, Extents...>> : std::true_type {};

} // namespace detail

    template <typename T>
    constexpr bool is_extents = detail::is_extents<T>::value;

namespace cpts
{

    template <typename T>
    concept mdspan = specialization_of<T, stdex::mdspan>;

    template <typename T>
    concept const_mdspan = mdspan<T> and std::is_const_v<typename T::element_type>;

    template <typename T>
    concept mutable_mdspan = mdspan<T> and (not std::is_const_v<typename T::element_type>);

    template <typename T>
    concept extents = is_extents<T>;

    template <typename T>
    concept mapping = requires (const T& m) {
        // TODO: complete the list...
        { m.required_span_size() } -> std::convertible_to<std::size_t>;

        { m.is_always_unique()     } -> std::convertible_to<bool>;
        { m.is_always_exhaustive() } -> std::convertible_to<bool>;
        { m.is_always_strided()    } -> std::convertible_to<bool>;
        { m.is_unique()            } -> std::convertible_to<bool>;
        { m.is_exhaustive()        } -> std::convertible_to<bool>;
        { m.is_strided()           } -> std::convertible_to<bool>;
    };

    template <typename T>
    // the only thing that determine a layout is the inner mapping type.
    concept layout = mapping< typename T::template mapping<std::experimental::extents<std::size_t>> >;


} // namespace cpts

    /**
     * @brief Select a submdspan from a mdspan. It return the template same type as the input mdspan.
     *
     * Contrary to stdex::submdspan this function does not requires to provide the full_extent for the non selected dimensions.
     *
     * @tparam MdSpan
     * @tparam SliceSpecifiers
     * @param src
     * @param slices
     * @return sub MdSpan
     */
    template <cpts::mdspan MdSpan, class... SliceSpecifiers>
    constexpr auto submdspan(
        const MdSpan &src, SliceSpecifiers... slices)
    {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            auto res = stdex::submdspan(src, slices..., ((void)Is, full_extent)...);

            if constexpr (requires { src.actual_from_base(res); })
                return src.actual_from_base(res);
            else
                return res;

        } (std::make_index_sequence<MdSpan::rank() - sizeof...(SliceSpecifiers)>{});
    }

    template<typename LayoutPolicy> inline const char* layout_name() { return "unknow"; }

    template<> inline const char* layout_name<layout_right >() { return "C/right"; }
    template<> inline const char* layout_name<layout_left  >() { return "F/left" ; }
    template<> inline const char* layout_name<layout_stride>() { return "stride" ; }

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

    inline auto format_extent(fmt::format_context::iterator it, size_t extent) {
        return (extent == dynamic_extent)
            ? fmt::format_to(it, "dyn")
            : fmt::format_to(it, "{}", extent);
    }

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

    template <cpts::mdspan MdSpan>
    struct info_t< MdSpan >
    {
        using type = MdSpan;

        using element_type = typename type::element_type;
        using mapping_type = typename type::mapping_type;

        info_t< mapping_type > mapping_info;

        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "mdspan<{}, ", emu::type_name<element_type>);
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

} // namespace emu

template<typename Extent>
struct fmt::formatter<emu::detail::static_extent_t<Extent>>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const emu::detail::static_extent_t<Extent>& s, FormatContext& ctx) const {
        auto it = ctx.out();
        if constexpr (Extent::rank() > 0) {
            it = emu::detail::format_extent(it, Extent::static_extent(0));
            for (std::size_t i = 1; i < Extent::rank(); ++i) {
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
    auto format(const emu::detail::extent_t<Mapping>& s, FormatContext& ctx) const {
        auto it = ctx.out();
        if constexpr (Mapping::extents_type::rank() > 0) {
            it = fmt::format_to(it, "{}", s.m.extents().extent(0));
            for (std::size_t i = 1; i < Mapping::extents_type::rank(); ++i)
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
    auto format(const emu::detail::stride_t<Mapping>& s, FormatContext& ctx) const {
        auto it = ctx.out();
        if constexpr (Mapping::extents_type::rank() > 0) {
            it = fmt::format_to(it, "{}", s.m.stride(0));
            for (std::size_t i = 1; i < Mapping::extents_type::rank(); ++i)
                it = fmt::format_to(it, "{}{}", s.sep, s.m.stride(i));
        }
        return it;
    }
};
