#ifndef EMU_SPAN_H
#define EMU_SPAN_H

#include <emu/type_traits.h>
#include <emu/utility.h>
#include <emu/location.h>

#include <array>
#include <ranges>
#include <span>
#include <iterator>
#include <ranges>

namespace emu
{

namespace detail
{
    template<typename ElementType, typename Location, std::size_t Extent = std::dynamic_extent>
    struct span : std::span<ElementType, Extent>
    {
        using base = std::span<ElementType, Extent>;

        using element_type               = typename base::element_type;
        using value_type                 = typename base::value_type;
        using size_type                  = typename base::size_type;
        using difference_type            = typename base::difference_type;
        using pointer                    = typename base::pointer;
        using const_pointer              = typename base::const_pointer;
        using reference                  = typename base::reference;
        using const_reference            = typename base::const_reference;
        using iterator                   = typename base::iterator;
        using reverse_iterator           = typename base::reverse_iterator;

        using base::extent;

        using location_type              = Location;


        constexpr span() EMU_NOEXCEPT_EXPR(location_type()) = default;

        template<std::contiguous_iterator It>
        constexpr explicit(extent != std::dynamic_extent)
        span(It begin, size_type count, location_type location = {}) noexcept:
            base(begin, count), location_(location)
        {}

        template<std::contiguous_iterator It, std::sized_sentinel_for<It> End>
        constexpr explicit(extent != std::dynamic_extent)
        span(It begin, End end, location_type location = {}) noexcept:
            base(begin, end), location_(location)
        {}

        template<std::size_t N>
        constexpr span(std::type_identity_t<element_type> (&arr)[N], location_type location = {}) noexcept:
            base(arr), location_(location)
        {}

        template<typename T, std::size_t N>
        constexpr span(std::array<T, N>& arr, location_type location = {}) noexcept:
            base(arr), location_(location)
        {}

        template<typename T, std::size_t N>
        constexpr span(const std::array<T, N>& arr, location_type location = {}) noexcept:
            base(arr), location_(location)
        {}

        template<typename Range>
        constexpr span(Range&& range, location_type location = {}) noexcept:
            base(range), location_(location)
        {}

        template <typename OtherElementType, std::size_t N>
        constexpr explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
        span(const span<OtherElementType, location_type, N>& other) noexcept:
            base(other), location_(other.location())
        {}

        template <typename OtherElementType, std::size_t N>
        constexpr explicit(extent != std::dynamic_extent && N == std::dynamic_extent)
        span(const std::span<OtherElementType, N>& other, location_type location = {}) noexcept:
            base(other), location_(location)
        {}


        constexpr span(span&&) noexcept = default;
        constexpr span(const span&) noexcept = default;

        constexpr span& operator=(const span&) noexcept = default;
        constexpr span& operator=(span&&) noexcept = default;

        template <std::size_t Count>
        constexpr auto first() const noexcept {
            return as_span_t(base::template first<Count>());
        }

        template <std::size_t Count>
        constexpr auto last() const noexcept {
            return as_span_t(base::template last<Count>());
        }

        constexpr auto first(size_type count) const noexcept {
            return as_span_t(base::first(count));
        }

        constexpr auto last(size_type count) const noexcept {
            return as_span_t(base::last(count));
        }

        template< std::size_t Offset, std::size_t Count = std::dynamic_extent >
        constexpr auto subspan() const noexcept {
            return as_span_t(base::template subspan<Offset, Count>());
        }

        constexpr auto subspan( size_type offset, size_type count = std::dynamic_extent ) const noexcept {
            return as_span_t(base::subspan(offset, count));
        }

        constexpr location_type location() const noexcept {
            return location_;
        }

    private:
        [[no_unique_address]] location_type location_;

        template<typename T, std::size_t N>
        constexpr auto as_span_t(std::span<T, N> s) const noexcept {
            return span<T, location_type, N>(s, location());
        }
    };

    // // Deduction Guides
    template <typename T, typename LocationType, std::size_t Extent>
    span(T (&)[Extent]) -> span<T, LocationType, Extent>;

