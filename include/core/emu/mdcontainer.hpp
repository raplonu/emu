#pragma once

//#include <container.hpp>

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>
#include <emu/mdspan.hpp>


#include <experimental/mdspan>
#include <memory>
#include <span>
#include <ranges>


namespace emu
{
    // struct capsule defined in container.hpp
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
    using _nd = dextents<std::size_t, N>;

    using _0d = _nd<0>;
    using _1d = _nd<1>;
    using _2d = _nd<2>;
    using _3d = _nd<3>;

    using stdex::mdspan;

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
        std::decay_t<T>,
        emu::mdcontainer<
            typename std::decay_t<T>::element_type,
            typename std::decay_t<T>::extents_type,
            typename std::decay_t<T>::layout_type,
            typename std::decay_t<T>::accessor_type
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

        emu::capsule capsule;

        using base::base;

        /* constructor (1)*/
        constexpr mdcontainer() = default;

        /* constructor (2)*/
        template<typename DataHolder, typename ... OtherIndexTypes>
            requires(  (not cpts::mdcontainer<DataHolder>)
                    && (std::is_convertible_v<OtherIndexTypes, std::size_t> && ...)
                    && sizeof...(OtherIndexTypes) > 0)
        mdcontainer(T* ptr, DataHolder&& dh,  OtherIndexTypes... exts)
            : base(ptr, exts...)
            , capsule(EMU_FWD(dh))
        {}

        template<std::ranges::contiguous_range DataHolder, typename ... OtherIndexTypes>
            requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer(DataHolder&& dh,  OtherIndexTypes... exts)
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),exts...){}

        /*constructor (3)*/
        template<typename DataHolder, class OtherIndexType, std::size_t N >
            constexpr explicit(N != Extents::rank_dynamic())
        mdcontainer( T* ptr, DataHolder &&dh, const emu::span<OtherIndexType, N>& exts )
            : base(ptr, exts)
            , capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder, class OtherIndexType, std::size_t N >
            requires (not cpts::mdcontainer<DataHolder>)
            constexpr explicit(N != Extents::rank_dynamic())
        mdcontainer(DataHolder &&dh, const emu::span<OtherIndexType, N>& exts )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts)
        {}

        /*constructor (4)*/
        template<typename DataHolder, class OtherIndexType, std::size_t N >
            constexpr explicit(N != Extents::rank_dynamic())
        mdcontainer( T* ptr, DataHolder &&dh, const std::array<OtherIndexType, N>& exts )
            : base(ptr,exts),
              capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder, class OtherIndexType, std::size_t N >
            requires (not cpts::mdcontainer<DataHolder>)
            constexpr explicit(N != Extents::rank_dynamic())
        mdcontainer( DataHolder &&dh, const std::array<OtherIndexType, N>& exts )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts)
        {}

        /*constructor (5)  */
        template<typename DataHolder>
        constexpr mdcontainer( T* ptr, DataHolder &&dh, const extents_type& ext )
            : base(ptr,ext),
              capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder>
            requires (not cpts::mdcontainer<DataHolder>)
        constexpr mdcontainer(DataHolder &&dh, const extents_type& ext )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), ext)
        {}

        /*constructor (6)*/
        template<typename DataHolder >
        mdcontainer( T* ptr, DataHolder &&dh, const mapping_type &m )
            : base(ptr,m),
              capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder >
            requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer(DataHolder &&dh, const mapping_type &m )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),m)
        {}

        /*constructor (7)*/
        // ! untested
        template<typename DataHolder>
        mdcontainer( T* ptr, DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : base(ptr,m,a),
              capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder>
            requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer(DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),m,a)
        {}

        /*constructor (8)*/
        // TODO ?

        /*constructor (9)*/
        constexpr mdcontainer( const mdcontainer& rhs ) = default;

        /*constructor (10)*/
        constexpr mdcontainer( mdcontainer&& rhs ) = default;

        template<typename DataHolder>
            requires (not cpts::container<DataHolder>)
        mdcontainer(base s, DataHolder&& dh)
            : base(s)
            , capsule(EMU_FWD(dh))
        {}

        mdcontainer(base s, emu::capsule capsule)
            : base(s), capsule(std::move(capsule))
        {}

        mdcontainer& operator=(const mdcontainer&) = default;
        mdcontainer& operator=(mdcontainer&&) = default;

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

        /*accessors*/
        using base::operator();

    };


    /***************************************************/
    // Deduction guide

    /* constructor 2*/
    template<std::ranges::contiguous_range  D, class ...Exts >
    requires((std::is_convertible_v<Exts, std::size_t> && ...) && sizeof...(Exts) > 0)
    mdcontainer(D&&, Exts... exts )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::dextents<std::size_t, sizeof...(exts)>>;

    template<class T, class D, class ...Exts >
    mdcontainer(T*, D&&, Exts... exts )
        ->  mdcontainer<T, emu::dextents<std::size_t, sizeof...(exts)>
                        >;
    /* constructor 3*/
    template<std::ranges::contiguous_range  D, class Exts, std::size_t N>
    mdcontainer(D&&, const std::span<Exts, N>& )
        ->  mdcontainer<std::ranges::range_value_t<D>, emu::dextents<std::size_t, N>>;

    template<class T, class D, class Exts, std::size_t N>
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
        for(auto e :exts ){size = size * e;}
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


namespace spe
{
    template <typename ElementType, typename Extents, typename LayoutPolicy,
              typename AccessorPolicy>
    struct info_t< mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy> >
    {
        using type = mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        constexpr auto format_type(fmt::format_context::iterator it) const {
            return []<std::size_t...Is>(fmt::format_context::iterator it,
                                        std::index_sequence<Is...>) {
                it = fmt::format_to(it, "mdcontainer<{}, [",
                    boost::core::demangle(typeid(ElementType).name()));

                bool first = true;
                for (auto e : std::array{type::static_extent(Is)...}) {
                    if (not first) it = fmt::format_to(it, ", "); else first = false;
                    it = detail::format_extent(it, e);
                }

                return fmt::format_to(it, "], {}>", layout_name<LayoutPolicy>());

            }(it, std::make_index_sequence<Extents::rank()>{});
        }

        constexpr auto format_value(const type &t, fmt::format_context::iterator it) const {
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                it = fmt::format_to(it, "@{} {}", fmt::ptr(t.data_handle()),
                    std::array{t.extent(Is)...});
                if constexpr (std::same_as<LayoutPolicy, layout_stride>) {
                    it = fmt::format_to(it, "[{}]", std::array{t.stride(Is)...});
                }
                return it;
            }(std::make_index_sequence<Extents::rank()>{});
        }
    };

} // namespace spe

} // namespace emu
