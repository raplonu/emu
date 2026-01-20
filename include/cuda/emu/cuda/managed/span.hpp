#pragma once

#include <emu/detail/basic_span.hpp>
#include <emu/cuda/managed/accessor.hpp>

namespace emu::cuda::managed
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType>>
    using span = emu::detail::basic_span<ElementType, Extent, cuda::managed::accessor<AccessorPolicy> >;

} // namespace emu::cuda::managed
