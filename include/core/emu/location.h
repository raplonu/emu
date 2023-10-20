#pragma once

#include <emu/type_traits.h>
#include <emu/concepts.hpp>

#include <memory>

namespace emu
{

namespace location
{

    struct host {
        template<typename T>
        using default_allocator_type = std::allocator<T>;

        template<typename T>
        default_allocator_type<T> make_default_allocator() {
            return default_allocator_type<T>();
        }
    };

} // namespace location

namespace detail
{
    template<typename T>
    struct has_location { using type = location::host; };

    template<cpts::has_location T>
    struct has_location<T> { using type = typename T::location_type; };

} // namespace detail

    template<typename T>
    using location_or_default_type = typename detail::has_location<T>::type;

    template<typename T>
    auto get_location(const T& v) {
        if constexpr (cpts::has_location<T>) {
            return v.location();
        } else {
            return location::host{};
        }
    }

namespace spe
{
    template<>
    struct is_location<location::host> : std::true_type {};

}

} // namespace emu
