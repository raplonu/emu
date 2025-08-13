#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/cuda/host/location_policy.hpp>
#include <emu/cuda/memory.hpp>
#include <emu/span.hpp>
#include <span>
#include <array>

namespace emu::cuda::host
{

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer : emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, AccessorPolicy, emu::cuda::host_location_policy,
        mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
    >
    {
        using base = emu::detail::basic_mdcontainer< T, Extents, LayoutPolicy, AccessorPolicy, emu::cuda::host_location_policy, mdcontainer >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    // EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES - DISABLED DUE TO TEMPLATE MISMATCH

    // EMU_DEFINE_MDCONTAINER_ALIAS - DISABLED DUE TO TEMPLATE MISMATCH

    /***************************************************/
    /* factories */
    /*constructor 2 (exts)*/
    template<typename T,
             typename LayoutPolicy = stdex::layout_right,
             typename AccessorPolicy = stdex::default_accessor<T>,
             typename ...OtherIndexTypes>
    requires((std::is_convertible_v<OtherIndexTypes, std::size_t> && ...)
             && sizeof...(OtherIndexTypes) > 0)
    mdcontainer<T, emu::dims< sizeof...(OtherIndexTypes)>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(OtherIndexTypes... exts){
        constexpr std::size_t nd = sizeof...(OtherIndexTypes);
        size_t size = (1 * ... * exts);
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return mdcontainer<T, emu::dims<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr), emu::exts_flag, exts...);
    }

    /*constructor 3 (span<exts>)*/
    template<typename T,
             typename LayoutPolicy = stdex::layout_right,
             typename AccessorPolicy = stdex::default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const emu::span<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e : exts ){size = size * e;}
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return {u_ptr.get(), std::move(u_ptr), exts};
    }

    template<typename T,
             typename LayoutPolicy = stdex::layout_right,
             typename AccessorPolicy = stdex::default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const std::span<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e : exts ){size = size * e;}
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return {u_ptr.get(), std::move(u_ptr), exts};
    }

    /*constructor 4 (array<exts>)*/
    template<typename T,
             typename LayoutPolicy = stdex::layout_right,
             typename AccessorPolicy = stdex::default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const std::array<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e :exts ){size *= e;}
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return {u_ptr.get(), std::move(u_ptr), exts};
    }

    /*constructor 5 (dextents<std::size_t, N>)*/
    template<typename T,
             std::size_t N,
             typename LayoutPolicy = stdex::layout_right,
             typename AccessorPolicy = stdex::default_accessor<T>
             >
    mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const emu::dims<N> &exts){
        std::size_t size = 1;
        for(std::size_t i = 0; i < N; i++) {
            size *= exts.extent(i);
        }
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return {u_ptr.get(), std::move(u_ptr), exts};
    }

    /*constructor 6 (mdcontainer<dextents<std::size_t, N>>::mapping)*/
    template<typename T,
             typename MappingType
             >
    mdcontainer<T, typename MappingType::extents_type, typename MappingType::layout_type, stdex::default_accessor<T>>
    make_mdcontainer(const MappingType& map){
        std::size_t size=map.required_span_size();
        auto u_ptr = emu::cuda::host::make_unique<T[]>(size);
        return {u_ptr.get(), std::move(u_ptr), map};
    }

} // namespace emu::cuda::host
