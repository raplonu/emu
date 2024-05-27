#pragma once

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>
#include <emu/mdspan.hpp>
#include <emu/type_name.hpp>

#include <memory>
#include <span>
#include <ranges>

namespace emu
{

    template<typename T,
             typename Extents,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>
             >
    struct mdcontainer;

    template<typename T> using mdcontainer_0d   = mdcontainer<T, _0d>;

    template<typename T> using mdcontainer_1d   = mdcontainer<T, _1d>;
    template<typename T> using mdcontainer_2d   = mdcontainer<T, _2d>;
    template<typename T> using mdcontainer_3d   = mdcontainer<T, _3d>;

    template<typename T> using mdcontainer_1d_c = mdcontainer<T, _1d>;
    template<typename T> using mdcontainer_2d_c = mdcontainer<T, _2d>;
    template<typename T> using mdcontainer_3d_c = mdcontainer<T, _3d>;

    template<typename T> using mdcontainer_1d_f = mdcontainer<T, _1d, layout_f>;
    template<typename T> using mdcontainer_2d_f = mdcontainer<T, _2d, layout_f>;
    template<typename T> using mdcontainer_3d_f = mdcontainer<T, _3d, layout_f>;

    template<typename T> using mdcontainer_1d_s = mdcontainer<T, _1d, layout_stride>;
    template<typename T> using mdcontainer_2d_s = mdcontainer<T, _2d, layout_stride>;
    template<typename T> using mdcontainer_3d_s = mdcontainer<T, _3d, layout_stride>;

namespace cpts
{

    // Check if T is a container or derived from one.
    template<typename T>
    concept mdcontainer = std::derived_from<
        T,
        emu::mdcontainer<
            typename T::element_type,
            typename T::extents_type,
            typename T::layout_type,
            typename T::accessor_type
        >
    >;

} // namespace cpts

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer : public mdspan<T, Extents, LayoutPolicy, AccessorPolicy>
    {
        using base = mdspan<T, Extents, LayoutPolicy, AccessorPolicy>;

        using element_type      = typename base::element_type;
        using extents_type      = typename base::extents_type;
        using layout_type       = typename base::layout_type;
        using accessor_type     = typename base::accessor_type;
        using mapping_type      = typename base::mapping_type;
        using value_type        = typename base::value_type;
        using index_type        = typename base::index_type;
        using size_type         = typename base::size_type;
        using rank_type         = typename base::rank_type;
        using data_handle_type  = typename base::data_handle_type;
        using reference         = typename base::reference;

        // constexpr static std::size_t extent = base::extent;

        static constexpr std::size_t rank() noexcept { return extents_type::rank(); }
        static constexpr std::size_t rank_dynamic() noexcept { return extents_type::rank_dynamic(); }
        static constexpr std::size_t static_extent(std::size_t r) noexcept { return extents_type::static_extent(r); }

        emu::capsule capsule;

        /***************************************************/
        /* default constructor (1)*/
        constexpr mdcontainer() = default;

        /***************************************************/
        /* variadic constructor (2)*/
        template<
            typename DataHolder
          , typename ... OtherIndexTypes
        >
        requires(
            (not cpts::mdcontainer<DataHolder>)
         && (std::is_convertible_v<OtherIndexTypes, index_type> && ...)
         && (sizeof...(OtherIndexTypes) == rank()) || (sizeof...(OtherIndexTypes) == rank_dynamic())
        )
        mdcontainer(data_handle_type p, DataHolder&& dh,  OtherIndexTypes... exts)
            : base(p, exts...)
            , capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder, typename ... OtherIndexTypes>
        mdcontainer(DataHolder&& dh, OtherIndexTypes... exts)
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts...)
        {}

