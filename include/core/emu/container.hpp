#pragma once

#include <emu/concepts.hpp>
#include <emu/capsule.hpp>

#include <span>
#include <span>
#include <ranges>

namespace emu
{

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
    struct container : protected std::span<T, Extent>
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

        template<typename U, std::size_t N>
        container(std::array<U, N>&) = delete;

        template<typename DataHolder>
            requires (not is_lref<DataHolder>) and (not cpts::container<DataHolder>)
        container(T* ptr, std::size_t size, DataHolder&& dh)
            : base(ptr, ptr + size)
            , capsule(EMU_FWD(dh))
        {}

        template<typename DataHolder>
            requires (not is_lref<DataHolder>) and (not cpts::container<DataHolder>)
        container(base s, DataHolder&& dh)
            : base(s)
            , capsule(EMU_FWD(dh))
        {}

        template<cpts::contiguous_sized_range DataHolder>
            requires (not is_lref<DataHolder>)
                 and (not cpts::container<DataHolder>)
                 and (not std::ranges::borrowed_range<DataHolder>)
                 and (not cpts::array<DataHolder>)
        container(DataHolder&& d)
            : base(d) // needs to be contiguous and sized
            , capsule(EMU_FWD(d)) // not borrowed, so we need to keep it alive.
        {}

        template<cpts::contiguous_sized_range DataHolder>
            requires is_lref<DataHolder>
                 and (not cpts::container<DataHolder>)
                //  and (std::ranges::borrowed_range<DataHolder>)
                 and (not cpts::array<DataHolder>)
        container(DataHolder&& d)
            : base(d) // needs to be contiguous and sized
            , capsule() // borrowed, so we don't need to keep it alive.
        {}

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
        constexpr auto subcontainer() const {
            return from_span(base::template subspan<Offset, Count>());
        }

        constexpr auto subcontainer( size_type Offset, size_type Count = std::dynamic_extent ) const {
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

    template< class It, class EndOrSize >
    container( It, EndOrSize ) -> container<std::remove_reference_t<std::iter_reference_t<It>>>;
    template< class T, std::size_t N >
    container( T (&)[N] ) -> container<T, N>;
    template< class T, std::size_t N >
    container( std::array<T, N>& ) -> container<T, N>;
    template< class T, std::size_t N >
    container( const std::array<T, N>& ) -> container<const T, N>;
    template< class R >
    container( R&& ) -> container<std::remove_reference_t<std::ranges::range_reference_t<R>>>;

    template<typename T>
    container<T> make_container(std::size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

    template< class T, std::size_t N >
    auto as_bytes( container<T, N> c ) noexcept {
        return c.from_span(std::as_bytes(c));
    }

    template< class T, std::size_t N >
    auto as_writable_bytes( container<T, N> s ) noexcept {
        return s.from_span(std::as_writable_bytes(s));
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(container<std::byte, Extent> sp) noexcept {
        return sp.from_span(as_t<NewType>(span(sp)));
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(container<const std::byte, Extent> sp) noexcept {
        return sp.from_span(as_t<const NewType>(span(sp)));
    }

} // namespace emu

// emu::container may own the data it points to. If it does, it will be destroyed when the last
// container pointing to it is destroyed.
// we cannot safely borrow the container.
template <typename T, std::size_t N>
inline constexpr bool std::ranges::enable_borrowed_range<emu::container<T, N>> = false;
