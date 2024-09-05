#pragma once

#include <emu/macro.hpp>
#include <emu/concepts.hpp>
#include <emu/functor.hpp>

#include <cmath>

namespace emu::math
{

namespace std
{

    //Note: being C++, should be using ::std math function but it doesn't work
    // with CUDA for some reasons. Try again later I guess.

    constexpr float       ceil (float       v ) noexcept { return ::ceilf ( v ); }
    constexpr double      ceil (double      v ) noexcept { return ::ceil  ( v ); }
    constexpr long double ceil (long double v ) noexcept { return ::ceill ( v ); }

    constexpr float       floor(float       v ) noexcept { return ::floorf( v ); }
    constexpr double      floor(double      v ) noexcept { return ::floor ( v ); }
    constexpr long double floor(long double v ) noexcept { return ::floorl( v ); }

    constexpr float       trunc(float       v ) noexcept { return ::truncf( v ); }
    constexpr double      trunc(double      v ) noexcept { return ::trunc ( v ); }
    constexpr long double trunc(long double v ) noexcept { return ::truncl( v ); }

    constexpr float       round(float       v ) noexcept { return ::roundf( v ); }
    constexpr double      round(double      v ) noexcept { return ::round ( v ); }
    constexpr long double round(long double v ) noexcept { return ::roundl( v ); }

    constexpr float       fma(float       a, float       b, float       c) noexcept { return ::fmaf(a, b, c); }
    constexpr double      fma(double      a, double      b, double      c) noexcept { return ::fma (a, b, c); }
    constexpr long double fma(long double a, long double b, long double c) noexcept { return ::fmal(a, b, c); }

} // namespace std

    // template<typename To>
    // struct as_t
    // {
    //     template<typename From> constexpr To ceil (From from) const noexcept;
    //     template<typename From> constexpr To floor(From from) const noexcept;
    //     template<typename From> constexpr To trunc(From from) const noexcept;
    //     template<typename From> constexpr To round(From from) const noexcept;
    // };

