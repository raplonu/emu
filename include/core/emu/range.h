#ifndef EMU_RANGE_H
#define EMU_RANGE_H

#include <emu/type_traits.h>
#include <emu/utility.h>

#include <vector>
#include <iostream>

namespace emu
{
    /**
     * @brief Takes a range and return the coresponding vector.
     *
     * By default, the vector will be std::remove_cv_t<range::value_type>
     * but the user is allowed to specify the value_type of vector using
     * the fist template parameter.
     */
    template<typename T = AutoTag, typename R>
    auto as_vector(R && range)
    {
        using value_type = IfNotAutoOr<T, std::remove_cv_t<RangeValueType<R>>>;
        using std::begin; using std::end;
        return std::vector<value_type>(begin(range), end(range));
    }

    /**
     * @brief Takes a c array and return the coresponding vector.
     *
     * By default, the vector will be std::remove_cv_t<R>
     * but the user is allowed to specify the value_type of vector using
     * the fist template parameter.
     */
    template<typename T = AutoTag, typename R, std::size_t N>
    auto as_vector(R (&range)[N])
    {
        using value_type = IfNotAutoOr<T, std::remove_cv_t<R>>;
        using std::begin; using std::end;
        return std::vector<value_type>(begin(range), end(range));
    }

    /**
     * @brief specialization that takes a rvalue reference of required std::vector type.
     *
     * This version allow to avoid copy if the required type correspond to the input type.
     */
    template<typename T>
    auto as_vector(std::vector<T> && range)
    {
        return emu::mv(range);
    }
}

#endif //EMU_RANGE_H