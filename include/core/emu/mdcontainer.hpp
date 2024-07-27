#pragma once

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>
#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/mdspan.hpp>
#include <emu/type_name.hpp>

#include <memory>
#include <span>
#include <ranges>

namespace emu
{
    template<typename T, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<T>>
    struct mdcontainer : detail::basic_mdcontainer<
            T, Extents, LayoutPolicy, AccessorPolicy, no_source_validator,
            mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
        >
    {

        using base = detail::basic_mdcontainer<
            T, Extents, LayoutPolicy, AccessorPolicy, no_source_validator, mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
        >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    template<typename T> using mdcontainer_0d   = mdcontainer<T, d0>;

    template<typename T> using mdcontainer_1d   = mdcontainer<T, d1>;
    template<typename T> using mdcontainer_2d   = mdcontainer<T, d2>;
    template<typename T> using mdcontainer_3d   = mdcontainer<T, d3>;

    template<typename T> using mdcontainer_1d_c = mdcontainer<T, d1>;
    template<typename T> using mdcontainer_2d_c = mdcontainer<T, d2>;
    template<typename T> using mdcontainer_3d_c = mdcontainer<T, d3>;

    template<typename T> using mdcontainer_1d_f = mdcontainer<T, d1, layout_f>;
    template<typename T> using mdcontainer_2d_f = mdcontainer<T, d2, layout_f>;
    template<typename T> using mdcontainer_3d_f = mdcontainer<T, d3, layout_f>;

    template<typename T> using mdcontainer_1d_s = mdcontainer<T, d1, layout_stride>;
    template<typename T> using mdcontainer_2d_s = mdcontainer<T, d2, layout_stride>;
    template<typename T> using mdcontainer_3d_s = mdcontainer<T, d3, layout_stride>;

    /***************************************************/
    // Deduction guide

    /* variadic constructor 2*/
    template<std::ranges::contiguous_range  D, class ...Exts >
    requires(
        (std::is_convertible_v<Exts, std::size_t> && ...)
     && (sizeof...(Exts) > 0)
    )
    mdcontainer(D&&, Exts... exts )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::dextents<std::size_t, sizeof...(exts)>>;

    template<class T, class D, class ...Exts >
    mdcontainer(T*, D&&, Exts... exts )
        ->  mdcontainer<T, emu::dextents<std::size_t, sizeof...(exts)>
                        >;
    /* array constructor 3*/
    template<std::ranges::contiguous_range  D, class Exts, std::size_t N>
        requires(N != std::dynamic_extent)
    mdcontainer(D&&, const std::span<Exts, N>& )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::dextents<std::size_t, N>>;

    template<class T, class D, class Exts, std::size_t N>
        requires(N != std::dynamic_extent)
    mdcontainer(T*, D&&, const std::span<Exts, N>&  )
        ->  mdcontainer<T, emu::dextents<std::size_t, N>>;

    /* constructor 4*/
    template<std::ranges::contiguous_range  D, class Exts, std::size_t N>
    mdcontainer(D&&, const std::array<Exts, N>& )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::dextents<std::size_t, N>>;

    template<class T, class D, class Exts, std::size_t N>
    mdcontainer(T*, D&&, const std::array<Exts, N>& )
        ->  mdcontainer<T, emu::dextents<std::size_t, N>>;

    /* constructor 5*/
    template<std::ranges::contiguous_range  D, class Exts, std::size_t... ExtsPack >
    mdcontainer( D&&, const emu::extents<Exts, ExtsPack...>& )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::extents<Exts, ExtsPack...>>;

    template<class T, class D, class Exts, std::size_t... ExtsPack>
    mdcontainer( T*, D&&, const emu::extents<Exts, ExtsPack...>& )
        ->  mdcontainer<T, emu::extents<Exts, ExtsPack...>>;

    /* constructor 6*/
    template<class T, class D, class MappingType>
    mdcontainer( T*, D&&, const MappingType& )
        ->  mdcontainer<T, typename MappingType::extents_type, typename MappingType::layout_type>;
    template<class D, class MappingType >
    mdcontainer(D&&, const MappingType& )
        ->  mdcontainer<std::ranges::range_value_t<D>,typename MappingType::extents_type,
                        typename MappingType::layout_type>;

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    mdcontainer(const mdspan<T, Extents, LayoutPolicy, AccessorPolicy>&, emu::capsule)
        -> mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>;

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
            std::move(u_ptr), exts_flag, exts...);
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
        return mdcontainer<T, emu::dims<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr), exts);
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
        return mdcontainer<T, dims<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(), move(u_ptr), exts);
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
        return mdcontainer<T, dims<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(), std::move(u_ptr),
            exts);
    }

    /*constructor 6 (mdcontainer<dextents<std::size_t, N>>::mapping)*/
    template<typename T,
             typename MappingType
             >
    mdcontainer<T, typename MappingType::extents_type, typename MappingType::layout_type>
    make_mdcontainer(const MappingType& map){
        std::size_t size=map.required_span_size();
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer(u_ptr.get(), std::move(u_ptr),map);
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
