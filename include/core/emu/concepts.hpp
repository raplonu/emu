#pragma once

#include <emu/fwd.hpp>
#include <emu/type_traits.hpp>

#include <fmt/core.h>

#include <concepts>
#include <type_traits>
#include <span>

namespace emu
{

namespace cpts
{

    template<typename T1, typename T2>
    concept equivalent = std::same_as<decay<T1>, decay<T2>>;

    template<typename T1, typename T2>
    concept not_equivalent = not equivalent<T1, T2>;

    template<class T, template<class...> class Template>
    concept specialization_of = is_specialization<T, Template>;

    template<typename Derived, typename Base>
    concept not_derived_from = not std::derived_from<Derived, Base>;

    // cannot use is_specialization here because second span argument is not a type.
    template<typename T>
    concept span = std::same_as<T, std::span<typename T::element_type, T::extent>>;

    template<typename T>
    concept mdspan = specialization_of<T, std::experimental::mdspan>;

    template<typename T>
    concept view = span<T> or mdspan<T>;

    // template<typename T>
    // concept array = std::same_as<T, std::array<typename T::value_type, std::tuple_size_v<T>>>;

namespace detail
{

    template<typename T>
    struct is_std_array : std::false_type {};

    template<typename T, std::size_t N>
    struct is_std_array<std::array<T, N>> : std::true_type {};

} // namespace detail

    template<typename T>
    concept array = detail::is_std_array<decay<T>>::value;

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

    template<typename T>
    concept contiguous_sized_range
        =   std::ranges::contiguous_range<T>
        and std::ranges::sized_range<T>;

} // namespace cpts

namespace cuda::cpts
{

    template<typename T>
    concept span = std::same_as<decay<T>, span<typename decay<T>::element_type, decay<T>::extent>>;

    template<typename T>
    concept mdspan = emu::cpts::specialization_of<T, mdspan>;

} // namespace cuda::cpts


} // namespace emu
