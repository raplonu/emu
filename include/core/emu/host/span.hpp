#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_span.hpp>

namespace emu::host
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct span : detail::basic_span<ElementType, Extent, host::source_validator, span<ElementType, Extent> >
    {
        using base = detail::basic_span<ElementType, Extent, host::source_validator, span >;

        using base::base;

        template<typename OT, size_t OExtent>
        constexpr auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return span<OT, OExtent>(sp.data(), sp.size());
        }
    };

    template< class It, class EndOrSize >
    span( It, EndOrSize ) -> span<iterator_cv_value<It>, dynamic_extent>;

    template< class T, size_t N >
    span( T (&)[N] ) -> span<T, N>;

    template< typename Range >
    span( Range&& ) -> span< range_cv_value<Range>, dynamic_extent>;

    template< class T, size_t N >
    span( std::array<T, N>& ) -> span<T, N>;

    template< class T, size_t N >
    span( const std::array<T, N>& ) -> span< const T, N>;

    template< class T, size_t N >
    span( std::span<T, N>& ) -> span<T, N>;

    template< class T, size_t N >
    span( std::span<const T, N>& ) -> span< const T, N>;

    template< typename T >
    span( std::initializer_list<T> ) -> span< const T, dynamic_extent>;

} // namespace emu::host
