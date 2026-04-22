#pragma once

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

    /**
     * @brief Unimplemented type to produce a compile-time error.
     *
     * This type is used to prduce a compile-time error in order to
     * display a message with the template parameter name.
     *
     * To produce the error, simply instantiate this type with the template
     * parameter you want to inspect.
     *
     */
    template<typename T>
    struct Error;

} // namespace emu
