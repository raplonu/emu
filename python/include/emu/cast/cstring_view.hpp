#pragma once

#include <emu/cstring_view.hpp>
#include <emu/assert.h>

#include <pybind11/cast.h>

#include <type_traits>

namespace pybind11::detail
{

    /**
     * @brief Specialization of `type_caster` for `emu::basic_cstring_view`.
     *
     * Specify how to convert from Python str like object to and from `emu::basic_cstring_view`.
     * It is safe to cast to `emu::basic_cstring_view` because `PyUnicode_AsUTF8AndSize` returns
     * a null-terminated string.
     *
     */
    template <typename CharT, class Traits>
        requires (is_std_char_type<CharT>::value)
    struct type_caster<emu::basic_cstring_view<CharT, Traits>>
        : string_caster<std::basic_string_view<CharT, Traits>, true>
    {
        using value_type = emu::basic_cstring_view<CharT, Traits>;
        using base = string_caster<std::basic_string_view<CharT, Traits>, true>;

        PYBIND11_TYPE_CASTER(value_type, const_name(PYBIND11_STRING_NAME));


        bool load(handle src, bool) {
            auto res = base::load(src, true);
            if (res) {
                // string_caster relies on `PyUnicode_AsUTF8AndSize` which returns a null-terminated string.
                // casting from string_view to cstring_view is safe.
                value = value_type(emu::null_terminated, base::value.data(), base::value.size());
                EMU_ASSERT_MSG(value[value.size()] == '\0', "string_caster returned a non null-terminated string");
            }
            return res;
        }

        static handle cast(value_type src, return_value_policy policy, handle h) {
            return base::cast(src, policy, h);
        }

    };

} // namespace pybind11::detail
