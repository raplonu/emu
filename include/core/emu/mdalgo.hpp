#pragma once

#include <type_traits>
#include <ranges>

namespace emu
{

namespace spe
{

    template<typename T>
    struct rank : std::integral_constant<std::size_t, 0> {};

    template<std::ranges::contiguous_range R>
    struct rank<R> : std::integral_constant<std::size_t, 1> {};

    // template <typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    // struct rank<stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>> : std::integral_constant<std::size_t, stdex::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>::rank()> {};

} // namespace spe

    template<typename T>
    constexpr auto rank = spe::rank<T>::value;

} // namespace emu
