#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_container.hpp>
#include <emu/detail/basic_mdspan.hpp>
#include <emu/host/location_policy.hpp>

namespace emu
{
namespace host
{

    template <typename ElementType, size_t Extent = dynamic_extent>
    struct container : emu::detail::basic_container<ElementType, Extent, host::source_policy, container<ElementType, Extent> >
    {
        using base = emu::detail::basic_container<ElementType, Extent, host::source_policy, container>;

        using base::base;

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T> make_container(size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace host

    template<typename T, std::size_t Extent>
    constexpr auto as_md(host::container<T, Extent> s) noexcept {
        return host::mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
    }

} // namespace emu
