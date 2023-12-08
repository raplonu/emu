#pragma once

#include <emu/mdspan.hpp>
#include <emu/cuda.h>

#include <span>
#include <array>

namespace emu
{

namespace cuda
{

    template <typename ElementType, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<ElementType>>
    struct mdspan : protected stdex::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> {

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

        using base::base;

        ~mdspan() = default;

        constexpr mdspan(const mdspan&) = default;
        constexpr mdspan(mdspan&&) = default;

        constexpr mdspan& operator=(const mdspan&) noexcept = default;
        constexpr mdspan& operator=(mdspan&&) noexcept = default;

        template<typename... Indexes>
        constexpr auto operator()(Indexes... indexes) const noexcept -> decltype(auto) {
#if MDSPAN_USE_BRACKET_OPERATOR
            return base::operator[](indexes...);
#else
            return base::operator()(indexes...);
#endif
        }

    };


    template <class ElementType, class... Integrals>
        requires((std::is_convertible_v<Integrals, std::size_t> and ...) and sizeof...(Integrals) > 0)
    mdspan(ElementType *, Integrals...)
        -> mdspan<ElementType, dextents<std::size_t, sizeof...(Integrals)>>;

    template <class ElementType, class OtherIndexType, std::size_t N>
    mdspan(ElementType *, std::span<OtherIndexType, N>)
        -> mdspan<ElementType, dextents<std::size_t, N>>;

    template <class ElementType, class OtherIndexType, std::size_t N>
    mdspan(ElementType *, const std::array<OtherIndexType, N> &)
        -> mdspan<ElementType, dextents<std::size_t, N>>;

    template <class ElementType, class IndexType, std::size_t... ExtentsPack>
    mdspan(ElementType *, const extents<IndexType, ExtentsPack...> &)
        -> mdspan<ElementType, extents<IndexType, ExtentsPack...>>;

    template <class ElementType, class MappingType>
    mdspan(ElementType *, const MappingType &)
        -> mdspan<ElementType, typename MappingType::extents_type,
                  typename MappingType::layout_type>;

    template <class MappingType, class AccessorType>
    mdspan(const typename AccessorType::data_handle_type &, const MappingType &, const AccessorType &)
        -> mdspan<typename AccessorType::element_type,
                  typename MappingType::extents_type,
                  typename MappingType::layout_type, AccessorType>;

    template<typename ElementType> using mdspan_1d = mdspan<ElementType, _1d>;
    template<typename ElementType> using mdspan_2d = mdspan<ElementType, _2d>;
    template<typename ElementType> using mdspan_3d = mdspan<ElementType, _3d>;

    template<typename ElementType> using mdspan_1d_c = mdspan<ElementType, _1d>;
    template<typename ElementType> using mdspan_2d_c = mdspan<ElementType, _2d>;
    template<typename ElementType> using mdspan_3d_c = mdspan<ElementType, _3d>;

    template<typename ElementType> using mdspan_1d_f = mdspan<ElementType, _1d, layout_f>;
    template<typename ElementType> using mdspan_2d_f = mdspan<ElementType, _2d, layout_f>;
    template<typename ElementType> using mdspan_3d_f = mdspan<ElementType, _3d, layout_f>;

    template<typename ElementType> using mdspan_1d_s = mdspan<ElementType, _1d, layout_stride>;
    template<typename ElementType> using mdspan_2d_s = mdspan<ElementType, _2d, layout_stride>;
    template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, _3d, layout_stride>;

} // namespace cuda

} // namespace emu
