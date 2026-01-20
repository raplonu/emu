#pragma once

#include <emu/detail/basic_span.hpp>

namespace emu
{

    template <typename ElementType, size_t Extent = std::dynamic_extent>
    using span = emu::detail::basic_span<ElementType, Extent, default_accessor<ElementType> >;

} // namespace emu
