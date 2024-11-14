#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_mdcontainer.hpp>
#include <emu/cuda/device/location_policy.hpp>

namespace emu::cuda::device
{

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer : emu::detail::basic_mdcontainer<
        T, Extents, LayoutPolicy, AccessorPolicy, cuda::device_location_policy,
        mdcontainer<T, Extents, LayoutPolicy, AccessorPolicy>
    >
    {
        using base = emu::detail::basic_mdcontainer< T, Extents, LayoutPolicy, AccessorPolicy, cuda::device_location_policy, mdcontainer >;

        using base::base;

        template<typename OT, typename OExtents, typename OLayoutPolicy, typename OAccessorPolicy>
        constexpr auto from_mdspan(stdex::mdspan<OT, OExtents, OLayoutPolicy, OAccessorPolicy> md) const noexcept {
            return mdcontainer<OT, OExtents, OLayoutPolicy, OAccessorPolicy>(md, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_MDCONTAINER_DEDUCTION_GUIDES

    EMU_DEFINE_MDCONTAINER_ALIAS

    // template<typename T>
    // container<T> make_container(device_cref device, size_t size) {
    //     auto u_span = cu::memory::device::make_unique_span<T>(device, size);
    //     return container<T>(u_span.data(), size, std::move(u_span));
    // }

    // template<typename T>
    // container<T> make_container(stream_cref stream, size_t size) {
    //     region_t region = cu::memory::device::async::allocate(stream, size * sizeof(T));

    //     return container<T>(
    //         region.as_span<T>(),
    //         size,
    //         scoped{[ptr = region.get(), stream_h = stream.handle()]{ cudaFreeAsync(ptr, stream_h); }}
    //     );
    // }


} // namespace emu::cuda::device
