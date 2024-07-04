#pragma once

#include <emu/type_traits.hpp>
#include <emu/utility.hpp>

#include <vector>
#include <iostream>

namespace emu
{
    /**
     * @brief Takes a range and return the coresponding vector.
     *
     * By default, the vector will be RemoveCVRef<range::value_type>
     * but the user is allowed to specify the value_type of vector using
     * the fist template parameter.
     */
    template<typename T = use_default, typename R>
    constexpr auto as_vector(R && range)
    {
        using value_type = not_default_or<T, std::ranges::range_value_t<std::remove_cvref_t<R>>>;
        using std::ranges::begin; using std::ranges::end;
        return std::vector<value_type>(begin(range), end(range));
    }

    /**
     * @brief Takes a c array and return the coresponding vector.
     *
     * By default, the vector will be RemoveCVRef<R>
     * but the user is allowed to specify the value_type of vector using
     * the fist template parameter.
     */
    template<typename T = use_default, typename R, std::size_t N>
    constexpr auto as_vector(R (&range)[N])
    {
        using value_type = not_default_or<T, std::remove_cvref_t<R>>;
        using std::begin; using std::end;
        return std::vector<value_type>(begin(range), end(range));
    }

    /**
     * @brief specialization that takes a rvalue reference of required std::vector type.
     *
     * This version allow to avoid copy if the required type correspond to the input type.
     */
    template<typename T>
    constexpr auto as_vector(std::vector<T> && range) noexcept
    {
        return move(range);
    }
}
