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


    template<typename ElementType> using mdcontainer_0d   = mdcontainer<ElementType, _0d>;

    template<typename ElementType> using mdcontainer_1d   = mdcontainer<ElementType, _1d>;
    template<typename ElementType> using mdcontainer_2d   = mdcontainer<ElementType, _2d>;
    template<typename ElementType> using mdcontainer_3d   = mdcontainer<ElementType, _3d>;

    template<typename ElementType> using mdcontainer_1d_c = mdcontainer<ElementType, _1d>;
    template<typename ElementType> using mdcontainer_2d_c = mdcontainer<ElementType, _2d>;
    template<typename ElementType> using mdcontainer_3d_c = mdcontainer<ElementType, _3d>;

    template<typename ElementType> using mdcontainer_1d_f = mdcontainer<ElementType, _1d, layout_f>;
    template<typename ElementType> using mdcontainer_2d_f = mdcontainer<ElementType, _2d, layout_f>;
    template<typename ElementType> using mdcontainer_3d_f = mdcontainer<ElementType, _3d, layout_f>;

    template<typename ElementType> using mdcontainer_1d_s = mdcontainer<ElementType, _1d, layout_stride>;
    template<typename ElementType> using mdcontainer_2d_s = mdcontainer<ElementType, _2d, layout_stride>;
    template<typename ElementType> using mdcontainer_3d_s = mdcontainer<ElementType, _3d, layout_stride>;

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
            requires (not cpts::mdcontainer<DataHolder>)
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
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts){}

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
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), exts){}

        /*constructor (5)  */
        template<typename DataHolder>
        constexpr mdcontainer( T* ptr, DataHolder &&dh, const extents_type& ext )
            : base(ptr,ext),
              capsule(EMU_FWD(dh))
        {}
        template<typename DataHolder>
            requires (not cpts::mdcontainer<DataHolder>)
        constexpr mdcontainer(DataHolder &&dh, const extents_type& ext )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh), ext){}

        /*constructor (6)*/
        template<typename DataHolder >
        mdcontainer( T* ptr, DataHolder &&dh, const mapping_type &m )
            : base(ptr,m),
              capsule(EMU_FWD(dh))
        {}
        template<typename DataHolder >
            requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer(DataHolder &&dh, const mapping_type &m )
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh)){}

        /*constructor (7)*/
        template<typename DataHolder>
        mdcontainer( T* ptr, DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : base(ptr,m,a),
              capsule(EMU_FWD(dh))
        {}
        template<typename DataHolder>
            requires (not cpts::mdcontainer<DataHolder>)
        mdcontainer(DataHolder &&dh, const mapping_type &m, const accessor_type &a)
            : mdcontainer(std::ranges::data(dh),EMU_FWD(dh),m,a){}

        /*constructor (8) -> remove it */
        /*template< class OtherElementType, class OtherExtents,
          class OtherLayoutPolicy, class OtherAccessor >
    constexpr explicit( see below )
        mdspan( const mdspan<OtherElementType, OtherExtents,
                             OtherLayoutPolicy, OtherAccessor>& other );*/

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

        // template<typename Deleter>
        //     requires std::is_invocable_v<Deleter>
        // container(T* ptr, std::size_t size, Deleter&& deleter )
        //     : base(ptr, ptr + size)
        //     , capsule(std::make_shared<detail::impl<scoped<void, std::decay_t<Deleter>>>>(
        //              std::forward<Deleter>(deleter)))
        // {}

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
        using base::operator[];

    };


    /***************************************************/
    // Deduction guide

    template< class CArray >
    requires(std::is_array_v<CArray> && std::rank_v<CArray> == 1)
    mdcontainer( CArray& ) -> mdcontainer<std::remove_all_extents_t<CArray>,
                  emu::extents<std::size_t, std::extent_v<CArray, 0>>>;

    template< class Pointer >
    requires(std::is_pointer_v<std::remove_reference_t<Pointer>>)
    mdcontainer( Pointer&& ) -> mdcontainer<std::remove_pointer_t<std::remove_reference_t<Pointer>>,
                  emu::extents<size_t>>;

    template< class ElementType, class... Integrals >
    requires((std::is_convertible_v<Integrals, std::size_t> && ...) && sizeof...(Integrals) > 0)
    explicit mdcontainer( ElementType*, Integrals... ) -> mdcontainer<ElementType,
                emu::dextents<std::size_t, sizeof...(Integrals)>>;

    template< class ElementType, class OtherIndexType, std::size_t N >
    mdcontainer( ElementType*, std::span<OtherIndexType, N> )
        -> mdcontainer<ElementType, emu::dextents<std::size_t, N>>;

    template< class ElementType, class OtherIndexType, std::size_t N >
    mdcontainer( ElementType*, const std::array<OtherIndexType, N>& )
        -> mdcontainer<ElementType, emu::dextents<std::size_t, N>>;

    template< class ElementType, class IndexType, std::size_t... ExtentsPack >
    mdcontainer( ElementType*, const emu::extents<IndexType, ExtentsPack...>& )
        -> mdcontainer<ElementType, emu::extents<IndexType, ExtentsPack...>>;

    template< class ElementType, class MappingType >
    mdcontainer( ElementType*, const MappingType& )
        -> mdcontainer<ElementType, typename MappingType::extents_type,
                  typename MappingType::layout_type>;

    template< class MappingType, class AccessorType >
    mdcontainer( const typename AccessorType::data_handle_type&, const MappingType&, const AccessorType& )
        -> mdcontainer<typename AccessorType::element_type,
                  typename MappingType::extents_type,
                  typename MappingType::layout_type, AccessorType>;

    /***************************************************/
    /* factories */
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename ...Extents>
    mdcontainer<T, emu::_nd< sizeof...(Extents)>,LayoutPolicy,AccessorPolicy>
    make_mdcontainer(Extents... exts){
        constexpr std::size_t nd = sizeof...(Extents);
        std::size_t size=1;
        for(auto e :{exts...}){size = size*e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::_nd<nd>, LayoutPolicy, AccessorPolicy>(u_ptr.get(), std::move(u_ptr),exts...);
    }

//,
    template<typename T,
             typename LayoutPolicy = layout_right,
             typename AccessorPolicy = default_accessor<T>,
             typename OtherIndexType,
             std::size_t N>
    mdcontainer<T, emu::_nd<N>,LayoutPolicy,AccessorPolicy> make_container(
            const emu::span<OtherIndexType, N>& exts){

        std::size_t size=1;
        for(auto e :exts){size = size*e;}
        auto u_ptr = std::make_unique<T[]>(size );
        return mdcontainer<T, emu::_nd<N>, LayoutPolicy, AccessorPolicy>(
            u_ptr.get(), std::move(u_ptr),exts);
    }


//    template< class T, std::size_t N >
//    auto as_bytes( container<T, N> c ) noexcept {
//        return c.from_span(std::as_bytes(c));
//    }
//
//    template< class T, std::size_t N >
//    auto as_writable_bytes( container<T, N> s ) noexcept {
//        return s.from_span(std::as_writable_bytes(s));
//    }



// #include <emu/utility.h>

// #include <emu/misc/location.h>

// // Include for accessor types.
// #include <emu/mdspan.h>
// #include <emu/scoped.h>
// #include <emu/type_traits.hpp>

// #include <memory>

// namespace emu
// {

//     template<typename ViewType>
//     struct container
//     {

//         using view_type = ViewType;

//         struct interface
//         {
//             virtual ~interface() = default;
//         };

//         template<typename Capsule>
//         struct implementation : interface
//         {
//             Capsule capsule_; // maintain a captured object alive if moved into.

//             template<typename C>
//             implementation(view_type view, C&& capsule)
//                 : view(view), capsule_(EMU_FWD(capsule))
//             {}

//             view_type view() override
//             {
//                 return view;
//             }
//         };

//         std::shared_ptr<interface> impl_;
//         view_type view;

//         template<typename C>
//         container(view_type view, C&& capsule)
//             : impl_(std::make_shared<implementation<Capsule>>(view, EMU_FWD(capsule)))
//         {}

//         template<typename C, typename... ViewArgs>
//         container(C&& con, ViewArgs&&... view_args)
//             : container(view_type(con, EMU_FWD(view_args)...), EMU_FWD(con))
//         {}

//     };


namespace spe
{
    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct info_t< mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy> >
    {
        using type = mdcontainer<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

        constexpr auto format_type(fmt::format_context::iterator it) const {
            return []<std::size_t...Is>(fmt::format_context::iterator it, std::index_sequence<Is...>) {
                it = fmt::format_to(it, "mdcontainer<{}, [", boost::core::demangle(typeid(ElementType).name()));

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
                it = fmt::format_to(it, "@{} {}", fmt::ptr(t.data_handle()), std::array{t.extent(Is)...});
                if constexpr (std::same_as<LayoutPolicy, layout_stride>) {
                    it = fmt::format_to(it, "[{}]", std::array{t.stride(Is)...});
                }
                return it;
            }(std::make_index_sequence<Extents::rank()>{});
        }
    };

} // namespace spe

} // namespace emu
