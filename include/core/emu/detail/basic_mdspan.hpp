#pragma once

#include <emu/concepts.hpp>
#include <emu/assert.hpp>
#include <emu/detail/basic_span.hpp>

#include <experimental/mdspan>
#include <ranges>


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
    using dims = dextents<std::size_t, N>;

    using d0 = dims<0>;
    using d1 = dims<1>;
    using d2 = dims<2>;
    using d3 = dims<3>;
    using d4 = dims<4>;

namespace detail
{

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename LocationPolicy, typename ActualType>
    struct basic_mdspan : stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>
    {

        using base = stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>;

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
        constexpr explicit basic_mdspan( Range&& r )
            : base( std::ranges::data(r), std::ranges::size(r) )
        {}


        // ##################################
        // # Variadic shape constructor (2) #
        // ##################################

        template< std::ranges::contiguous_range Range, class... OtherIndexTypes >
            requires validate_source<Range>
                and (std::convertible_to<OtherIndexTypes, size_type> and ...)
                and (std::is_nothrow_convertible_v<OtherIndexTypes, size_type> and ...)
                and (sizeof...(OtherIndexTypes) == base::rank_dynamic() or sizeof...(OtherIndexTypes) == base::rank())
        constexpr explicit basic_mdspan( Range&& r, OtherIndexTypes... exts )
            : base( std::ranges::data(r), exts... )
        {
            check_range_size(r);

        }


        // ##############################
        // # Span shape constructor (3) #
        // ##############################

        template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != base::rank_dynamic())
        basic_mdspan( Range&& r, std::span<OtherIndexType, N> exts )
            : base( std::ranges::data(r), exts )
        {
            check_range_size(r);
        }


        // ###############################
        // # Array shape constructor (4) #
        // ###############################

        template< std::ranges::contiguous_range Range, class OtherIndexType, std::size_t N >
            requires validate_source<Range>
        constexpr explicit(N != base::rank_dynamic())
        basic_mdspan( Range&& r, const std::array<OtherIndexType, N>& exts )
            : base( std::ranges::data(r), exts )
        {
            check_range_size(r);
        }


        // ##########################
        // # extent constructor (5) #
        // ##########################

        template< std::ranges::contiguous_range Range>
            requires validate_source<Range>
        constexpr basic_mdspan( Range&& r, const extents_type& ext )
            : base( std::ranges::data(r), ext )
        {
            check_range_size(r);
        }


        // ###########################
        // # mapping constructor (6) #
        // ###########################

        template< std::ranges::contiguous_range Range>
            requires validate_source<Range>
        constexpr basic_mdspan( Range&& r, const mapping_type& m )
            : base( std::ranges::data(r), m )
        {
            check_range_size(r);
        }


        // ########################################
        // # mapping and accessor constructor (7) #
        // ########################################

        template< std::ranges::contiguous_range Range>
            requires validate_source<Range>
        constexpr basic_mdspan( Range&& r, const mapping_type& m,
                  const accessor_type& a )
            : base( std::ranges::data(r), m, a )
        {
            check_range_size(r);
        }


        // ################################
        // # other mdspan constructor (8) #
        // ################################

        template< class OtherElementType, class OtherExtents,
                  class OtherLayoutPolicy, class OtherAccessor,
                  class OActualType,
                  class OMapping = OtherLayoutPolicy::template mapping<OtherExtents> >
        constexpr explicit(
            not std::is_convertible_v<
                const OMapping&,
                mapping_type
            >
         or not std::is_convertible_v<const OtherAccessor&, accessor_type>
        )
        basic_mdspan( const basic_mdspan<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor,
                             location_type, OActualType>& other )
            : base( other )
        {}

        template< class OtherElementType, class OtherExtents,
                  class OtherLayoutPolicy, class OtherAccessor,
                  class OMapping = OtherLayoutPolicy::template mapping<OtherExtents> >
        constexpr explicit(
            not std::is_convertible_v<
                const OMapping&,
                mapping_type
            >
            or not std::is_convertible_v<const OtherAccessor&, accessor_type>
        )
        basic_mdspan( const stdex::mdspan<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor>& other )
            : base( other )
        {}

        constexpr basic_mdspan( const basic_mdspan& rhs ) = default;
        constexpr basic_mdspan( basic_mdspan&& rhs ) = default;

        constexpr basic_mdspan& operator=(const basic_mdspan&) = default;
        constexpr basic_mdspan& operator=(basic_mdspan&&) = default;

        constexpr ~basic_mdspan() = default;

// #ifdef __cpp_multidimensional_subscript
//         using base::operator[];
// #else
//         using base::operator();
// #endif

//         using base::size;
//         using base::empty;
//         using base::stride;
//         using base::extents;
//         using base::data_handle;
//         using base::mapping;
//         using base::accessor;
//         using base::is_unique;
//         using base::is_exhaustive;
//         using base::is_strided;
//         using base::is_always_unique;
//         using base::is_always_exhaustive;
//         using base::is_always_strided;

//         using base::rank;
//         using base::rank_dynamic;
//         using base::static_extent;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return actual_type::from_mdspan(md);
        }

    protected:

        template<typename Range>
        constexpr void check_range_size( Range&& r ) const
        {
            if constexpr (std::ranges::sized_range<Range>) {
                fmt::print("size: {}\n", std::ranges::size(r));
                fmt::print("base::size(): {}\n", base::size());
                EMU_ASSERT_MSG( std::ranges::size(r) == base::size(), "input range and mdspan size mismatch" );
            }
        }

    };

} // namespace detail

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

            if constexpr (cpts::std_mdspan<MdSpan>)
                return res;
            else
                return src.from_mdspan(res);

        } (std::make_index_sequence<MdSpan::rank() - sizeof...(SliceSpecifiers)>{});
    }

} // namespace emu