        /***************************************************/
        /*span constructor (3)*/
        template<
            typename DataHolder,
            typename OtherIndexType,
            std::size_t N
        >
        requires (
            (not cpts::mdcontainer<DataHolder>)
         && (std::is_convertible_v<OtherIndexType, index_type>)
         && (N == rank()) || (N == rank_dynamic())
        )
        explicit(N != Extents::rank_dynamic())
        constexpr mdcontainer(
            data_handle_type p,
            DataHolder &&dh,
            const std::span<OtherIndexType, N>& exts
        )
            : base(p, exts)
            , capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder, class OtherIndexType, std::size_t N >
        constexpr mdcontainer(DataHolder &&dh, emu::span<OtherIndexType, N> exts )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts)
        {}

        /***************************************************/
        /* array constructor (4)*/
        template<typename DataHolder, class OtherIndexType, std::size_t N >
        requires (
            (not cpts::mdcontainer<DataHolder>)
         && (std::is_convertible_v<OtherIndexType, index_type>)
         && (N == rank()) || (N == rank_dynamic())
        )
        explicit(N != Extents::rank_dynamic())
        constexpr mdcontainer( data_handle_type p, DataHolder &&dh, const std::array<OtherIndexType, N>& exts )
            : base(p,exts),
              capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder, class OtherIndexType, std::size_t N >
        explicit(N != Extents::rank_dynamic())
        constexpr mdcontainer( DataHolder &&dh, const std::array<OtherIndexType, N>& exts )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts)
        {}

        /***************************************************/
        /*constructor (5)  */
        template<typename DataHolder>
        requires (not cpts::mdcontainer<DataHolder>)
        constexpr mdcontainer( data_handle_type p, DataHolder &&dh, const extents_type& ext )
            : base(p,ext)
            , capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder>
        constexpr mdcontainer(DataHolder &&dh, const extents_type& ext )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), ext)
        {}

        /***************************************************/
        /*constructor (6)*/
        template<typename DataHolder >
        requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer( data_handle_type p, DataHolder &&dh, const mapping_type &m )
            : base(p,m)
            , capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder >
        mdcontainer(DataHolder &&dh, const mapping_type &m )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),m)
        {}

        /***************************************************/
        /*constructor (7)*/
        template<typename DataHolder>
        requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer( data_handle_type p, DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : base(p, m, a),
              capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder>
        mdcontainer(DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),m,a)
        {}

        /***************************************************/
        /*constructor (8)*/
        // TODO ?

        /***************************************************/
        /* copy & move constructor (9)*/
        constexpr mdcontainer( const mdcontainer& rhs ) = default;
        constexpr mdcontainer( mdcontainer&& rhs ) = default;

        // template<typename DataHolder>
        //     requires (not cpts::container<DataHolder>)
        // mdcontainer(base s, DataHolder&& dh)
        //     : base(s)
        //     , capsule(EMU_FWD(dh))
        // {}

        mdcontainer(const base& s, emu::capsule capsule)
            : base(s), capsule(std::move(capsule))
        {}

        mdcontainer& operator=(const mdcontainer&) = default;
        mdcontainer& operator=(mdcontainer&&) = default;

        constexpr ~mdcontainer() = default;

        auto use_count() const {
            return capsule.use_count();
        }

        /*observers*/
        using base::size;
        using base::empty;
        using base::stride;
        using base::extents;
        using base::data_handle;
        using base::mapping;
        using base::accessor;
        using base::is_unique;
        using base::is_exhaustive;
        using base::is_strided;
        using base::is_always_unique;
        using base::is_always_exhaustive;
        using base::is_always_strided;

        constexpr index_type extent(std::size_t r) const noexcept { return extents().extent(r); };

        /*accessors*/
        using base::operator();


    };


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
    mdcontainer<T, emu::_nd< sizeof...(OtherIndexTypes)>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(OtherIndexTypes... exts){
        constexpr std::size_t nd = sizeof...(OtherIndexTypes);
        size_t size = (1 * ... * exts);
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::_nd<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr),exts...);
    }

    /*constructor 3 (span<exts>)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::_nd<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const emu::span<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e : exts ){size = size * e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::_nd<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr),exts);
    }

    /*constructor 4 (array<exts>)*/
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::_nd<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const std::array<OtherIndexType, N>& exts){
        std::size_t size = 1;
        for(auto e :exts ){size = size * e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::_nd<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(),
            std::move(u_ptr),exts);
    }

    /*constructor 5 (dextents<std::size_t, N>)*/
    template<typename T,
             std::size_t N,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>
             >
    mdcontainer<T, _nd<N>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(const _nd<N> &exts){
        std::size_t size = 1;
        for(int i=0;i<N;i++){
            size *= exts.extent(i);
        }
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, _nd<N>, LayoutPolicy, AccessorPolicy>(u_ptr.get(), std::move(u_ptr),
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

    template <class ElementType, class Extents, class LayoutPolicy,
          class AccessorPolicy, class... SliceSpecifiers>
    constexpr auto submdcontainer(
        const mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy> &src,
        SliceSpecifiers... slices)
    {
        return mdcontainer(emu::submdspan(src, EMU_FWD(slices)...), src.capsule);
    }

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
