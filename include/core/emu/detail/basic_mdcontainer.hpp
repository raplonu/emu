#pragma once

#include <concepts>
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
    struct basic_mdcontainer : basic_mdspan<T, Extents, LayoutPolicy, AccessorPolicy, LocationPolicy, ActualType>, capsule
    {
        using base = basic_mdspan<T, Extents, LayoutPolicy, AccessorPolicy, LocationPolicy, ActualType>;

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

        using location_type = LocationPolicy;
        using actual_type = ActualType;

        template <typename Type>
        inline static constexpr bool validate_source = location_type::template validate_source<Type>;

        using base::base;

        // ##########################################
        // # Sized range constructor (non standard) #
        // ##########################################

        template< cpts::contiguous_sized_range Range >
            requires validate_source<Range>
                 and (base::rank() == 1)
        constexpr explicit basic_mdcontainer( Range&& r )
            : base( std::ranges::data(r), std::ranges::size(r) )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {}


        // ##################################
        // # Variadic shape constructor (2) #
        // ##################################

        template< std::ranges::contiguous_range Range, typename... OtherIndexTypes >
            requires validate_source<Range>
                and (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == base::rank_dynamic() or sizeof...(OtherIndexTypes) == base::rank())
        constexpr explicit basic_mdcontainer( Range&& r, OtherIndexTypes... exts )
            : base( std::ranges::data(r), exts... )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {}

        //TODO: check if this constructor is viable.
        // The main issue comes from the distinction with the constructor above.
        // The only way to distinct these two is to be able to check if the DataHolder is convertible to size_type or not.
        template< std::ranges::contiguous_range Range, typename DataHolder, typename... OtherIndexTypes >
            requires validate_source<Range>
                and (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == base::rank_dynamic() or sizeof...(OtherIndexTypes) == base::rank())
                and (not std::convertible_to<DataHolder, size_type>)
        constexpr explicit basic_mdcontainer( Range&& r, DataHolder&& dh, exts_flag_t, OtherIndexTypes... exts )
            : base( std::ranges::data(r), exts... )
            , capsule(EMU_FWD(dh))
        {}

        //TODO: same issue here
        template< typename DataHolder, typename... OtherIndexTypes >
            requires (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == base::rank_dynamic() or sizeof...(OtherIndexTypes) == base::rank())
                and (not std::convertible_to<DataHolder, size_type>)
        constexpr explicit basic_mdcontainer( data_handle_type p, DataHolder&& dh, exts_flag_t, OtherIndexTypes... exts )
            : base( p, exts... )
            , capsule(EMU_FWD(dh))
        {}


        // ##############################
        // # Span shape constructor (3) #
        // ##############################

        template< std::ranges::contiguous_range Range, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
                 and std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == base::rank()) || (N == base::rank_dynamic()))
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( Range&& r, std::span<OtherIndexType, N> exts )
            : base( std::ranges::data(r), exts )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {}

        template< std::ranges::contiguous_range Range, typename DataHolder, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
                 and std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == base::rank()) || (N == base::rank_dynamic()))
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( Range&& r, DataHolder&& dh, std::span<OtherIndexType, N> exts )
            : base( std::ranges::data(r), exts )
            , capsule(EMU_FWD(dh))
        {}

        template< typename DataHolder, typename OtherIndexType, std::size_t N >
            requires std::convertible_to<OtherIndexType, size_type>
                 and std::is_nothrow_convertible_v<OtherIndexType, size_type>
                 and ((N == base::rank()) || (N == base::rank_dynamic()))
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, std::span<OtherIndexType, N> exts )
            : base( p, exts )
            , capsule(EMU_FWD(dh))
        {}


        // ###############################
        // # Array shape constructor (4) #
        // ###############################

        template< std::ranges::contiguous_range Range, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( Range&& r, const std::array<OtherIndexType, N>& exts )
            : base( std::ranges::data(r), exts )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {}

        template< std::ranges::contiguous_range Range, typename DataHolder, typename OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( Range&& r, DataHolder&& dh, const std::array<OtherIndexType, N>& exts )
            : base( std::ranges::data(r), exts )
            , capsule(EMU_FWD(dh))
        {}

        template< typename DataHolder, typename OtherIndexType, std::size_t N >
        constexpr explicit(N != base::rank_dynamic())
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const std::array<OtherIndexType, N>& exts )
            : base( p, exts )
            , capsule(EMU_FWD(dh))
        {}


        // ##########################
        // # extent constructor (5) #
        // ##########################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const extents_type& exts )
            : base( std::ranges::data(r), exts )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {
            base::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const extents_type& exts )
            : base( std::ranges::data(r), exts )
            , capsule(EMU_FWD(dh))
        {
            base::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const extents_type& exts )
            : base( p, exts )
            , capsule(EMU_FWD(dh))
        {}


        // ###########################
        // # mapping constructor (6) #
        // ###########################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const mapping_type& m )
            : base( std::ranges::data(r), m )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {
            base::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const mapping_type& m )
            : base( std::ranges::data(r), m )
            , capsule(EMU_FWD(dh))
        {
            base::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const mapping_type& m )
            : base( p, m )
            , capsule(EMU_FWD(dh))
        {}


        // ########################################
        // # mapping and accessor constructor (7) #
        // ########################################

        template< std::ranges::contiguous_range Range >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, const mapping_type& m, const accessor_type& a )
            : base( std::ranges::data(r), m, a )
            , capsule(capsule_from_range(EMU_FWD(r)))
        {
            base::check_range_size(r);
        }

        template< std::ranges::contiguous_range Range, typename DataHolder >
            requires validate_source<Range>
        basic_mdcontainer( Range&& r, DataHolder&& dh, const mapping_type& m, const accessor_type& a )
            : base( std::ranges::data(r), m, a )
            , capsule(EMU_FWD(dh))
        {
            base::check_range_size(r);
        }

        template< typename DataHolder >
        basic_mdcontainer( data_handle_type p, DataHolder&& dh, const mapping_type& m, const accessor_type& a )
            : base( p, m, a )
            , capsule(EMU_FWD(dh))
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
            : base(static_cast<const base&>(other))
            , capsule(static_cast<const capsule&>(other))
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
            : base( other )
            , capsule(EMU_FWD(dh))
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
            : base( other )
            , capsule(EMU_FWD(dh))
        {}

        constexpr basic_mdcontainer( const basic_mdcontainer& rhs ) = default;
        constexpr basic_mdcontainer( basic_mdcontainer&& rhs ) = default;

        constexpr basic_mdcontainer& operator=(const basic_mdcontainer&) = default;
        constexpr basic_mdcontainer& operator=(basic_mdcontainer&&) = default;

        constexpr ~basic_mdcontainer() = default;

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
