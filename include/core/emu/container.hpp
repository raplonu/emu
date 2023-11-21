#pragma once

#include <span>
#include <memory>
#include <span>
#include <ranges>

namespace emu
{

    struct capsule {

        struct interface {
            virtual ~interface() = default;
        };

        template<typename DataHolder>
        struct impl : interface {
            impl(auto&& d) : data_holder(EMU_FWD(d)) {}

            DataHolder data_holder;
        };

        capsule() = default;

        template<typename DataHolder>
            requires (not std::same_as<capsule, std::decay_t<DataHolder>>)
        capsule(DataHolder&& d) : holder(std::make_shared<impl<DataHolder>>(EMU_FWD(d))) {}

        std::shared_ptr<interface> holder;

        std::size_t use_count() const {
            return holder.use_count();
        }
    };

    template<typename T, std::size_t Extent = std::dynamic_extent>
    struct container;

namespace cpts
{

    // Check if T is a container or derived from one.
    template<typename T>
    concept container = std::derived_from<
        std::decay_t<T>,
        emu::container<
            typename std::decay_t<T>::element_type,
            std::decay_t<T>::extent
        >
    >;

} // namespace cpts

    template<typename T, std::size_t Extent>
    struct container : std::span<T, Extent>
    {
        using base = std::span<T, Extent>;

        using element_type =     typename base::element_type;
        using value_type =       typename base::value_type;
        using size_type =        typename base::size_type;
        using difference_type =  typename base::difference_type;
        using pointer =          typename base::pointer;
        using const_pointer =    typename base::const_pointer;
        using reference =        typename base::reference;
        using const_reference =  typename base::const_reference;
        using iterator =         typename base::iterator;
        using reverse_iterator = typename base::reverse_iterator;

        // constexpr static std::size_t extent = base::extent;

        emu::capsule capsule;

        using base::base;

        container() = default;

        template<typename DataHolder>
            requires (not cpts::container<DataHolder>)
        container(T* ptr, std::size_t size, DataHolder&& dh)
            : base(ptr, ptr + size)
            , capsule(EMU_FWD(dh))
        {}


        template<typename DataHolder>
            requires (not cpts::container<DataHolder>)
        container(base s, DataHolder&& dh)
            : base(s)
            , capsule(EMU_FWD(dh))
        {}

        template<typename D>
            requires (not cpts::container<D>) and (not std::ranges::borrowed_range<D>) /* and cpts::contigious_sized_range<D> */
        container(D&& d)
            : base(std::ranges::data(d), std::ranges::size(d))
            , capsule(EMU_FWD(d))
        {}

        // template<typename Deleter>
        //     requires std::is_invocable_v<Deleter>
        // container(T* ptr, std::size_t size, Deleter&& deleter )
        //     : base(ptr, ptr + size)
        //     , capsule(std::make_shared<detail::impl<scoped<void, std::decay_t<Deleter>>>>(std::forward<Deleter>(deleter)))
        // {}

        container(base s, emu::capsule capsule)
            : base(s), capsule(std::move(capsule))
        {}

        container(const container&) = default;
        container(container&&) = default;

        container& operator=(const container&) = default;
        container& operator=(container&&) = default;

        auto use_count() const {
            return capsule.use_count();
        }

        template< std::size_t Offset, std::size_t Count = std::dynamic_extent >
        constexpr auto subspan() const {
            return from_span(base::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type Offset, size_type Count = std::dynamic_extent ) const {
            return from_span(base::subspan(Offset, Count));
        }

        template< std::size_t Count >
        constexpr auto first() const {
            return from_span(base::template first<Count>());
        }

        constexpr auto first( size_type Count ) const {
            return from_span(base::first(Count));
        }

        template< std::size_t Count >
        constexpr std::span<element_type, Count> last() const {
            return from_span(base::template last<Count>());
        }

        constexpr std::span<element_type, std::dynamic_extent> last( size_type Count ) const {
            return from_span(base::last(Count));
        }

        template<typename NT, std::size_t NExtent>
        constexpr auto from_span(std::span<NT, NExtent> s) const {
            return container<NT, NExtent>(s, capsule);
        }

        using base::begin;
        using base::end;
        using base::rbegin;
        using base::rend;

        using base::front;
        using base::back;
        using base::operator[];
        using base::data;

        using base::size;
        using base::size_bytes;
        using base::empty;

    };

    template< class T, std::size_t N >
    auto as_bytes( container<T, N> c ) noexcept {
        return c.from_span(std::as_bytes(c));
    }

    template< class T, std::size_t N >
    auto as_writable_bytes( container<T, N> s ) noexcept {
        return s.from_span(std::as_writable_bytes(s));
    }

} // namespace emu






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

// } // namespace emu
