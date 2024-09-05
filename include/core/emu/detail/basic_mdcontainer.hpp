#pragma once

#include <emu/type_traits.hpp>
#include <emu/concepts.hpp>

#include <emu/detail/basic_mdspan.hpp>
#include <emu/capsule.hpp>

namespace emu
{

namespace detail
{
    struct exts_flag_t {};

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename LocationPolicy, typename ActualType>
    struct basic_mdcontainer : basic_mdspan<T, Extents, LayoutPolicy, AccessorPolicy, LocationPolicy, ActualType>, emu::capsule
    {
        using mdspan_type = basic_mdspan<T, Extents, LayoutPolicy, AccessorPolicy, LocationPolicy, ActualType>;
        using capsule_base = emu::capsule;

        using extents_type     = typename mdspan_type::extents_type;
        using layout_type      = typename mdspan_type::layout_type;
        using accessor_type    = typename mdspan_type::accessor_type;
        using mapping_type     = typename mdspan_type::mapping_type;
        using element_type     = typename mdspan_type::element_type;
        using value_type       = typename mdspan_type::value_type;
        using index_type       = typename mdspan_type::index_type;
        using size_type        = typename mdspan_type::size_type;
        using rank_type        = typename mdspan_type::rank_type;
        using data_handle_type = typename mdspan_type::data_handle_type;
        using reference        = typename mdspan_type::reference;

        using location_type = LocationPolicy;
        using actual_type = ActualType;

        template <typename Type>
        inline static constexpr bool validate_source = location_type::template validate_source<Type>;

        using mdspan_type::mdspan_type;

        // ##########################################
        // # Sized range constructor (non standard) #
        // ##########################################

        template< cpts::contiguous_sized_range Range >
            requires validate_source<Range>
                 and (mdspan_type::rank() == 1)
                 and (std::ranges::borrowed_range<Range> or is_const<element_type> or cpts::relocatable_owning_range<Range>)
        constexpr explicit basic_mdcontainer( Range&& r )
            : mdspan_type( std::ranges::data(r), std::ranges::size(r) )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {}


        // ##################################
        // # Variadic shape constructor (2) #
        // ##################################

        template< std::ranges::contiguous_range Range, typename... OtherIndexTypes >
            requires validate_source<Range>
                and (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == mdspan_type::rank_dynamic() or sizeof...(OtherIndexTypes) == mdspan_type::rank())
        constexpr explicit basic_mdcontainer( Range&& r, OtherIndexTypes... exts )
            : mdspan_type( std::ranges::data(r), exts... )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {}

        //TODO: check if this constructor is viable.
        // The main issue comes from the distinction with the constructor above.
        // The only way to distinct these two is to be able to check if the DataHolder is convertible to size_type or not.
        template< std::ranges::contiguous_range Range, typename DataHolder, typename... OtherIndexTypes >
            requires validate_source<Range>
                and (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == mdspan_type::rank_dynamic() or sizeof...(OtherIndexTypes) == mdspan_type::rank())
                // and (not std::convertible_to<DataHolder, size_type>)
        constexpr explicit basic_mdcontainer( Range&& r, DataHolder&& dh, exts_flag_t, OtherIndexTypes... exts )
            : mdspan_type( std::ranges::data(r), exts... )
            , capsule_base(EMU_FWD(dh))
        {}

        //TODO: same issue here
        template< typename DataHolder, typename... OtherIndexTypes >
            requires (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == mdspan_type::rank_dynamic() or sizeof...(OtherIndexTypes) == mdspan_type::rank())
                // and (not std::convertible_to<DataHolder, size_type>)
        constexpr explicit basic_mdcontainer( data_handle_type p, DataHolder&& dh, exts_flag_t, OtherIndexTypes... exts )
            : mdspan_type( p, exts... )
            , capsule_base(EMU_FWD(dh))
        {}


        // ##############################
        // # Span shape constructor (3) #
        // ##############################

