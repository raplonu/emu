#pragma once

// #include <emu/type_traits.hpp>
#include <emu/concepts.hpp>

// #include <emu/assert.hpp>
// #include <emu/type_name.hpp>
#include <emu/detail/mdspan_types.hpp>

// #include <string_view>
// #include <ranges>

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
template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, d3, layout_stride>;


// namespace detail
// {

//     // template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename LocationPolicy, typename ActualType>
//     // struct basic_mdspan : stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>
//     // {

//     //     using base = stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>;

//     //     using extents_type     = typename base::extents_type;
//     //     using layout_type      = typename base::layout_type;
//     //     using accessor_type    = typename base::accessor_type;
//     //     using mapping_type     = typename base::mapping_type;
//     //     using element_type     = typename base::element_type;
//     //     using value_type       = typename base::value_type;
//     //     using index_type       = typename base::index_type;
//     //     using size_type        = typename base::size_type;
//     //     using rank_type        = typename base::rank_type;
//     //     using data_handle_type = typename base::data_handle_type;
//     //     using reference        = typename base::reference;

//     //     using location_type = LocationPolicy;
//     //     using actual_type = ActualType;

//     //     template <typename Type>
//     //     inline static constexpr bool validate_source = location_type::template validate_source<Type>;

//     //     using base::base;

//     //     // ##########################################
//     //     // # Sized range constructor (non standard) #
//     //     // ##########################################

//     //     template< cpts::contiguous_sized_range Range >
//     //         requires validate_source<Range>
//     //              and (base::rank() == 1)
//     //              and (std::ranges::borrowed_range<Range> or is_const<element_type>)
//     //     constexpr explicit basic_mdspan( Range&& r )
//     //         : base( std::ranges::data(r), std::ranges::size(r) )
//     //     {}


//     //     // ##################################
//     //     // # Variadic shape constructor (2) #
//     //     // ##################################

//     //     template< std::ranges::contiguous_range Range, class... OtherIndexTypes >
//     //         requires validate_source<Range>
//     //             and (std::convertible_to<OtherIndexTypes, size_type> and ...)
//     //             and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
//     //             and (sizeof...(OtherIndexTypes) == base::rank_dynamic() or sizeof...(OtherIndexTypes) == base::rank())
//     //     constexpr explicit basic_mdspan( Range&& r, OtherIndexTypes... exts )
//     //         : base( std::ranges::data(r), exts... )
//     //     {
//     //         check_range_size(r);

//     //     }


//     //     // ##############################
//     //     // # Span shape constructor (3) #
//     //     // ##############################

//     //     template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >
//     //         requires validate_source<Range>
//     //     constexpr explicit(N != base::rank_dynamic())
//     //     basic_mdspan( Range&& r, std::span<OtherIndexType, N> exts )
//     //         : base( std::ranges::data(r), exts )
//     //     {
//     //         check_range_size(r);
//     //     }


//     //     // ###############################
//     //     // # Array shape constructor (4) #
//     //     // ###############################

//     //     template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >
//     //         requires validate_source<Range>
//     //     constexpr explicit(N != base::rank_dynamic())
//     //     basic_mdspan( Range&& r, const std::array<OtherIndexType, N>& exts )
//     //         : base( std::ranges::data(r), exts )
//     //     {
//     //         check_range_size(r);
//     //     }


//     //     // ##########################
//     //     // # extent constructor (5) #
//     //     // ##########################

//     //     template< std::ranges::contiguous_range Range>
//     //         requires validate_source<Range>
//     //     constexpr basic_mdspan( Range&& r, const extents_type& ext )
//     //         : base( std::ranges::data(r), ext )
//     //     {
//     //         check_range_size(r);
//     //     }


//     //     // ###########################
//     //     // # mapping constructor (6) #
//     //     // ###########################

//     //     template< std::ranges::contiguous_range Range>
//     //         requires validate_source<Range>
//     //     constexpr basic_mdspan( Range&& r, const mapping_type& m )
//     //         : base( std::ranges::data(r), m )
//     //     {
//     //         check_range_size(r);
//     //     }


//     //     // ########################################
//     //     // # mapping and accessor constructor (7) #
//     //     // ########################################

//     //     template< std::ranges::contiguous_range Range>
//     //         requires validate_source<Range>
//     //     constexpr basic_mdspan( Range&& r, const mapping_type& m,
//     //               const accessor_type& a )
//     //         : base( std::ranges::data(r), m, a )
//     //     {
//     //         check_range_size(r);
//     //     }


