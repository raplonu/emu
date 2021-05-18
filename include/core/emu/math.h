#ifndef EMU_MATH_H
#define EMU_MATH_H

#include <emu/macro.h>
#include <emu/utility.h>

#include <cmath>

namespace emu
{

namespace math
{

namespace std
{

    constexpr EMU_HODE float       ceil (float       v ) noexcept { return ::ceilf ( v ); }
    constexpr EMU_HODE double      ceil (double      v ) noexcept { return ::ceil  ( v ); }
    constexpr EMU_HODE long double ceil (long double v ) noexcept { return ::ceill ( v ); }

    constexpr EMU_HODE float       floor(float       v ) noexcept { return ::floorf( v ); }
    constexpr EMU_HODE double      floor(double      v ) noexcept { return ::floor ( v ); }
    constexpr EMU_HODE long double floor(long double v ) noexcept { return ::floorl( v ); }

    constexpr EMU_HODE float       trunc(float       v ) noexcept { return ::truncf( v ); }
    constexpr EMU_HODE double      trunc(double      v ) noexcept { return ::trunc ( v ); }
    constexpr EMU_HODE long double trunc(long double v ) noexcept { return ::truncl( v ); }

    constexpr EMU_HODE float       round(float       v ) noexcept { return ::roundf( v ); }
    constexpr EMU_HODE double      round(double      v ) noexcept { return ::round ( v ); }
    constexpr EMU_HODE long double round(long double v ) noexcept { return ::roundl( v ); }

} // namespace std

namespace detail
{

    template<typename T1, typename T2>
    using SameTypes      = EnableIf<    Equivalent<T1, T2>::value or      (IsIntegral<T1> and IsIntegral<T2>), T1>;

    template<typename T1, typename T2>
    using DifferentTypes = EnableIf<not Equivalent<T1, T2>::value and not (IsIntegral<T1> and IsIntegral<T2>), T1>;

} // namespace detail

    template<typename To>
    struct as_t : emu::detail::as_t<To> {
        template<typename From> EMU_HODE constexpr detail::SameTypes     <To, From> ceil (From from) const noexcept { return as<To>(from); }
        template<typename From> EMU_HODE constexpr detail::SameTypes     <To, From> floor(From from) const noexcept { return as<To>(from); }
        template<typename From> EMU_HODE constexpr detail::SameTypes     <To, From> trunc(From from) const noexcept { return as<To>(from); }
        template<typename From> EMU_HODE constexpr detail::SameTypes     <To, From> round(From from) const noexcept { return as<To>(from); }

        template<typename From> EMU_HODE constexpr detail::DifferentTypes<To, From> ceil (From from) const noexcept;
        template<typename From> EMU_HODE constexpr detail::DifferentTypes<To, From> floor(From from) const noexcept;
        template<typename From> EMU_HODE constexpr detail::DifferentTypes<To, From> trunc(From from) const noexcept;
        template<typename From> EMU_HODE constexpr detail::DifferentTypes<To, From> round(From from) const noexcept;
    };

#define ADD_CONVERTER(FROM, TO, METHOD, IMPL)                                   \
    template<> template<> constexpr                                             \
    detail::DifferentTypes<TO, FROM> as_t<TO>::METHOD(FROM from) const noexcept \
    { return IMPL(from); }

#define ADD_CONVERTER_COMPONENT(FROM, TO, CEIL, FLOOR, TRUNC, ROUND) \
    ADD_CONVERTER(FROM, TO, ceil , CEIL )                            \
    ADD_CONVERTER(FROM, TO, floor, FLOOR)                            \
    ADD_CONVERTER(FROM, TO, trunc, TRUNC)                            \
    ADD_CONVERTER(FROM, TO, round, ROUND)

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

#if HOST_CONTEXT

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

    template<typename To> EMU_HODE_CONSTEXPR auto as = as_t<To>{};

    template<typename To> struct ceil_t  {
        template<typename From>
        constexpr EMU_HODE To operator()(From from) const noexcept { return as<To>.ceil (from); }
    };
    template<typename To> struct floor_t {
        template<typename From>
        constexpr EMU_HODE To operator()(From from) const noexcept { return as<To>.floor(from); }
    };
    template<typename To> struct trunc_t {
        template<typename From>
        constexpr EMU_HODE To operator()(From from) const noexcept { return as<To>.trunc(from); }
    };
    template<typename To> struct round_t {
        template<typename From>
        constexpr EMU_HODE To operator()(From from) const noexcept { return as<To>.round(from); }
    };

    template<typename To> EMU_HODE_CONSTEXPR auto ceil  = ceil_t <To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto floor = floor_t<To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto trunc = trunc_t<To>{};
    template<typename To> EMU_HODE_CONSTEXPR auto round = round_t<To>{};

    struct add_t  {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a + b; }
    };
    struct sub_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a - b; }
    };
    struct mul_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a * b; }
    };
    struct div_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a / b; }
    };
    struct min_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return (a < b) ? a : b; }
    };
    struct max_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return (a > b) ? a : b; }
    };
    struct bor_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a | b; }
    };
    struct band_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a & b; }
    };
    struct bxor_t {
        template<typename T>
        constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a ^ b; }
    };
    // struct exch_t {
    //     template<typename T>
    //     constexpr EMU_HODE T operator()(T a, Identity<T> b) const noexcept { return a + b; }
    // };


    constexpr auto add  = add_t {};
    constexpr auto sub  = sub_t {};
    constexpr auto mul  = mul_t {};
    constexpr auto div  = div_t {};
    constexpr auto min  = min_t {};
    constexpr auto max  = max_t {};
    constexpr auto bor  = bor_t {};
    constexpr auto band = band_t{};
    constexpr auto bxor = bxor_t{};

} // namespace math


} // namespace emu

#endif //EMU_MATH_H