#pragma once

#include <emu/detail/basic_container.hpp>
#include <emu/cuda/managed/accessor.hpp>

namespace emu::cuda::managed
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType> >
    using container = emu::detail::basic_container<ElementType, Extent, cuda::managed::accessor<AccessorPolicy> >;

    template<typename T>
    container<T> make_container(const device_t& device, size_t size) {
        auto u_ptr = managed::make_unique<T[]>(device, size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace emu::cuda::managed
