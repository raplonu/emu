#pragma once

#include <emu/concepts.hpp>

#include <fmt/core.h>

#include <iosfwd>
#include <iterator>

//TODO: Rework this when gcc 13 and <format> is available. Use `std::formattable` instead of `cpts::formattable`.

namespace emu
{
    template<cpts::formattable T>
    struct as_fmt { const T& t; };

    template<cpts::formattable T>
    std::ostream& operator<<(std::ostream& o, as_fmt<T> w)
    {
        fmt::format_to(std::ostream_iterator<char>(o), "{}", w.t);
        return o;
    }

} // namespace emu