        template< std::ranges::contiguous_range Range, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
                 and std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == mdspan_type::rank()) || (N == mdspan_type::rank_dynamic()))
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( Range&& r, std::span<OtherIndexType, N> exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {}

        template< std::ranges::contiguous_range Range, typename DataHolder, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
                 and std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == mdspan_type::rank()) || (N == mdspan_type::rank_dynamic()))
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( Range&& r, DataHolder&& dh, std::span<OtherIndexType, N> exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(EMU_FWD(dh))
        {}

        template< typename DataHolder, typename OtherIndexType, std::size_t N >
            requires std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == mdspan_type::rank()) || (N == mdspan_type::rank_dynamic()))
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, std::span<OtherIndexType, N> exts )
            : mdspan_type( p, exts )
            , capsule_base(EMU_FWD(dh))
        {}


        // ###############################
        // # Array shape constructor (4) #
        // ###############################

        template< std::ranges::contiguous_range Range, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( Range&& r, const std::array<OtherIndexType, N>& exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {}

        template< std::ranges::contiguous_range Range, typename DataHolder, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( Range&& r, DataHolder&& dh, const std::array<OtherIndexType, N>& exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(EMU_FWD(dh))
        {}

        template< typename DataHolder, typename OtherIndexType, std::size_t N >
        constexpr explicit(N != mdspan_type::rank_dynamic())
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const std::array<OtherIndexType, N>& exts )
            : mdspan_type( p, exts )
            , capsule_base(EMU_FWD(dh))
        {}


        // ##########################
        // # extent constructor (5) #
        // ##########################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const extents_type& exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {
            mdspan_type::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const extents_type& exts )
            : mdspan_type( std::ranges::data(r), exts )
            , capsule_base(EMU_FWD(dh))
        {
            mdspan_type::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const extents_type& exts )
            : mdspan_type( p, exts )
            , capsule_base(EMU_FWD(dh))
        {}


        // ###########################
        // # mapping constructor (6) #
        // ###########################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const mapping_type& m )
            : mdspan_type( std::ranges::data(r), m )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {
            mdspan_type::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const mapping_type& m )
            : mdspan_type( std::ranges::data(r), m )
            , capsule_base(EMU_FWD(dh))
        {
            mdspan_type::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const mapping_type& m )
            : mdspan_type( p, m )
            , capsule_base(EMU_FWD(dh))
        {}


        // ########################################
        // # mapping and accessor constructor (7) #
        // ########################################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const mapping_type& m, const accessor_type& a )
            : mdspan_type( std::ranges::data(r), m, a )
            , capsule_base(capsule_from_range(EMU_FWD(r)))
        {
            mdspan_type::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const mapping_type& m, const accessor_type& a )
            : mdspan_type( std::ranges::data(r), m, a )
            , capsule_base(EMU_FWD(dh))
        {
            mdspan_type::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const mapping_type& m, const accessor_type& a )
            : mdspan_type( p, m, a )
            , capsule_base(EMU_FWD(dh))
        {}



        // ################################
        // # other mdspan constructor (8) #
        // ################################

        //TODO: check if allow OActualType to be different to actual_type is a good idea.
        template< typename OtherElementType, typename OtherExtents,
                  typename OtherLayoutPolicy, typename OtherAccessor,
                  typename OActualType,
                  typename OMapping = OtherLayoutPolicy::template mapping<OtherExtents> >
        constexpr explicit(
            not std::is_convertible_v<
                const OMapping&,
                mapping_type
            >
         or not std::is_convertible_v<const OtherAccessor&, accessor_type>
        )
        basic_mdcontainer( const basic_mdcontainer<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor,
                             location_type, OActualType>& other )
            : mdspan_type(static_cast<const mdspan_type&>(other))
            , capsule_base(other.capsule())
        {}

        template< typename OtherElementType, typename OtherExtents,
                  typename OtherLayoutPolicy, typename OtherAccessor,
                  typename OActualType, typename DataHolder,
                  typename OMapping = OtherLayoutPolicy::template mapping<OtherExtents> >
        constexpr explicit(
            not std::is_convertible_v<
                const OMapping&,
                mapping_type
            >
         or not std::is_convertible_v<const OtherAccessor&, accessor_type>
        )
        basic_mdcontainer( const basic_mdspan<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor,
                             location_type, OActualType>& other,
                      DataHolder&& dh )
            : mdspan_type( other )
            , capsule_base(EMU_FWD(dh))
        {}

        template< typename OtherElementType, typename OtherExtents,
                  typename OtherLayoutPolicy, typename OtherAccessor,
                  typename DataHolder,
                  typename OMapping = OtherLayoutPolicy::template mapping<OtherExtents> >
        constexpr explicit(
            not std::is_convertible_v<
                const OMapping&,
                mapping_type
            >
            or not std::is_convertible_v<const OtherAccessor&, accessor_type>
        )
        basic_mdcontainer( const stdex::mdspan<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor>& other,
                      DataHolder&& dh )
            : mdspan_type( other )
            , capsule_base(EMU_FWD(dh))
        {}

        constexpr basic_mdcontainer( const basic_mdcontainer& rhs ) = default;
        constexpr basic_mdcontainer( basic_mdcontainer&& rhs ) = default;

        constexpr basic_mdcontainer& operator=(const basic_mdcontainer&) = default;
        constexpr basic_mdcontainer& operator=(basic_mdcontainer&&) = default;

        constexpr ~basic_mdcontainer() = default;

        emu::capsule&        capsule() &      noexcept { return static_cast<emu::capsule&>(*this); }
        emu::capsule const & capsule() const& noexcept { return static_cast<emu::capsule>(*this); }
        emu::capsule         capsule() &&     noexcept { return static_cast<emu::capsule>(*this); }

    };

} // namespace detail

    constexpr detail::exts_flag_t exts_flag;

    template <cpts::mdcontainer MdContainer, class... SliceSpecifiers>
    constexpr auto submdcontainer(
        const MdContainer &src, SliceSpecifiers... slices)
    {
        return emu::submdspan(src, EMU_FWD(slices)...);
    }

} // namespace emu