    template<typename To> struct ceil_t  {
        template<typename From>
        constexpr To operator()(From from) const noexcept { return static_cast<To>(from); }

        constexpr To operator()(To from) const noexcept requires ::std::floating_point<To> { return std::ceil(from); }
    };
    template<typename To> struct floor_t {
        template<typename From>
        constexpr To operator()(From from) const noexcept { return static_cast<To>(from); }

        constexpr To operator()(To from) const noexcept requires ::std::floating_point<To> { return std::floor(from); }
    };
    template<typename To> struct trunc_t {
        template<typename From>
        constexpr To operator()(From from) const noexcept { return static_cast<To>(from); }

        constexpr To operator()(To from) const noexcept requires ::std::floating_point<To> { return std::trunc(from); }
    };
    template<typename To> struct round_t {
        template<typename From>
        constexpr To operator()(From from) const noexcept { return static_cast<To>(from); }

        constexpr To operator()(To from) const noexcept requires ::std::floating_point<To> { return std::round(from); }
    };

#define ADD_CONVERTER(TYPE, FROM, TO, IMPL)     \
    template<> template<> constexpr                     \
    TO TYPE<TO>::operator()<FROM>(FROM from) const noexcept \
    { return IMPL(from); }

#define ADD_CONVERTER_COMPONENT(FROM, TO, CEIL, FLOOR, TRUNC, ROUND) \
    ADD_CONVERTER(ceil_t , FROM, TO, CEIL )                            \
    ADD_CONVERTER(floor_t, FROM, TO, FLOOR)                            \
    ADD_CONVERTER(trunc_t, FROM, TO, TRUNC)                            \
    ADD_CONVERTER(round_t, FROM, TO, ROUND)

#define ADD_CAST_CONVERTER_COMPONENT(FROM, TO)                        \
    ADD_CONVERTER_COMPONENT(FROM, TO, as<TO>, as<TO>, as<TO>, as<TO>)

#define ADD_HOST_CONVERTER_COMPONENT(FROM, TO) \
    ADD_CONVERTER_COMPONENT(FROM, TO,          \
                            math::std::ceil ,  \
                            math::std::floor,  \
                            math::std::trunc,  \
                            math::std::round)

#define ADD_DEVICE_CONVERTER_COMPONENT(FROM, TO, PREFIX, SUFFIX) \
    ADD_CONVERTER_COMPONENT(FROM, TO,                            \
                            __##PREFIX##2##SUFFIX##_ru,          \
                            __##PREFIX##2##SUFFIX##_rd,          \
                            __##PREFIX##2##SUFFIX##_rz,          \
                            __##PREFIX##2##SUFFIX##_rn)

    ADD_CAST_CONVERTER_COMPONENT(float, double);

#if EMU_HOST_CONTEXT

    ADD_HOST_CONVERTER_COMPONENT(double, float             );

    ADD_HOST_CONVERTER_COMPONENT(double, short             );
    ADD_HOST_CONVERTER_COMPONENT(double, int               );
    ADD_HOST_CONVERTER_COMPONENT(double, long              );
    ADD_HOST_CONVERTER_COMPONENT(double, long long         );
    ADD_HOST_CONVERTER_COMPONENT(double, unsigned short    );
    ADD_HOST_CONVERTER_COMPONENT(double, unsigned int      );
    ADD_HOST_CONVERTER_COMPONENT(double, unsigned long     );
    ADD_HOST_CONVERTER_COMPONENT(double, unsigned long long);


    ADD_HOST_CONVERTER_COMPONENT(float , short             );
    ADD_HOST_CONVERTER_COMPONENT(float , int               );
    ADD_HOST_CONVERTER_COMPONENT(float , long              );
    ADD_HOST_CONVERTER_COMPONENT(float , long long         );
    ADD_HOST_CONVERTER_COMPONENT(float , unsigned short    );
    ADD_HOST_CONVERTER_COMPONENT(float , unsigned int      );
    ADD_HOST_CONVERTER_COMPONENT(float , unsigned long     );
    ADD_HOST_CONVERTER_COMPONENT(float , unsigned long long);

    // NOTE : There is no functions to efficiently cast integral type to floating point ones.

    ADD_CAST_CONVERTER_COMPONENT(short             , float );
    ADD_CAST_CONVERTER_COMPONENT(int               , float );
    ADD_CAST_CONVERTER_COMPONENT(long              , float );
    ADD_CAST_CONVERTER_COMPONENT(long long         , float );
    ADD_CAST_CONVERTER_COMPONENT(unsigned short    , float );
    ADD_CAST_CONVERTER_COMPONENT(unsigned int      , float );
    ADD_CAST_CONVERTER_COMPONENT(unsigned long     , float );
    ADD_CAST_CONVERTER_COMPONENT(unsigned long long, float );

    ADD_CAST_CONVERTER_COMPONENT(short             , double);
    ADD_CAST_CONVERTER_COMPONENT(int               , double);
    ADD_CAST_CONVERTER_COMPONENT(long              , double);
    ADD_CAST_CONVERTER_COMPONENT(long long         , double);
    ADD_CAST_CONVERTER_COMPONENT(unsigned short    , double);
    ADD_CAST_CONVERTER_COMPONENT(unsigned int      , double);
    ADD_CAST_CONVERTER_COMPONENT(unsigned long     , double);
    ADD_CAST_CONVERTER_COMPONENT(unsigned long long, double);

#else // DEVICE CONTEXT

    ADD_DEVICE_CONVERTER_COMPONENT(double, float             , double, float);

    ADD_DEVICE_CONVERTER_COMPONENT(double, short             , double, int );
    ADD_DEVICE_CONVERTER_COMPONENT(double, int               , double, int );
    ADD_DEVICE_CONVERTER_COMPONENT(double, long              , double, ll  );
    ADD_DEVICE_CONVERTER_COMPONENT(double, long long         , double, ll  );
    ADD_DEVICE_CONVERTER_COMPONENT(double, unsigned short    , double, uint);
    ADD_DEVICE_CONVERTER_COMPONENT(double, unsigned int      , double, uint);
    ADD_DEVICE_CONVERTER_COMPONENT(double, unsigned long     , double, ull );
    ADD_DEVICE_CONVERTER_COMPONENT(double, unsigned long long, double, ull );

    ADD_DEVICE_CONVERTER_COMPONENT(float , short             , float , int );
    ADD_DEVICE_CONVERTER_COMPONENT(float , int               , float , int );
    ADD_DEVICE_CONVERTER_COMPONENT(float , long              , float , ll  );
    ADD_DEVICE_CONVERTER_COMPONENT(float , long long         , float , ll  );
    ADD_DEVICE_CONVERTER_COMPONENT(float , unsigned short    , float , uint);
    ADD_DEVICE_CONVERTER_COMPONENT(float , unsigned int      , float , uint);
    ADD_DEVICE_CONVERTER_COMPONENT(float , unsigned long     , float , ull );
    ADD_DEVICE_CONVERTER_COMPONENT(float , unsigned long long, float , ull );

    ADD_DEVICE_CONVERTER_COMPONENT(short             , float , int , float );
    ADD_DEVICE_CONVERTER_COMPONENT(int               , float , int , float );
    ADD_DEVICE_CONVERTER_COMPONENT(long              , float , ll  , double);
    ADD_DEVICE_CONVERTER_COMPONENT(long long         , float , ll  , double);
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned short    , float , uint, float );
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned int      , float , uint, float );
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned long     , float , ull , double);
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned long long, float , ull , double);

    ADD_DEVICE_CONVERTER_COMPONENT(short             , double, int , float );
    ADD_DEVICE_CONVERTER_COMPONENT(int               , double, int , float );
    ADD_DEVICE_CONVERTER_COMPONENT(long              , double, ll  , double);
    ADD_DEVICE_CONVERTER_COMPONENT(long long         , double, ll  , double);
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned short    , double, uint, float );
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned int      , double, uint, float );
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned long     , double, ull , double);
    ADD_DEVICE_CONVERTER_COMPONENT(unsigned long long, double, ull , double);

