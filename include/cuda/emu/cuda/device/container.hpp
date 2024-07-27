#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_container.hpp>
#include <emu/cuda.hpp>

namespace emu::cuda::device
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct container : detail::basic_container<ElementType, Extent, cuda::device_source_validator, container<ElementType, Extent>>
    {
        using base = detail::basic_container<ElementType, Extent, cuda::device_source_validator, container>;

        using base::base;

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    template< class It, class EndOrSize >
    container( It, EndOrSize )               -> container< iterator_cv_value<It>, dynamic_extent >;
    template< class It, class EndOrSize, class DataHolder >
    container( It, EndOrSize, DataHolder&& ) -> container< iterator_cv_value<It>, dynamic_extent >;

    template< class T, size_t N >
    container( T (&)[N] )               -> container<T, N>;
    template< class T, size_t N, class DataHolder >
    container( T (&)[N], DataHolder&& ) -> container<T, N>;

    template< typename Range >
    container( Range&& )               -> container< range_cv_value<Range>, dynamic_extent>;
    template< typename Range, class DataHolder >
    container( Range&&, DataHolder&& ) -> container< range_cv_value<Range>, dynamic_extent>;

    template< class T, size_t N >
    container( std::array<T, N>& )       -> container<T, N>;
    template< class T, size_t N >
    container( const std::array<T, N>& ) -> container< const T, N>;

    template< class T, size_t N >
    container( std::span<T, N> )       -> container<T, N>;
    template< class T, size_t N, class DataHolder >
    container( std::span<T, N>, DataHolder&& )       -> container<T, N>;

    template< class T, size_t N >
    container( std::span<const T, N> ) -> container< const T, N>;
    template< class T, size_t N, class DataHolder >
    container( std::span<const T, N>, DataHolder&& ) -> container< const T, N>;

    template< typename T >
    container( std::initializer_list<T> ) -> container< const T, dynamic_extent>;

    template<typename T>
    container<T> make_container(device_cref device, size_t size) {
        auto u_span = cu::memory::device::make_unique_span<T>(device, size);
        return container<T>(u_span.data(), size, std::move(u_span));
    }

    template<typename T>
    container<T> make_container(stream_cref stream, size_t size) {
        region_t region = cu::memory::device::async::allocate(stream, size * sizeof(T));

        return container<T>(
            region.as_span<T>(),
            size,
            scoped{[ptr = region.get(), stream_h = stream.handle()]{ cudaFreeAsync(ptr, stream_h); }}
        );
    }

} // namespace emu::cuda::device