#define EMU_DEFINE_MDCONTAINER_ALIAS                                                                 \
template<typename ElementType> using mdcontainer_0d   = mdcontainer<ElementType, d0>;                \
                                                                                                     \
template<typename ElementType> using mdcontainer_1d   = mdcontainer<ElementType, d1>;                \
template<typename ElementType> using mdcontainer_2d   = mdcontainer<ElementType, d2>;                \
template<typename ElementType> using mdcontainer_3d   = mdcontainer<ElementType, d3>;                \
                                                                                                     \
template<typename ElementType> using mdcontainer_1d_c = mdcontainer<ElementType, d1>;                \
template<typename ElementType> using mdcontainer_2d_c = mdcontainer<ElementType, d2>;                \
template<typename ElementType> using mdcontainer_3d_c = mdcontainer<ElementType, d3>;                \
                                                                                                     \
template<typename ElementType> using mdcontainer_1d_f = mdcontainer<ElementType, d1, layout_f>;      \
template<typename ElementType> using mdcontainer_2d_f = mdcontainer<ElementType, d2, layout_f>;      \
template<typename ElementType> using mdcontainer_3d_f = mdcontainer<ElementType, d3, layout_f>;      \
                                                                                                     \
template<typename ElementType> using mdcontainer_1d_s = mdcontainer<ElementType, d1, layout_stride>; \
template<typename ElementType> using mdcontainer_2d_s = mdcontainer<ElementType, d2, layout_stride>; \
template<typename ElementType> using mdcontainer_3d_s = mdcontainer<ElementType, d3, layout_stride>;


#define EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES                                                \
/* ############################################## */                                           \
/* # Sized range deduction guide (non standard) # */                                           \
/* ############################################## */                                           \
                                                                                               \
template< emu::cpts::contiguous_sized_range Range >                                            \
    requires (not std::is_array_v<Range>)                                                      \
mdcontainer( Range&& )                                                                         \
    -> mdcontainer< rm_ref< std::ranges::range_reference_t< rm_ref<Range> > >,                 \
                extents<std::size_t, 1>>;                                                      \
                                                                                               \
/* #################################### */                                                     \
/* # Static array deduction guide (1) # */                                                     \
/* #################################### */                                                     \
                                                                                               \
template< class CArray >                                                                       \
    requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)                              \
mdcontainer( CArray& )                                                                         \
    -> mdcontainer< std::remove_all_extents_t<CArray>,                                         \
                extents<std::size_t, std::extent_v<CArray, 0>>>;                               \
                                                                                               \
/* ############################### */                                                          \
/* # pointer deduction guide (2) # */                                                          \
/* ############################### */                                                          \
                                                                                               \
template< class Pointer >                                                                      \
    requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)                              \
mdcontainer( Pointer&& )                                                                       \
    -> mdcontainer< std::remove_pointer_t<std::remove_reference_t<Pointer>>,                   \
                extents<size_t>>;                                                              \
                                                                                               \
/* ###################################### */                                                   \
/* # variadic shape deduction guide (3) # */                                                   \
/* ###################################### */                                                   \
                                                                                               \
