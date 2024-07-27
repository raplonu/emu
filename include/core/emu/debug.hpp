#pragma once

#warning "This header is for debug only, do not include it in production code"

namespace emu
{
    template<auto V>
    [[nodiscard]] inline constexpr int warn() {
        return 0;
    }

    template<class T>
    [[nodiscard]] inline constexpr int warn() {
        return 0;
    }

    template<class T>
    [[nodiscard]] inline constexpr int warn(T&&) {
        return 0;
    }

} // namespace emu
