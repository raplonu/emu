#pragma once

#include <boost/core/demangle.hpp>
#include <half.hpp>

#include <fmt/format.h>

#include <cstdint>

namespace emu
{
    //TODO: find a better place for that.
    using half_float::half;

    template <typename T>
    struct type_name_t
    {};

    /**
     * @brief Generic type to string utility.
     *
     * @tparam T the type we want the name to format.
     */
    template <typename T>
    constexpr type_name_t<T> type_name;

    template<typename T>
    auto format_as(type_name_t<T>) {
        return boost::core::demangle(typeid(T).name());
    }

    template <typename T>
    auto type_name_of() -> std::string {
        return fmt::to_string(type_name<T>);
    }

    template <typename T>
    struct numeric_name_t
    {};

    /**
     * @brief Special type name formatter for numeric type.
     *
     *
     * @tparam T
     */
    template <typename T>
    constexpr numeric_name_t<T> numeric_name;

    template<typename T>
    auto format_as(numeric_name_t<T>);

#define MAP_NUM_NAME(T, name)                                         \
    template<> inline auto format_as<T>(numeric_name_t<T>) { return #name; }

    MAP_NUM_NAME(bool,     bool    )
    MAP_NUM_NAME(int8_t,   int8    )
    MAP_NUM_NAME(int16_t,  int16   )
    MAP_NUM_NAME(int32_t,  int32   )
    MAP_NUM_NAME(int64_t,  int64   )
    MAP_NUM_NAME(uint8_t,  uint8   )
    MAP_NUM_NAME(uint16_t, uint16  )
    MAP_NUM_NAME(uint32_t, uint32  )
    MAP_NUM_NAME(uint64_t, uint64  )
    MAP_NUM_NAME(half,     half    )
    MAP_NUM_NAME(float,    float   )
    MAP_NUM_NAME(double,   double  )

} // namespace emu
