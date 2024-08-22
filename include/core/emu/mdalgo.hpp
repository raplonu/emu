#pragma once

#include "type_traits.hpp"
#include <emu/type_traits.hpp>
#include <emu/utility.hpp>

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

    template <typename T>
    struct md_converter {
        static constexpr auto convert(T&& t) noexcept {
            static_assert(dependent_false<T>, "No md_converter specialization for this type");
        }
    };

} // namespace spe

    template<typename T>
    constexpr auto rank = spe::rank<T>::value;

    template <typename T>
    constexpr auto as_md(T&& t) noexcept {
        return spe::md_converter<rm_cvref<T>>::convert(EMU_FWD(t));
    }

    template <typename T>
    using md_equivalent = decltype(as_md(std::declval<T>()));

} // namespace emu