//     //     // ################################
//     //     // # other mdspan constructor (8) #
//     //     // ################################

//     //     template< class OtherElementType, class OtherExtents,
//     //               class OtherLayoutPolicy, class OtherAccessor,
//     //               class OActualType,
//     //               class OMapping = typename OtherLayoutPolicy::template mapping<OtherExtents> >
//     //     constexpr explicit(
//     //         not std::is_convertible_v<
//     //             const OMapping&,
//     //             mapping_type
//     //         >
//     //      or not std::is_convertible_v<const OtherAccessor&, accessor_type>
//     //     )
//     //     basic_mdspan( const basic_mdspan<OtherElementType, OtherExtents,
//     //                          OtherLayoutPolicy, OtherAccessor,
//     //                          location_type, OActualType>& other )
//     //         : base( other )
//     //     {}

//     //     template< class OtherElementType, class OtherExtents,
//     //               class OtherLayoutPolicy, class OtherAccessor,
//     //               class OMapping = typename OtherLayoutPolicy::template mapping<OtherExtents> >
//     //     constexpr explicit(
//     //         not std::is_convertible_v<
//     //             const OMapping&,
//     //             mapping_type
//     //         >
//     //         or not std::is_convertible_v<const OtherAccessor&, accessor_type>
//     //     )
//     //     basic_mdspan( const stdex::mdspan<OtherElementType, OtherExtents,
//     //                          OtherLayoutPolicy, OtherAccessor>& other )
//     //         : base( other )
//     //     {}

//     //     constexpr basic_mdspan( const basic_mdspan& rhs ) = default;
//     //     constexpr basic_mdspan( basic_mdspan&& rhs ) = default;

//     //     constexpr basic_mdspan& operator=(const basic_mdspan&) = default;
//     //     constexpr basic_mdspan& operator=(basic_mdspan&&) = default;

//     //     constexpr ~basic_mdspan() = default;

//     //     template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
//     //     constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
//     //         return actual_type::from_mdspan(md);
//     //     }

//     // protected:

    //     template<typename Range>
    //     constexpr void check_range_size( [[maybe_unused]] Range&& r ) const
    //     {
    //         if constexpr (std::ranges::sized_range<Range>) {
    //             EMU_ASSERT_MSG( std::ranges::size(r) == base::size(), "input range and mdspan size mismatch" );
    //         }
    //     }

//     // };

// } // namespace detail


    // inline size_t compute_memory_size(span<const size_t> shape, span<const size_t> strides = {}) {
    //     size_t size = 0;
    //     size_t stride = 1;

    //     for (size_t i = shape.size(); i-- > 0;) {
    //         size += (shape[i] - 1) * (strides.empty() ? stride : strides[i]);
    //         stride *= shape[i];
    //     }

    //     return size + 1;
    // }

    // size_t compute_memory_size(std::span<const size_t> shape, std::span<const ptrdiff_t> strides = {}) {
    //     size_t size = 0;
    //     ptrdiff_t stride = 1;

    //     for (size_t i = shape.size(); i-- > 0;) {
    //         ptrdiff_t effective_stride = strides.empty() ? stride : strides[i];
    //         size += (shape[i] - 1) * std::abs(effective_stride);
    //         stride *= shape[i];
    //     }

    //     return size + 1;
    // }


// } // namespace emu




