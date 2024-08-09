#pragma once

#include <emu/detail/basic_container.hpp>
#include <emu/mdcontainer.hpp>
#include <emu/location_policy.hpp>

namespace emu
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct container : emu::detail::basic_container<ElementType, Extent, no_location_policy, container<ElementType, Extent>>
    {
        using base = emu::detail::basic_container<ElementType, Extent, no_location_policy, container>;

        using base::base;

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T> make_container(std::size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

    template<typename T, std::size_t Extent>
    constexpr auto as_md(container<T, Extent> s) noexcept {
        return mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
    }

} // namespace emu
