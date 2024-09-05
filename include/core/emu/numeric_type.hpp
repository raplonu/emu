#pragma once

#include <cstdint>
#include <half.hpp>
#include <emu/detail/dlpack_types.hpp>
#include <complex>

namespace emu
{

    // Expose half_float::half in emu namespace
    // TODO: replace by std::float16_t when C++23 is available.
    using half_float::half;

    enum class data_type_t {
        boolean,
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
        complex32,
        complex64,
        complex128,
        complex256,
        bfloat16
    };

    namespace detail
    {
        template<typename T>
        struct dlpack_supported : std::false_type {};

        template <typename TypeIn>
        constexpr data_type_t data_type_impl();

        template <typename TypeIn>
        constexpr dlpack::data_type_t dl_data_type_impl();

        #define MAP_TYPE(type, value, dl_code, dl_lanes)                                                        \
        template <>                                                                                             \
        struct dlpack_supported<type> : std::true_type {};                                                      \
        template <>                                                                                             \
        constexpr data_type_t data_type_impl<type>() {                                                          \
            return value;                                                                                       \
        }                                                                                                       \
        template <>                                                                                             \
        constexpr dlpack::data_type_t dl_data_type_impl<type>() {                                               \
            return {.code = static_cast<uint8_t>(dl_code), .bits = sizeof(type) * CHAR_BIT, .lanes = dl_lanes}; \
        }

        MAP_TYPE(bool,        data_type_t::boolean,  dlpack::data_type_code_t::kDLBool,  1)
        MAP_TYPE(int8_t,      data_type_t::int8,     dlpack::data_type_code_t::kDLInt,   1)
        MAP_TYPE(int16_t,     data_type_t::int16,    dlpack::data_type_code_t::kDLInt,   1)
        MAP_TYPE(int32_t,     data_type_t::int32,    dlpack::data_type_code_t::kDLInt,   1)
        MAP_TYPE(int64_t,     data_type_t::int64,    dlpack::data_type_code_t::kDLInt,   1)
        MAP_TYPE(uint8_t,     data_type_t::uint8,    dlpack::data_type_code_t::kDLUInt,  1)
        MAP_TYPE(uint16_t,    data_type_t::uint16,   dlpack::data_type_code_t::kDLUInt,  1)
        MAP_TYPE(uint32_t,    data_type_t::uint32,   dlpack::data_type_code_t::kDLUInt,  1)
        MAP_TYPE(uint64_t,    data_type_t::uint64,   dlpack::data_type_code_t::kDLUInt,  1)
        MAP_TYPE(half,        data_type_t::float16,  dlpack::data_type_code_t::kDLFloat, 1)
        MAP_TYPE(float,       data_type_t::float32,  dlpack::data_type_code_t::kDLFloat, 1)
        MAP_TYPE(double,      data_type_t::float64,  dlpack::data_type_code_t::kDLFloat, 1)
        MAP_TYPE(long double, data_type_t::float128, dlpack::data_type_code_t::kDLFloat, 1)

        //NOTE: lanes are for vectorized types. Complex types are still scalar.
        MAP_TYPE(std::complex<half>,        data_type_t::complex32,  dlpack::data_type_code_t::kDLComplex, 1)
        MAP_TYPE(std::complex<float>,       data_type_t::complex64,  dlpack::data_type_code_t::kDLComplex, 1)
        MAP_TYPE(std::complex<double>,      data_type_t::complex128, dlpack::data_type_code_t::kDLComplex, 1)
        // MAP_TYPE(std::complex<long double>, data_type_t::complex256, dlpack::data_type_code_t::kDLComplex, 1)
    }

    template<typename T>
    constexpr data_type_t data_type = detail::data_type_impl<T>();

namespace dlpack
{

    template<typename T>
    constexpr dlpack::data_type_t data_type = detail::dl_data_type_impl<T>();

} // namespace dlpack

namespace cpts
{

    template<typename T>
    concept dlpack_supported_data_type = detail::dlpack_supported<T>::value;

} // namespace cpts


} // namespace emu
