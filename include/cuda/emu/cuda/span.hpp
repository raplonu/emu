#pragma once

#include <emu/span.hpp>
#include <emu/cuda.h>

#include <type_traits>
#include <ranges>
#include <array>
#include <iterator>

namespace emu
{

namespace cuda
{

    template<typename ElementType, std::size_t Extent = std::dynamic_extent>
    struct span : protected std::span<ElementType, Extent>
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

        template< std::size_t N >
        span( std::type_identity_t<element_type> (&arr)[N] ) = delete;

        template< class U, std::size_t N >
        span( std::array<U, N>& arr ) = delete;

        template< class U, std::size_t N >
        span( const std::array<U, N>& arr ) = delete;

        span(const span&) noexcept = default;
        span(span&&) noexcept = default;

        span(const base&) noexcept = delete;
        span(base&&) noexcept = delete;

        constexpr span& operator=(const span&) noexcept = default;
        constexpr span& operator=(span&&) noexcept = default;

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

        using base::first;
        using base::last;
        using base::subspan;

    };

    // Deduction Guides
    template <std::contiguous_iterator Iter, typename End>
    span(Iter, End) -> span<std::remove_reference_t<std::iter_reference_t<Iter>>, std::dynamic_extent>;

    template <std::ranges::contiguous_range _Range>
    span(_Range &&) -> span<std::remove_reference_t<std::ranges::range_reference_t<_Range &>>, std::dynamic_extent>;

} // namespace cuda

namespace detail
{

    template <typename T, auto Extent>
    constexpr auto as_std_span(cuda::span<T, Extent> s) noexcept {
        return static_cast<std::span<T, Extent>>(s);
    }

} // namespace detail


    template< class T, std::size_t N >
    auto as_bytes(cuda::span<T, N> s) noexcept {
        return cuda::span(std::as_bytes(detail::as_std_span(s)));
    }

    template< class T, std::size_t N >
    auto as_writable_bytes(cuda::span<T, N> s) noexcept {
        return cuda::span(std::as_writable_bytes(detail::as_std_span(s)));
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(cuda::span<std::byte, Extent> s) noexcept {
        return cuda::span(as_t(detail::as_std_span(s)));
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(cuda::span<const std::byte, Extent> s) noexcept {
        return cuda::span(as_t(detail::as_std_span(s)));
    }

} // namespace emu

// I don't think we need this anymore
// namespace std::ranges
// {
// // see https://en.cppreference.com/w/cpp/ranges/borrowed_range
// template <typename ElementType, std::size_t Extent, typename LocationType>
// inline constexpr bool enable_borrowed_range<emu::cuda::span<ElementType, LocationType, Extent>>
//     = true;

// // see https://en.cppreference.com/w/cpp/ranges/view
// template <typename ElementType, std::size_t Extent, typename LocationType>
// inline constexpr bool enable_view<emu::cuda::span<ElementType, LocationType, Extent>>
//     =  Extent == 0 || Extent == std::dynamic_extent;

// } // namespace std::ranges
