#pragma once

#include <emu/detail/basic_container.hpp>
#include <emu/host/accessor.hpp>

namespace emu::host
{

    template <typename ElementType, size_t Extent = dynamic_extent, typename AccessorPolicy = default_accessor<ElementType> >
    struct container
        : emu::detail::basic_container<
            ElementType,
            Extent,
            host::accessor<AccessorPolicy>,
            container<ElementType, Extent, AccessorPolicy>
        >
    {
        using base = emu::detail::basic_container<
            ElementType,
            Extent,
            host::accessor<AccessorPolicy>,
            container<ElementType, Extent, AccessorPolicy>
        >;

        using base::base;

        constexpr container() noexcept = default;

        friend constexpr void swap(container& x, container& y) noexcept {
            swap(static_cast<base&>(x), static_cast<base&>(y));
        }

        template<typename OT, size_t OExtent>
        constexpr auto actual_from_base(const std::span<OT, OExtent>& sp) const noexcept {
            return container(sp.data(), sp.size(), capsule());
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES(container);

    template<typename T>
    container<T> make_container(size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace emu::host
