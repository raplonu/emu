#pragma once

#include <emu/detail/basic_span.hpp>
#include <emu/cuda/device/accessor.hpp>

namespace emu::cuda::device
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType>>
    using span = emu::detail::basic_span<ElementType, Extent, cuda::device::accessor<AccessorPolicy> >;

} // namespace emu::cuda::device
