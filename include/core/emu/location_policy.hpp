#pragma once

#include <emu/type_traits.hpp>

namespace emu
{

    struct no_location_policy
    {
        template <typename>
        static constexpr bool validate_source = true;

    };

namespace detail
{
    template<typename T>
    struct location_type_of { using type = no_location_policy; };

    template<typename T>
        requires( requires { typename T::location_type; })
    struct location_type_of<T> { using type = typename T::location_type; };

} // namespace detail

    template<typename T>
    using location_type_of = typename detail::location_type_of<T>::type;

} // namespace emu
