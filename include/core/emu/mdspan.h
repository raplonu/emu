#pragma once

#include <emu/type_traits.h>
#include <emu/concepts.hpp>
#include <emu/location.h>
#include <emu/span.h>

#include <experimental/mdspan>

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

    // template<cpts::mdspan M>
    // auto extents(const M& m)
    // {
    //     std::array<typename M::element_type, M::extents_type::rank()> res{};
    //     for (std::size_t i = 0; i < res.size(); ++i)
    //         res[i] = m.extents().extent(i);
    //     return res;
    // }

    // template<cpts::mdspan M>
    // auto strides(const M& m)
    // {
    //     std::array<typename M::element_type, M::extents_type::rank()> res{};
    //     for (std::size_t i = 0; i < res.size(); ++i)
    //         res[i] = m.stride(i);
    //     return res;
    // }

    template <typename T = std::size_t, typename Src>
    auto strides_vector(std::span<Src> extents, std::size_t word_size)
    {
        std::vector<T> res(extents.size());

        auto last = word_size;
        for (int i = extents.size() - 1; i >= 0; --i) {
            res[i] = last;
            last = static_cast<T>(extents[i]) * res[i];
        }
        return res;
    }

namespace detail
{

    template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<ElementType>>
    struct mdspan : stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> {

        using base = stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        using extents_type     = typename base::extents_type;
        using layout_type      = typename base::layout_type;
        using accessor_type    = typename base::accessor_type;
        using mapping_type     = typename base::mapping_type;
        using element_type     = typename base::element_type;
        using value_type       = typename base::value_type;
        using index_type       = typename base::index_type;
        using size_type        = typename base::size_type;
        using rank_type        = typename base::rank_type;
        using data_handle_type = typename base::data_handle_type;
        using reference        = typename base::reference;

        using location_type   = Location;

    // private:
    //     static constexpr auto DefaultAccessor = std::is_default_constructible_v<accessor_type>;
    //     static constexpr auto MappingConstructible = std::is_constructible_v<mapping_type, extents_type>;

    //     template<typename SizeType, std::size_t N>
    //     static constexpr auto array_match =
    //         std::is_convertible_v<SizeType, size_type> and
    //         N == extents_type::rank_dynamic();

    public:


        constexpr mdspan() = default;

        template<typename... SizeTypes>
        explicit constexpr mdspan(data_handle_type p, SizeTypes... dynamic_extents)
            : base(p, dynamic_extents...), location_()
        {}

        template<typename... SizeTypes>
        explicit constexpr mdspan(data_handle_type p, location_type location, SizeTypes... dynamic_extents)
            : base(p, dynamic_extents...), location_(location)
        {}


        template<typename SizeType, size_t N>
        constexpr mdspan(data_handle_type p, const std::array<SizeType, N>& dynamic_extents, location_type location = {})
            : base(p, dynamic_extents), location_(location)
        {}

        template<typename SizeType, size_t N>
        constexpr mdspan(data_handle_type p, std::span<SizeType, N> dynamic_extents, location_type location = {})
            : base(p, dynamic_extents), location_(location)
        {}

        constexpr mdspan(data_handle_type p, const extents_type& exts, location_type location = {})
            : base(p, exts), location_(location)
        {}

        constexpr mdspan(data_handle_type p, const mapping_type& m, location_type location = {})
            : base(p, m), location_(location)
        {}

        constexpr mdspan(data_handle_type p, const mapping_type& m, const accessor_type& a, location_type location = {})
            : base(p, m, a), location_(location)
        { }

        template<typename OtherElementType, typename OtherExtents, typename OtherLayoutPolicy, typename OtherAccessor>
        constexpr mdspan(const mdspan<OtherElementType, Location, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other)
            : base(other), location_(other.location)
        {}

        template<typename OtherElementType, typename OtherExtents, typename OtherLayoutPolicy, typename OtherAccessor>
        constexpr mdspan(const stdex::mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>& other, location_type location = {})
            : base(other), location_(location)
        {}

        ~mdspan() = default;


        constexpr mdspan(const mdspan&) = default;
        constexpr mdspan(mdspan&&) = default;

        constexpr mdspan& operator=(const mdspan&) noexcept = default;
        constexpr mdspan& operator=(mdspan&&) noexcept = default;

        // template<typename... SliceSpecs>
        // constexpr auto subspan(SliceSpecs... slices) const noexcept {
        //     return as_span_t(detail::stdex::submdspan(*this, EMU_FWD(slices)...));
        // }

        constexpr location_type location() const noexcept {
            return location_;
        }

