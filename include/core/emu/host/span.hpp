#pragma once

#include <emu/detail/basic_span.hpp>
#include <emu/host/accessor.hpp>

namespace emu::host
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType>>
    using span = emu::detail::basic_span<ElementType, Extent, host::accessor<AccessorPolicy> >;

} // namespace emu::host
