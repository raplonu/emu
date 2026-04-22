#pragma once

#include <emu/cufft/type.hpp>

namespace emu
{

namespace cufft
{

namespace detail
{
    template<typename T1, typename T2>
    constexpr type_t type_of() noexcept;

    #define TYPE_OF(T1, T2, Ret)                                                   \
    template<> constexpr type_t type_of<T1, T2>() noexcept { return type_t::Ret; }

    TYPE_OF(float,            cuComplex,         r2c);
    TYPE_OF(cuComplex,        float,             c2r);
    TYPE_OF(cuComplex,        cuComplex,         c2c);
    TYPE_OF(double,           cuDoubleComplex,   d2z);
    TYPE_OF(cuDoubleComplex,  double,            z2d);
    TYPE_OF(cuDoubleComplex,  cuDoubleComplex,   z2z);

    #undef TYPE_OF

} // namespace detail

} // namespace cufft

} // namespace emu
