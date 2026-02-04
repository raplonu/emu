#pragma once

#include <emu/detail/basic_span.hpp>
#include <emu/host/accessor.hpp>

namespace emu
{

    template <typename ElementType, size_t Extent = std::dynamic_extent, typename AccessorPolicy = default_accessor<ElementType> >
    struct span
        : emu::detail::basic_span<
            ElementType,
            Extent,
            host::accessor<AccessorPolicy>,
            span<ElementType, Extent, AccessorPolicy>
        >
    {
        using base = emu::detail::basic_span<
            ElementType,
            Extent,
            host::accessor<AccessorPolicy>,
            span<ElementType, Extent, AccessorPolicy>
        >;

        using base::base;

        constexpr span() noexcept = default;

        friend constexpr void swap(span& x, span& y) noexcept {
            swap(static_cast<base&>(x), static_cast<base&>(y));
        }

        template<typename OT, size_t OExtent>
        constexpr auto actual_from_span(std::span<OT, OExtent> sp) const noexcept {
            return span(sp.data(), sp.size());
        }

    };

    EMU_DEFINE_SPAN_DEDUCTION_GUIDES(span);

} // namespace emu