// #define EMU_DEFINE_MDSPAN_DEDUCTION_GUIDES                                            \
// /* ############################################## */                                  \
// /* # Sized range deduction guide (non standard) # */                                  \
// /* ############################################## */                                  \
//                                                                                       \
// template< emu::cpts::contiguous_sized_range Range >                                        \
//     requires (not std::is_array_v<Range>)                                             \
// mdspan( Range&& )                                                                     \
//     -> mdspan< rm_ref< std::ranges::range_reference_t< rm_ref<Range> > >,             \
//                 extents<std::size_t, 1>>;                                        \
//                                                                                       \
// /* #################################### */                                            \
// /* # Static array deduction guide (1) # */                                            \
// /* #################################### */                                            \
//                                                                                       \
// template< class CArray >                                                              \
//     requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)                     \
// mdspan( CArray& )                                                                     \
//     -> mdspan< std::remove_all_extents_t<CArray>,                                     \
//                 extents<std::size_t, std::extent_v<CArray, 0>>>;                 \
//                                                                                       \
// /* ############################### */                                                 \
// /* # pointer deduction guide (2) # */                                                 \
// /* ############################### */                                                 \
//                                                                                       \
// template< class Pointer >                                                             \
//     requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)                     \
// mdspan( Pointer&& )                                                                   \
//     -> mdspan< std::remove_pointer_t<std::remove_reference_t<Pointer>>,               \
//                 extents<size_t>>;                                                \
//                                                                                       \
// /* ###################################### */                                          \
// /* # variadic shape deduction guide (3) # */                                          \
// /* ###################################### */                                          \
//                                                                                       \
// template< class ElementType, class... Integrals >                                     \
//     requires((std::is_convertible_v<Integrals, std::size_t> && ...) and               \
//                 sizeof...(Integrals) > 0)                                             \
// explicit mdspan( ElementType*, Integrals... )                                         \
//     -> mdspan<ElementType, dextents<std::size_t, sizeof...(Integrals)>>;         \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class... Integrals >                          \
//     requires((std::is_convertible_v<Integrals, std::size_t> && ...) and               \
//                 sizeof...(Integrals) > 0)                                             \
// explicit mdspan( Range&&, Integrals... )                                              \
//     -> mdspan<range_cv_value<Range>, dextents<std::size_t, sizeof...(Integrals)>>;         \
//                                                                                       \
// /* ############################ */                                                    \
// /* # span deduction guide (4) # */                                                    \
// /* ############################ */                                                    \
//                                                                                       \
// template< class ElementType, class OtherIndexType, std::size_t N >                    \
// mdspan( ElementType*, std::span<OtherIndexType, N> )                                  \
//     -> mdspan<ElementType, dextents<std::size_t, N>>;                            \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >         \
// mdspan( Range&&, std::span<OtherIndexType, N> )                                       \
//     -> mdspan<range_cv_value<Range>, dextents<std::size_t, N>>;                  \
//                                                                                       \
// /* ############################# */                                                   \
// /* # array deduction guide (5) # */                                                   \
// /* ############################# */                                                   \
//                                                                                       \
// template< class ElementType, class OtherIndexType, std::size_t N >                    \
// mdspan( ElementType*, const std::array<OtherIndexType, N>& )                          \
//     -> mdspan<ElementType, dextents<std::size_t, N>>;                            \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >         \
// mdspan( Range&&, const std::array<OtherIndexType, N>& )                               \
//     -> mdspan<range_cv_value<Range>, dextents<std::size_t, N>>;                  \
//                                                                                       \
// /* ############################## */                                                  \
// /* # extent deduction guide (6) # */                                                  \
// /* ############################## */                                                  \
//                                                                                       \
// template< class ElementType, class IndexType, std::size_t... ExtentsPack >            \
// mdspan( ElementType*, const extents<IndexType, ExtentsPack...>& )                \
//     -> mdspan<ElementType, extents<IndexType, ExtentsPack...>>;                  \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class IndexType, std::size_t... ExtentsPack > \
// mdspan( Range&&, const extents<IndexType, ExtentsPack...>& )                     \
//     -> mdspan<range_cv_value<Range>, extents<IndexType, ExtentsPack...>>;        \
//                                                                                       \
// /* ############################### */                                                 \
// /* # mapping deduction guide (7) # */                                                 \
// /* ############################### */                                                 \
//                                                                                       \
// template< class ElementType, class MappingType >                                      \
// mdspan( ElementType*, const MappingType& )                                            \
//     -> mdspan<ElementType, typename MappingType::extents_type,                        \
//                 typename MappingType::layout_type>;                                   \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class MappingType >                           \
// mdspan( Range&&, const MappingType& )                                                 \
//     -> mdspan<range_cv_value<Range>, typename MappingType::extents_type,              \
//                 typename MappingType::layout_type>;                                   \
//                                                                                       \
// /* ############################################ */                                    \
// /* # mapping and accessor deduction guide (8) # */                                    \
// /* ############################################ */                                    \
//                                                                                       \
// template< class MappingType, class AccessorType >                                     \
// mdspan( const typename AccessorType::data_handle_type&, const MappingType&,           \
//         const AccessorType& )                                                         \
//     -> mdspan<typename AccessorType::element_type,                                    \
//                 typename MappingType::extents_type,                                   \
//                 typename MappingType::layout_type, AccessorType>;                     \
//                                                                                       \
// template< std::ranges::contiguous_range Range, class MappingType, class AccessorType >       \
// mdspan( Range&&, const MappingType&,                                                  \
//         const AccessorType& )                                                         \
//     -> mdspan<range_cv_value<Range>,                                                  \
//                 typename MappingType::extents_type,                                   \
//                 typename MappingType::layout_type, AccessorType>;
