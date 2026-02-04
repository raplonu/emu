#pragma once

#include <emu/concepts.hpp>

#include <emu/detail/mdspan_types.hpp>

namespace emu
{

} // namespace emu

/**
 * @brief Utility macro to easily declare a set of aliases to reduce friction using `mdspan`.
 *
 */
#define EMU_DEFINE_MDSPAN_ALIAS                                                            \
template<typename ElementType> using mdspan_0d   = mdspan<ElementType, d0>;                \
                                                                                           \
template<typename ElementType> using mdspan_1d   = mdspan<ElementType, d1>;                \
template<typename ElementType> using mdspan_2d   = mdspan<ElementType, d2>;                \
template<typename ElementType> using mdspan_3d   = mdspan<ElementType, d3>;                \
                                                                                           \
template<typename ElementType> using mdspan_1d_c = mdspan<ElementType, d1>;                \
template<typename ElementType> using mdspan_2d_c = mdspan<ElementType, d2>;                \
template<typename ElementType> using mdspan_3d_c = mdspan<ElementType, d3>;                \
                                                                                           \
template<typename ElementType> using mdspan_1d_f = mdspan<ElementType, d1, layout_f>;      \
template<typename ElementType> using mdspan_2d_f = mdspan<ElementType, d2, layout_f>;      \
template<typename ElementType> using mdspan_3d_f = mdspan<ElementType, d3, layout_f>;      \
                                                                                           \
template<typename ElementType> using mdspan_1d_s = mdspan<ElementType, d1, layout_stride>; \
template<typename ElementType> using mdspan_2d_s = mdspan<ElementType, d2, layout_stride>; \
template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, d3, layout_stride>

/**
 * @brief Utility macro to easily declare mdspan deduction guides.
 *
 */
#define EMU_DEFINE_MDSPAN_DEDUCTION_GUIDES(mdspan_type)                                      \
/* ############################################## */                                         \
/* # Sized range deduction guide (non standard) # */                                         \
/* ############################################## */                                         \
                                                                                             \
template< emu::cpts::contiguous_sized_range Range >                                          \
    requires (not std::is_array_v<Range>)                                                    \
mdspan_type( Range&& )                                                                       \
    -> mdspan_type< rm_ref< std::ranges::range_reference_t< rm_ref<Range> > >,               \
                extents<std::size_t, 1>>;                                                    \
                                                                                             \
/* #################################### */                                                   \
/* # Static array deduction guide (1) # */                                                   \
/* #################################### */                                                   \
                                                                                             \
template< class CArray >                                                                     \
    requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)                            \
mdspan_type( CArray& )                                                                       \
    -> mdspan_type< std::remove_all_extents_t<CArray>,                                       \
                extents<std::size_t, std::extent_v<CArray, 0>>>;                             \
                                                                                             \
/* ############################### */                                                        \
/* # pointer deduction guide (2) # */                                                        \
/* ############################### */                                                        \
                                                                                             \
template< class Pointer >                                                                    \
    requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)                            \
mdspan_type( Pointer&& )                                                                     \
    -> mdspan_type< std::remove_pointer_t<std::remove_reference_t<Pointer>>,                 \
                extents<size_t>>;                                                            \
                                                                                             \
/* ###################################### */                                                 \
/* # variadic shape deduction guide (3) # */                                                 \
/* ###################################### */                                                 \
                                                                                             \
template< class ElementType, class... Integrals >                                            \
    requires((std::is_convertible_v<Integrals, std::size_t> && ...) and                      \
                sizeof...(Integrals) > 0)                                                    \
explicit mdspan_type( ElementType*, Integrals... )                                           \
    -> mdspan_type<ElementType, dextents<std::size_t, sizeof...(Integrals)>>;                \
                                                                                             \
template< std::ranges::contiguous_range Range, class... Integrals >                          \
    requires((std::is_convertible_v<Integrals, std::size_t> && ...) and                      \
                sizeof...(Integrals) > 0)                                                    \
explicit mdspan_type( Range&&, Integrals... )                                                \
    -> mdspan_type<range_cv_value<Range>, dextents<std::size_t, sizeof...(Integrals)>>;      \
                                                                                             \
/* ############################ */                                                           \
/* # span deduction guide (4) # */                                                           \
/* ############################ */                                                           \
                                                                                             \
template< class ElementType, class OtherIndexType, std::size_t N >                           \
mdspan_type( ElementType*, std::span<OtherIndexType, N> )                                    \
    -> mdspan_type<ElementType, dextents<std::size_t, N>>;                                   \
                                                                                             \
template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >         \
mdspan_type( Range&&, std::span<OtherIndexType, N> )                                         \
    -> mdspan_type<range_cv_value<Range>, dextents<std::size_t, N>>;                         \
                                                                                             \
/* ############################# */                                                          \
/* # array deduction guide (5) # */                                                          \
/* ############################# */                                                          \
                                                                                             \
template< class ElementType, class OtherIndexType, std::size_t N >                           \
mdspan_type( ElementType*, const std::array<OtherIndexType, N>& )                            \
    -> mdspan_type<ElementType, dextents<std::size_t, N>>;                                   \
                                                                                             \
template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >         \
mdspan_type( Range&&, const std::array<OtherIndexType, N>& )                                 \
    -> mdspan_type<range_cv_value<Range>, dextents<std::size_t, N>>;                         \
                                                                                             \
/* ############################## */                                                         \
/* # extent deduction guide (6) # */                                                         \
/* ############################## */                                                         \
                                                                                             \
template< class ElementType, class IndexType, std::size_t... ExtentsPack >                   \
mdspan_type( ElementType*, const extents<IndexType, ExtentsPack...>& )                       \
    -> mdspan_type<ElementType, extents<IndexType, ExtentsPack...>>;                         \
                                                                                             \
template< std::ranges::contiguous_range Range, class IndexType, std::size_t... ExtentsPack > \
mdspan_type( Range&&, const extents<IndexType, ExtentsPack...>& )                            \
    -> mdspan_type<range_cv_value<Range>, extents<IndexType, ExtentsPack...>>;               \
                                                                                             \
/* ############################### */                                                        \
/* # mapping deduction guide (7) # */                                                        \
/* ############################### */                                                        \
                                                                                             \
template< class ElementType, class MappingType >                                             \
mdspan_type( ElementType*, const MappingType& )                                              \
    -> mdspan_type<ElementType, typename MappingType::extents_type,                          \
                typename MappingType::layout_type>;                                          \
                                                                                             \
template< std::ranges::contiguous_range Range, class MappingType >                           \
mdspan_type( Range&&, const MappingType& )                                                   \
    -> mdspan_type<range_cv_value<Range>, typename MappingType::extents_type,                \
                typename MappingType::layout_type>;                                          \
                                                                                             \
/* ############################################ */                                           \
/* # mapping and accessor deduction guide (8) # */                                           \
/* ############################################ */                                           \
                                                                                             \
template< class MappingType, class AccessorType >                                            \
mdspan_type( const typename AccessorType::data_handle_type&, const MappingType&,             \
        const AccessorType& )                                                                \
    -> mdspan_type<typename AccessorType::element_type,                                      \
                typename MappingType::extents_type,                                          \
                typename MappingType::layout_type, AccessorType>;                            \
                                                                                             \
template< std::ranges::contiguous_range Range, class MappingType, class AccessorType >       \
mdspan_type( Range&&, const MappingType&,                                                    \
        const AccessorType& )                                                                \
    -> mdspan_type<range_cv_value<Range>,                                                    \
                typename MappingType::extents_type,                                          \
                typename MappingType::layout_type, AccessorType>
