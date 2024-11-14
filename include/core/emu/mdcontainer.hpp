#pragma once

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>
#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/location_policy.hpp>
#include <emu/span.hpp>
#include <emu/mdspan.hpp>
#include <emu/type_name.hpp>

#include <memory>
#include <span>
#include <ranges>

namespace emu
{
    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer : emu::detail::basic_mdcontainer<
            T, Extents, LayoutPolicy, AccessorPolicy, no_location_policy,
            mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
        >
    {

        using base = emu::detail::basic_mdcontainer<
            T, Extents, LayoutPolicy, AccessorPolicy, no_location_policy, mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
        >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES

    EMU_DEFINE_MDCONTAINER_ALIAS

    /***************************************************/
    /* factories */
    /*constructor 2 (exts)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename ...OtherIndexTypes>
    requires((std::is_convertible_v<OtherIndexTypes, std::size_t> && ...)
             && sizeof...(OtherIndexTypes) > 0)
    mdcontainer<T, emu::dims< sizeof...(OtherIndexTypes)>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(OtherIndexTypes... exts){
        constexpr std::size_t nd = sizeof...(OtherIndexTypes);
        size_t size = (1 * ... * exts);
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::dims<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            move(u_ptr), exts_flag, exts...);
    }

    /*constructor 3 (span<exts>)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const emu::span<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e : exts ){size = size * e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return {u_ptr.get(), move(u_ptr), exts};
    }

    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const std::span<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e : exts ){size = size * e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return {u_ptr.get(), move(u_ptr), exts};
    }

    /*constructor 4 (array<exts>)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const std::array<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e :exts ){size *= e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return {u_ptr.get(), move(u_ptr), exts};
    }

    /*constructor 5 (dextents<std::size_t, N>)*/
    template<typename T,
             std::size_t N,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>
             >
    mdcontainer<T, dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const dims<N> &exts){
        std::size_t size = 1;
        for(std::size_t i = 0; i < N; i++) {
            size *= exts.extent(i);
        }
        auto u_ptr = std::make_unique<T[]>(size );
        return {u_ptr.get(), move(u_ptr), exts};
    }

    /*constructor 6 (mdcontainer<dextents<std::size_t, N>>::mapping)*/
    template<typename T,
             typename MappingType
             >
    mdcontainer<T, typename MappingType::extents_type, typename MappingType::layout_type>
    make_mdcontainer(const MappingType& map){
        std::size_t size=map.required_span_size();
        auto u_ptr = std::make_unique<T[]>(size );
        return {u_ptr.get(), move(u_ptr), map};
    }

    /*constructor 7*/
    //TODO if need be

    // template <class ElementType, class Extents, class LayoutPolicy,
    //       class AccessorPolicy, class... SliceSpecifiers>
    // constexpr auto submdcontainer(
    //     const mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy> &src,
    //     SliceSpecifiers... slices)
    // {
    //     return mdcontainer(emu::submdspan(src, EMU_FWD(slices)...), static_cast<const capsule&>(src) );
    // }

namespace spe
{
    template <typename ElementType, typename Extents, typename LayoutPolicy,
              typename AccessorPolicy>
    struct info_t< mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy> >
        : info_t<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>>
    {
        //TODO: Adds the capsule info
    };

} // namespace spe

} // namespace emu
