#pragma once

#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/cuda/device/accessor.hpp>
#include <emu/cuda/memory.hpp>

namespace emu::cuda::device
{

    template<
        typename T, typename Extents,
        typename LayoutPolicy = layout_right,
        typename AccessorPolicy = default_accessor<T>
    >
    struct mdcontainer
        : emu::detail::basic_mdcontainer<
            T,
            Extents,
            LayoutPolicy,
            cuda::device::accessor<AccessorPolicy>
        >
    {
        using base = emu::detail::basic_mdcontainer<T, Extents, LayoutPolicy, cuda::device::accessor<AccessorPolicy>>;

        using base::base;

        constexpr mdcontainer() noexcept = default;

        friend constexpr void swap(mdcontainer& x, mdcontainer& y) noexcept {
            swap(static_cast<base&>(x), static_cast<base&>(y));
        }
    };

    EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES(mdcontainer);
    EMU_DEFINE_MDCONTAINER_ALIAS;

    /***************************************************/
    /* factories */
    /*constructor 2 (exts)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename ...OtherIndexTypes>
    requires((std::is_convertible_v<OtherIndexTypes, size_t> && ...)
             && sizeof...(OtherIndexTypes) > 0)
    auto make_mdcontainer(device_ref device, OtherIndexTypes... exts){
        constexpr size_t nd = sizeof...(OtherIndexTypes);
        size_t size = (1 * ... * exts);
        auto u_ptr = device::make_unique<T[]>(device, size );
        return mdcontainer<T, emu::dims<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr), exts_flag, exts...);
    }

    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename ...OtherIndexTypes>
    requires((std::is_convertible_v<OtherIndexTypes, size_t> && ...)
             && sizeof...(OtherIndexTypes) > 0)
    auto make_mdcontainer(OtherIndexTypes... exts){
        constexpr size_t nd = sizeof...(OtherIndexTypes);
        size_t size = (1 * ... * exts);
        auto u_ptr = device::make_unique<T[]>(size );
        return mdcontainer<T, emu::dims<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr), exts_flag, exts...);
    }

    // /*constructor 3 (span<exts>)*/
    // template<typename T,
    //          typename LayoutPolicy = layout_right,
    //          typename AccessorPolicy = default_accessor<T>,
    //          typename OtherIndexType,
    //          size_t N>
    // mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    // make_mdcontainer(const emu::span<OtherIndexType, N>& exts){
    //     size_t size = 1;
    //     for(auto e : exts ){size = size * e;}
    //     auto u_ptr = device::make_unique<T[]>(size );
    //     return {u_ptr.get(), std::move(u_ptr), exts};
    // }

    // template<typename T,
    //          typename LayoutPolicy = layout_right,
    //          typename AccessorPolicy = default_accessor<T>,
    //          typename OtherIndexType,
    //          size_t N>
    // mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    // make_mdcontainer(const std::span<OtherIndexType, N>& exts){
    //     size_t size = 1;
    //     for(auto e : exts ){size = size * e;}
    //     auto u_ptr = std::make_unique<T[]>(size );
    //     return {u_ptr.get(), std::move(u_ptr), exts};
    // }

    // /*constructor 4 (array<exts>)*/
    // template<typename T,
    //          typename LayoutPolicy = layout_right,
    //          typename AccessorPolicy = default_accessor<T>,
    //          typename OtherIndexType,
    //          size_t N>
    // mdcontainer<T, emu::dims<N>,LayoutPolicy,AccessorPolicy>
    // make_mdcontainer(const std::array<OtherIndexType, N>& exts){
    //     size_t size = 1;
    //     for(auto e :exts ){size *= e;}
    //     auto u_ptr = device::make_unique<T[]>(size );
    //     return {u_ptr.get(), std::move(u_ptr), exts};
    // }

    // /*constructor 5 (dextents<size_t, N>)*/
    // template<typename T,
    //          size_t N,
    //          typename LayoutPolicy = layout_right,
    //          typename AccessorPolicy = default_accessor<T>
    //          >
    // mdcontainer<T, dims<N>,LayoutPolicy,AccessorPolicy>
    // make_mdcontainer(const dims<N> &exts){
    //     size_t size = 1;
    //     for(size_t i = 0; i < N; i++) {
    //         size *= exts.extent(i);
    //     }
    //     auto u_ptr = device::make_unique<T[]>(size );
    //     return {u_ptr.get(), std::move(u_ptr), exts};
    // }

    // /*constructor 6 (mdcontainer<dextents<size_t, N>>::mapping)*/
    // template<typename T,
    //          typename MappingType
    //          >
    // mdcontainer<T, typename MappingType::extents_type, typename MappingType::layout_type>
    // make_mdcontainer(const MappingType& map){
    //     size_t size=map.required_span_size();
    //     auto u_ptr = std::make_unique<T[]>(size );
    //     return {u_ptr.get(), std::move(u_ptr), map};
    // }

} // namespace emu::cuda::device