    template <typename T, typename LocationType, std::size_t Extent>
    span(std::array<T, Extent> &) -> span<T, LocationType, Extent>;

    template <typename T, typename LocationType, std::size_t Extent>
    span(const std::array<T, Extent> &) -> span<const T, LocationType, Extent>;

    template <std::contiguous_iterator Iter, typename End, typename LocationType>
    span(Iter, End) -> span<std::remove_reference_t<std::iter_reference_t<Iter>>, LocationType, std::dynamic_extent>;

    template <std::ranges::contiguous_range _Range, typename LocationType>
    span(_Range &&) -> span<std::remove_reference_t<std::ranges::range_reference_t<_Range &>>, LocationType, std::dynamic_extent>;

} // namespace detail

    /**
     * @brief The class template span describes an object that can refer to a contiguous sequence of objects
     * with the first element of the sequence at position zero.
     *
     * A span can either have a static extent, in which case the number of elements in the sequence is known
     * and encoded in the type, or a dynamic extent.
     * Please refer to https://en.cppreference.com/w/cpp/container/span for more detail.
     */
    template<typename ElementType, std::size_t Extent = std::dynamic_extent>
    using span = detail::span<ElementType, location::host, Extent>;

namespace detail
{

    template <typename T, auto Extent>
    constexpr auto byte_extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent * sizeof(T);

} // namespace detail

    template <typename Type, std::size_t Extent, typename LocationType>
    constexpr auto as_bytes(detail::span<Type, LocationType, Extent> sp) noexcept {
        auto data = reinterpret_cast<const std::byte *>(sp.data());
        auto size = sp.size_bytes();
        auto location = sp.location();
        constexpr auto extent = detail::byte_extent<Type, Extent>;

        return detail::span<const std::byte, LocationType, extent>{data, size, location};
    }

    template <typename Type, std::size_t Extent, typename LocationType>
        requires(not std::is_const_v<Type>)
    constexpr auto as_writable_bytes(detail::span<Type, LocationType, Extent> sp) noexcept {
        auto data = reinterpret_cast<std::byte *>(sp.data());
        auto size = sp.size_bytes();
        auto location = sp.location();
        constexpr auto extent = detail::byte_extent<Type, Extent>;

        return detail::span<std::byte, LocationType, extent>{data, size, location};
    }

    template <typename NewType, std::size_t Extent, typename LocationType>
    constexpr auto as_t(detail::span<std::byte, LocationType, Extent> sp) noexcept {
        auto data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        auto location = sp.location();
        constexpr auto extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent / sizeof(NewType);

        return detail::span<NewType, LocationType, extent>{data, size, location};
    }

    template <typename NewType, std::size_t Extent, typename LocationType>
    constexpr auto as_t(detail::span<const std::byte, LocationType, Extent> sp) noexcept {
        auto data = reinterpret_cast<const NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        auto location = sp.location();
        constexpr auto extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent / sizeof(NewType);

        return detail::span<const NewType, LocationType, extent>{data, size, location};
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(std::span<std::byte, Extent> sp) noexcept {
        auto data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        constexpr auto extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent / sizeof(NewType);

        return std::span<NewType, extent>{data, size};
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(std::span<const std::byte, Extent> sp) noexcept {
        auto data = reinterpret_cast<const NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        constexpr auto extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent / sizeof(NewType);

        return std::span<const NewType, extent>{data, size};
    }

} // namespace emu

namespace std::ranges
{
// see https://en.cppreference.com/w/cpp/ranges/borrowed_range
template <typename ElementType, std::size_t Extent, typename LocationType>
inline constexpr bool enable_borrowed_range<emu::detail::span<ElementType, LocationType, Extent>>
    = true;

// see https://en.cppreference.com/w/cpp/ranges/view
template <typename ElementType, std::size_t Extent, typename LocationType>
inline constexpr bool enable_view<emu::detail::span<ElementType, LocationType, Extent>>
    =  Extent == 0 || Extent == std::dynamic_extent;

} // namespace std::ranges

#endif //EMU_SPAN_H