#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_mdspan.hpp>

namespace emu::host
{

    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    struct mdcontainer : detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, AccessorPolicy, host::source_validator,
        mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
    >
    {
        using base = detail::basic_mdcontainer< T, Extents, LayoutPolicy, AccessorPolicy, host::source_validator, mdcontainer >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    // ##############################################
    // # Sized range deduction guide (non standard) #
    // ##############################################

    template< cpts::contiguous_sized_range Range >
        requires (not std::is_array_v<Range>)
    mdcontainer( Range&& )
        -> mdcontainer< rm_ref< std::ranges::range_reference_t< rm_ref<Range> > >,
                   std::extents<std::size_t, 1>>;

    // ####################################
    // # Static array deduction guide (1) #
    // ####################################

    template< class CArray >
        requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)
    mdcontainer( CArray& )
        -> mdcontainer< std::remove_all_extents_t<CArray>,
                   std::extents<std::size_t, std::extent_v<CArray, 0>>>;

    // ###############################
    // # pointer deduction guide (2) #
    // ###############################

    template< class Pointer >
        requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)
    mdcontainer( Pointer&& )
        -> mdcontainer< std::remove_pointer_t<std::remove_reference_t<Pointer>>,
                   std::extents<size_t>>;

    // ######################################
    // # variadic shape deduction guide (3) #
    // ######################################

    template< class ElementType, class... Integrals >
        requires((std::is_convertible_v<Integrals, std::size_t> && ...) and
                 sizeof...(Integrals) > 0)
    explicit mdcontainer( ElementType*, Integrals... )
        -> mdcontainer<ElementType, std::dextents<std::size_t, sizeof...(Integrals)>>;

    template< cpts::contiguous_range Range, class... Integrals >
        requires((std::is_convertible_v<Integrals, std::size_t> && ...) and
                 sizeof...(Integrals) > 0)
    explicit mdcontainer( Range&&, Integrals... )
        -> mdcontainer<ElementType, std::dextents<std::size_t, sizeof...(Integrals)>>;

    // ############################
    // # span deduction guide (4) #
    // ############################

    template< class ElementType, class OtherIndexType, std::size_t N >
    mdcontainer( ElementType*, std::span<OtherIndexType, N> )
        -> mdcontainer<ElementType, std::dextents<std::size_t, N>>;

    template< cpts::contiguous_range Range, class OtherIndexType, std::size_t N >
    mdcontainer( Range&&, std::span<OtherIndexType, N> )
        -> mdcontainer<range_cv_value<Range>, std::dextents<std::size_t, N>>;

    // #############################
    // # array deduction guide (5) #
    // #############################

    template< class ElementType, class OtherIndexType, std::size_t N >
    mdcontainer( ElementType*, const std::array<OtherIndexType, N>& )
        -> mdcontainer<ElementType, std::dextents<std::size_t, N>>;

    template< cpts::contiguous_range Range, class OtherIndexType, std::size_t N >
    mdcontainer( Range&&, const std::array<OtherIndexType, N>& )
        -> mdcontainer<range_cv_value<Range>, std::dextents<std::size_t, N>>;

    // ##############################
    // # extent deduction guide (6) #
    // ##############################

    template< class ElementType, class IndexType, std::size_t... ExtentsPack >
    mdcontainer( ElementType*, const std::extents<IndexType, ExtentsPack...>& )
        -> mdcontainer<ElementType, std::extents<IndexType, ExtentsPack...>>;

    template< cpts::contiguous_range Range, class IndexType, std::size_t... ExtentsPack >
    mdcontainer( Range&&, const std::extents<IndexType, ExtentsPack...>& )
        -> mdcontainer<range_cv_value<Range>, std::extents<IndexType, ExtentsPack...>>;

    // ###############################
    // # mapping deduction guide (7) #
    // ###############################

    template< class ElementType, class MappingType >
    mdcontainer( ElementType*, const MappingType& )
        -> mdcontainer<ElementType, typename MappingType::extents_type,
                  typename MappingType::layout_type>;

    template< cpts::contiguous_range Range, class MappingType >
    mdcontainer( Range&&, const MappingType& )
        -> mdcontainer<range_cv_value<Range>, typename MappingType::extents_type,
                  typename MappingType::layout_type>;

    // ############################################
    // # mapping and accessor deduction guide (8) #
    // ############################################

    template< class MappingType, class AccessorType >
    mdcontainer( const typename AccessorType::data_handle_type&, const MappingType&,
            const AccessorType& )
        -> mdcontainer<typename AccessorType::element_type,
                  typename MappingType::extents_type,
                  typename MappingType::layout_type, AccessorType>;

    template< cpts::contiguous_range Range, class MappingType, class AccessorType >
    mdcontainer( Range&&, const MappingType&,
            const AccessorType& )
        -> mdcontainer<range_cv_value<Range>,
                  typename MappingType::extents_type,
                  typename MappingType::layout_type, AccessorType>;

} // namespace emu::host
