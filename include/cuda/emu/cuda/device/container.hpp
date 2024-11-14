#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_container.hpp>
#include <emu/cuda/device/mdcontainer.hpp>
#include <emu/cuda.hpp>
#include <emu/mdalgo.hpp>

namespace emu
{
namespace cuda::device
{

    template <typename ElementType, size_t Extent>
    struct container : emu::detail::basic_container<ElementType, Extent, cuda::device_location_policy, container<ElementType, Extent>>
    {
        using base = emu::detail::basic_container<ElementType, Extent, cuda::device_location_policy, container>;

        using base::base;

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T> make_container(device_cref device, size_t size) {
        auto u_span = cu::memory::device::make_unique_span<T>(device, size);
        return container<T>(u_span.data(), size, std::move(u_span));
    }

    // template<typename T>
    // container<T> make_container(stream_cref stream, size_t size) {
    //     region_t region = cu::memory::device::async::allocate(stream, size * sizeof(T));

    //     return container<T>(
    //         region.as_span<T>(),
    //         size,
    //         scoped{[ptr = region.get(), stream_h = stream.handle()]{ cudaFreeAsync(ptr, stream_h); }}
    //     );
    // }

} // namespace cuda::device

    template<typename T, std::size_t Extent>
    struct spe::md_converter<cuda::device::container<T, Extent>>
    {
        static constexpr auto convert(const cuda::device::container<T, Extent> &s) noexcept {
            return cuda::device::mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >(s.data(), s.capsule(), emu::exts_flag, s.size());
        }
    };


} // namespace emu