        template<class OtherElementType, class OtherExtents, class OtherLayoutPolicy, class OtherAccessor>
        constexpr auto as_mdspan(stdex::mdspan<OtherElementType, OtherExtents, OtherLayoutPolicy, OtherAccessor>&& m) const noexcept {
            return mdspan<OtherElementType, location_type, OtherExtents, OtherLayoutPolicy, OtherAccessor>(mv(m), location());
        }

    private:
        location_type location_;
    };

    template <class CArray, typename LocationType>
        requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)
    mdspan(CArray &)
        -> mdspan<std::remove_all_extents_t<CArray>, LocationType,
                  extents<std::size_t, std::extent_v<CArray, 0>>>;

    template <class Pointer, class LocationType>
        requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)
    mdspan(Pointer &&)
        -> mdspan<std::remove_pointer_t<std::remove_reference_t<Pointer>>, LocationType,
                  extents<size_t>>;

    template <class ElementType, class LocationType, class... Integrals>
        requires((std::is_convertible_v<Integrals, std::size_t> && ...) &&
                 sizeof...(Integrals) > 0)
    explicit mdspan(ElementType *, Integrals...)
        -> mdspan<ElementType, LocationType, dextents<std::size_t, sizeof...(Integrals)>>;

    template <class ElementType, class LocationType, class... Integrals>
        requires((std::is_convertible_v<Integrals, std::size_t> && ...) &&
                 sizeof...(Integrals) > 0)
    explicit mdspan(ElementType *, LocationType, Integrals...)
        -> mdspan<ElementType, LocationType, dextents<std::size_t, sizeof...(Integrals)>>;

    template <class ElementType, class LocationType, class OtherIndexType, std::size_t N>
    mdspan(ElementType *, std::span<OtherIndexType, N>)
        -> mdspan<ElementType, LocationType, dextents<std::size_t, N>>;

    template <class ElementType, class LocationType, class OtherIndexType, std::size_t N>
    mdspan(ElementType *, const std::array<OtherIndexType, N> &)
        -> mdspan<ElementType, LocationType, dextents<std::size_t, N>>;

    template <class ElementType, class LocationType, class IndexType, std::size_t... ExtentsPack>
    mdspan(ElementType *, const extents<IndexType, ExtentsPack...> &)
        -> mdspan<ElementType, LocationType, extents<IndexType, ExtentsPack...>>;

    template <class ElementType, class LocationType, class MappingType>
    mdspan(ElementType *, const MappingType &)
        -> mdspan<ElementType, LocationType, typename MappingType::extents_type,
                  typename MappingType::layout_type>;

    template <class MappingType, class LocationType, class AccessorType>
    mdspan(const typename AccessorType::data_handle_type &, const MappingType &, const AccessorType &)
        -> mdspan<typename AccessorType::element_type,
                  LocationType,
                  typename MappingType::extents_type,
                  typename MappingType::layout_type, AccessorType>;

} // namespace detail

    template<typename ET, typename LO, typename EXT, typename LP, typename AP, typename... SliceSpecs>
    constexpr auto submdspan(const detail::mdspan<ET, LO, EXT, LP, AP>& m, SliceSpecs... slices) noexcept {
        return m.as_mdspan(stdex::submdspan(m, slices...));
    }

    template <typename ElementType, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<ElementType>>
    using mdspan = detail::mdspan<ElementType, location::host, Extents, LayoutPolicy, AccessorPolicy>;

    template<typename ElementType> using mdspan_1d   = mdspan<ElementType, dextents<std::size_t, 1>>;
    template<typename ElementType> using mdspan_2d   = mdspan<ElementType, dextents<std::size_t, 2>>;
    template<typename ElementType> using mdspan_3d   = mdspan<ElementType, dextents<std::size_t, 3>>;

    template<typename ElementType> using mdspan_1d_c = mdspan<ElementType, dextents<std::size_t, 1>>;
    template<typename ElementType> using mdspan_2d_c = mdspan<ElementType, dextents<std::size_t, 2>>;
    template<typename ElementType> using mdspan_3d_c = mdspan<ElementType, dextents<std::size_t, 3>>;

    template<typename ElementType> using mdspan_1d_f = mdspan<ElementType, dextents<std::size_t, 1>, layout_f>;
    template<typename ElementType> using mdspan_2d_f = mdspan<ElementType, dextents<std::size_t, 2>, layout_f>;
    template<typename ElementType> using mdspan_3d_f = mdspan<ElementType, dextents<std::size_t, 3>, layout_f>;

    template<typename ElementType> using mdspan_1d_s = mdspan<ElementType, dextents<std::size_t, 1>, layout_stride>;
    template<typename ElementType> using mdspan_2d_s = mdspan<ElementType, dextents<std::size_t, 2>, layout_stride>;
    template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, dextents<std::size_t, 3>, layout_stride>;

} // namespace emu
