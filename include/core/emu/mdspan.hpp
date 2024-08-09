#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/type_name.hpp>
#include <emu/detail/basic_mdspan.hpp>

namespace emu
{

    using stdex::mdspan;

    EMU_DEFINE_MDSPAN_ALIAS

    template <typename T>
    constexpr auto as_md(T& t) noexcept {
        return mdspan_0d<T>{&t};
    }

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    constexpr auto as_md(mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> md) noexcept {
        return md;
    }

    template <typename T>
    using md_equivalent = decltype(as_md(std::declval<T>()));

} // namespace emu
