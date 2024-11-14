#pragma once

#include <emu/error.hpp>

#include <charconv>
#include <string_view>

namespace emu
{

    template<typename T>
    auto from_chars(std::string_view sv, T& value) -> result<std::string_view> {
        const auto* last = sv.data() + sv.size();
        auto [p, ec] = std::from_chars(sv.data(), last, value);
        EMU_TRUE_OR_RETURN_UN_EC(ec == std::errc(), ec);
        return std::string_view{p, last};

    }

} // namespace emu