#endif

#undef ADD_CONVERTER
#undef ADD_CONVERTER_COMPONENT
#undef ADD_CAST_CONVERTER_COMPONENT
#undef ADD_HOST_CONVERTER_COMPONENT
#undef ADD_DEVICE_CONVERTER_COMPONENT

    // was EMU_HODE_CONSTEXPR before. Should not be necessary.
    template<typename To> EMU_HODE_CONSTEXPR auto ceil  = ceil_t <To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto floor = floor_t<To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto trunc = trunc_t<To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto round = round_t<To>{};

    struct add_t  {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a + b; }
    };
    struct sub_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a - b; }
    };
    struct mul_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a * b; }
    };
    struct div_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a / b; }
    };
    struct min_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return (a < b) ? a : b; }
    };
    struct max_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return (a > b) ? a : b; }
    };
    struct bor_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a | b; }
    };
    struct band_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a & b; }
    };
    struct bxor_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b) const noexcept { return a ^ b; }
    };
    struct exch_t {
        template<typename T>
        constexpr T operator()(T& a, ::std::type_identity_t<T> b) const noexcept { return ::std::exchange(a, b); }
    };
    struct fma_t {
        template<typename T>
        constexpr T operator()(T a, ::std::type_identity_t<T> b, ::std::type_identity_t<T> c) const noexcept { return math::std::fma(a, b, c); }
    };

    EMU_HODE_CONSTEXPR add_t  add  {};
    EMU_HODE_CONSTEXPR sub_t  sub  {};
    EMU_HODE_CONSTEXPR mul_t  mul  {};
    EMU_HODE_CONSTEXPR div_t  div  {};
    EMU_HODE_CONSTEXPR min_t  min  {};
    EMU_HODE_CONSTEXPR max_t  max  {};
    EMU_HODE_CONSTEXPR bor_t  bor  {};
    EMU_HODE_CONSTEXPR band_t band {};
    EMU_HODE_CONSTEXPR bxor_t bxor {};
    EMU_HODE_CONSTEXPR exch_t exch {};
    EMU_HODE_CONSTEXPR fma_t  fma  {};

} // namespace emu::math