template< class ElementType, class... Integrals >                                              \
    requires((std::is_convertible_v<Integrals, std::size_t> && ...) and                        \
                sizeof...(Integrals) > 0)                                                      \
explicit mdcontainer( ElementType*, Integrals... )                                             \
    -> mdcontainer<ElementType, dextents<std::size_t, sizeof...(Integrals)>>;                  \
                                                                                               \
template< std::ranges::contiguous_range Range, class... Integrals >                            \
    requires((std::is_convertible_v<Integrals, std::size_t> && ...) and                        \
                sizeof...(Integrals) > 0)                                                      \
explicit mdcontainer( Range&&, Integrals... )                                                  \
    -> mdcontainer<range_cv_value<Range>, dextents<std::size_t, sizeof...(Integrals)>>;        \
                                                                                               \
/* ############################ */                                                             \
/* # span deduction guide (4) # */                                                             \
/* ############################ */                                                             \
                                                                                               \
template< class ElementType, class OtherIndexType, std::size_t N >                             \
mdcontainer( ElementType*, std::span<OtherIndexType, N> )                                      \
    -> mdcontainer<ElementType, dextents<std::size_t, N>>;                                     \
                                                                                               \
template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >           \
mdcontainer( Range&&, std::span<OtherIndexType, N> )                                           \
    -> mdcontainer<range_cv_value<Range>, dextents<std::size_t, N>>;                           \
                                                                                               \
/* ############################# */                                                            \
/* # array deduction guide (5) # */                                                            \
/* ############################# */                                                            \
                                                                                               \
template< class ElementType, class OtherIndexType, std::size_t N >                             \
mdcontainer( ElementType*, const std::array<OtherIndexType, N>& )                              \
    -> mdcontainer<ElementType, dextents<std::size_t, N>>;                                     \
                                                                                               \
template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >           \
mdcontainer( Range&&, const std::array<OtherIndexType, N>& )                                   \
    -> mdcontainer<range_cv_value<Range>, dextents<std::size_t, N>>;                           \
                                                                                               \
/* ############################## */                                                           \
/* # extent deduction guide (6) # */                                                           \
/* ############################## */                                                           \
                                                                                               \
template< class ElementType, class IndexType, std::size_t... ExtentsPack >                     \
mdcontainer( ElementType*, const extents<IndexType, ExtentsPack...>& )                         \
    -> mdcontainer<ElementType, extents<IndexType, ExtentsPack...>>;                           \
                                                                                               \
template< std::ranges::contiguous_range Range, class IndexType, std::size_t... ExtentsPack >   \
mdcontainer( Range&&, const extents<IndexType, ExtentsPack...>& )                              \
    -> mdcontainer<range_cv_value<Range>, extents<IndexType, ExtentsPack...>>;                 \
                                                                                               \
/* ############################### */                                                          \
/* # mapping deduction guide (7) # */                                                          \
/* ############################### */                                                          \
                                                                                               \
template< class ElementType, class MappingType >                                               \
mdcontainer( ElementType*, const MappingType& )                                                \
    -> mdcontainer<ElementType, typename MappingType::extents_type,                            \
                typename MappingType::layout_type>;                                            \
                                                                                               \
template< std::ranges::contiguous_range Range, class MappingType >                             \
mdcontainer( Range&&, const MappingType& )                                                     \
    -> mdcontainer<range_cv_value<Range>, typename MappingType::extents_type,                  \
                typename MappingType::layout_type>;                                            \
                                                                                               \
/* ############################################ */                                             \
/* # mapping and accessor deduction guide (8) # */                                             \
/* ############################################ */                                             \
                                                                                               \
template< class MappingType, class AccessorType >                                              \
mdcontainer( const typename AccessorType::data_handle_type&, const MappingType&,               \
        const AccessorType& )                                                                  \
    -> mdcontainer<typename AccessorType::element_type,                                        \
                typename MappingType::extents_type,                                            \
                typename MappingType::layout_type, AccessorType>;                              \
                                                                                               \
template< std::ranges::contiguous_range Range, class MappingType, class AccessorType >         \
mdcontainer( Range&&, const MappingType&,                                                      \
        const AccessorType& )                                                                  \
    -> mdcontainer<range_cv_value<Range>,                                                      \
                typename MappingType::extents_type,                                            \
                typename MappingType::layout_type, AccessorType>;
