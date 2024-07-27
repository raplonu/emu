#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_span.hpp>
#include <emu/info.hpp>
#include <emu/type_name.hpp>

#include <span>

namespace emu
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct span : detail::basic_span<ElementType, Extent, no_source_validator, span<ElementType, Extent> >
    {
        using base = detail::basic_span<ElementType, Extent, no_source_validator, span >;

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

namespace spe
{
    template <typename T, size_t Extent>
    struct info_t< span<T, Extent> >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "std::span<{}, ", type_name<T>);
            it = detail::format_extent(it, Extent);
            return fmt::format_to(it, ">");
        }

        constexpr auto format_value(const span<T, Extent> &t, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "@{}[{}]", fmt::ptr(t.data()), t.size());
        }
    };

} // namespace spe

} // namespace emu
