#pragma once

#include <emu/concepts.hpp>

#include <fmt/core.h>

#include <iosfwd>

namespace emu
{

    template<cpts::formattable T>
    std::ostream& operator<<(std::ostream& o, const T& t) {
        fmt::format_to(std::ostream_iterator<char>(o), "{}", t);
        return o;
    }

} // emu
