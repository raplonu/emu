#ifndef EMU_NUMERIC_TYPE_H
#define EMU_NUMERIC_TYPE_H

#include <cstdint>
#include <half.hpp>

namespace emu
{
    // Expose half_float::half in emu namespace
    using half_float::half;

    enum class DataType {
        uint8,
        uint16,
        uint32,
        uint64,
        int8,
        int16,
        int32,
        int64,
        float16,
        float32,
        float64,
        float128,
        complex64,
        complex128
    };

    namespace detail
    {
        template <typename TypeIn>
        constexpr DataType data_type_impl();

        #define MAP_TYPE(type, value)               \
        template <>                                 \
        constexpr DataType data_type_impl<type>() { \
            return value;                           \
        }

        MAP_TYPE(int8_t,  DataType::int8)
        MAP_TYPE(int16_t, DataType::int16)
        MAP_TYPE(int32_t, DataType::int32)
        MAP_TYPE(int64_t, DataType::int64)

        MAP_TYPE(uint8_t,  DataType::uint8)
        MAP_TYPE(uint16_t, DataType::uint16)
        MAP_TYPE(uint32_t, DataType::uint32)
        MAP_TYPE(uint64_t, DataType::uint64)

        MAP_TYPE(half, DataType::float16)
        MAP_TYPE(float,            DataType::float32)
        MAP_TYPE(double,           DataType::float64)
    }

    template<typename T>
    constexpr DataType data_type = detail::data_type_impl<T>();

}

#endif //EMU_NUMERIC_TYPE_H