#pragma once

#include <emu/fwd.hpp>
#include <emu/type_traits.h>

#include <fmt/core.h>

#include <concepts>
#include <type_traits>
#include <span>

namespace emu::cpts
{

    template<typename T1, typename T2>
    concept equivalent = std::same_as<std::decay_t<T1>, std::decay_t<T2>>;

    template<typename T1, typename T2>
    concept not_equivalent = not equivalent<T1, T2>;

    template<class T, template<class...> class Template>
    concept specialization_of = is_specialization<std::decay_t<T>, Template>;

    template<typename Derived, typename Base>
    concept not_derived_from = not std::derived_from<std::decay_t<Derived>, Base>;

    template<typename T>
    concept location = is_location<T>;

    template<typename T>
    concept has_location = requires (const T& v) {
        { v.location() } -> location;
    };

    // cannot use is_specialization here because second span argument is not a type.
    template<typename T>
    concept span = std::derived_from<std::decay_t<T>, std::span<typename std::decay_t<T>::element_type, std::decay_t<T>::extent>>;

    template<typename T>
    concept device_span = std::derived_from<std::decay_t<T>, emu::detail::span<typename std::decay_t<T>::element_type, cuda::location::device, std::decay_t<T>::extent>>;

    template<typename T>
    concept mdspan = is_specialization<std::decay_t<T>, std::experimental::mdspan>;

    template<typename T>
    concept formattable = requires (const T& v, fmt::format_context ctx) {
        fmt::formatter<T>().format(v, ctx);
    };

    template<typename P>
    concept const_pointer = std::is_pointer_v<P> and std::is_const_v<std::remove_pointer_t<P>>;

    template<typename R>
    concept c_string = requires(const R r) {
        { r.c_str() } -> const_pointer; // Can't know for sure if it's a char type.
        { r.size() } -> std::convertible_to<std::size_t>;
    };

} // namespace emu::cpts
