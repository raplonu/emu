#pragma once

#include <boost/core/demangle.hpp>

#include <fmt/format.h>

namespace emu
{

    template <typename T>
    struct type_name_t
    {};

    template<typename T>
    auto format_as(type_name_t<T>) {
        return boost::core::demangle(typeid(T).name());
    }

    template <typename T>
    constexpr type_name_t<T> type_name;

} // namespace emu

