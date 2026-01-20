#pragma once

#include <emu/detail/basic_container.hpp>

namespace emu
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType> >
    using container = emu::detail::basic_container<ElementType, Extent, AccessorPolicy>;

    template<typename T>
    container<T> make_container(size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace emu
