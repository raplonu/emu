#pragma once

#include <emu/config.hpp>

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <gsl/assert>

/// Specify noinline attribute regardless of compiler.
/// gcc allow cold path attribute.
/// See https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
#if EMU_GCC or EMU_CUDACC

#define EMU_NOINLINE __attribute__((__noinline__,cold))

#else // compiler is neither gcc nor nvcc.

#define EMU_NOINLINE BOOST_NOINLINE

#endif

#define EMU_FORCEINLINE BOOST_FORCEINLINE

#define EMU_ASSUME(cond) GSL_ASSUME(cond)

#define EMU_LIKELY(cond)   GSL_LIKELY(cond)
#define EMU_UNLIKELY(cond) GSL_UNLIKELY(cond)

/// Specify that a function may throw or not.
#define EMU_NOEXCEPT(cond) noexcept(cond)

/// Specify that a function may throw if the given expression may throw.
#define EMU_NOEXCEPT_EXPR(...) noexcept(noexcept(__VA_ARGS__))

#define EMU_FWD(v) ::std::forward<decltype(v)>(v)

#if EMU_CUDACC

// EMU_GLOB and EMU_DEVICE are defined in `emu/macro.cuh`.
#define EMU_HOST __host__
#define EMU_HODE __host__ __device__

/**
 * @brief Qualifier to declare host device template constexpr global variable.
 *
 * CUDA compiler refuses to give access to template constexpr global variable from device space unlike
 * non template constexpr global variable. Values need __device__ qualifier. However, __device__ constexpr variable
 * are still accessible from host space.
 *
 * N/A for non template values.
 *
 */
#define EMU_HODE_CONSTEXPR __device__ constexpr

#ifdef __CUDA_ARCH__
    #define EMU_DEVICE_CONTEXT 1
    #define EMU_HOST_CONTEXT   0
#else
    #define EMU_DEVICE_CONTEXT 0
    #define EMU_HOST_CONTEXT   1
#endif

#else

#define EMU_DEVICE_CONTEXT     0
#define EMU_HOST_CONTEXT       1

#define EMU_HOST
#define EMU_HODE

#define EMU_HODE_CONSTEXPR constexpr

#endif

#define EMU_STR(str) #str

#define EMU_COMMA ,

#define EMU_CONCAT(a, b)             BOOST_PP_CAT(a, b)
#define EMU_CONCAT_2(a, b)          BOOST_PP_CAT(a, b)
#define EMU_CONCAT_3(a, b, c)       BOOST_PP_CAT(a, EMU_CONCAT_2(b, c))
#define EMU_CONCAT_4(a, b, c, d)    BOOST_PP_CAT(a, EMU_CONCAT_3(b, c, d))
#define EMU_CONCAT_5(a, b, c, d, e) BOOST_PP_CAT(a, EMU_CONCAT_4(b, c, d, e))

#define EMU_UNIQUE_NAME(base) EMU_CONCAT(base, __COUNTER__)


// #################
// # Logging macro #
// #################

/// `EMU_LOGGER` is a macro that takes a format string and zero or more arguments
/// By default, it will display a message like: "<current_file>:<current_line>: <msg>\n"
/// using libfmt
/// You can change the behavior by defining `EMU_LOGGER` before the first include.
/// You can also disable the logger by defining `EMU_NO_LOG`.

#ifndef EMU_LOGGER

#ifndef EMU_NO_LOG

#include <fmt/color.h>

#define EMU_LOGGER(...)                                                            \
    ::fmt::print(stderr, fmt::fg(fmt::color::red), "{}:{}: ", __FILE__, __LINE__); \
    ::fmt::print(stderr, fmt::fg(fmt::color::red), __VA_ARGS__);                   \
    ::fmt::print(stderr, "\n");

#else // ifndef EMU_NO_LOG

#define EMU_LOGGER(...)

#endif // ifndef EMU_NO_LOG

#endif // ifndef EMU_LOGGER

/// EMU_COLD_LOGGER will try to put the logging code away from the hot execution path.
#define EMU_COLD_LOGGER(...) [&] () EMU_NOINLINE { EMU_LOGGER(__VA_ARGS__); }()

// ################
// # truthy macro #
// ################

/// For implicit bool convertible types. Return value if it is truthy, continue otherwise.
#define EMU_RETURN_IF_TRUE( ... ) \
    if (auto&& value__ = __VA_ARGS__ ; value__) { return ::std::move(value__); }

/// For implicit bool convertible types. Return dereferenced value if it is truthy, continue otherwise.
#define EMU_UNWRAP_RETURN_IF_TRUE( ... ) \
    if (auto&& value__ = __VA_ARGS__ ; value__) { return *::std::move(value__); }

// #############################
// # unsuccessful return macro #
// #############################

#define EMU_TRUE_OR_RETURN( maybe, else_value) if (EMU_UNLIKELY( not(maybe) )) return else_value

/// Return false if condition is false, continue otherwise.
#define EMU_TRUE_OR_RETURN_FALSE( maybe ) EMU_TRUE_OR_RETURN(maybe, false)

/// Return nullopt if condition is false, continue otherwise.
#define EMU_TRUE_OR_RETURN_NULLOPT( maybe ) EMU_TRUE_OR_RETURN(maybe, ::emu::nullopt)

/// Return unexpected if condition is false, continue otherwise.
#define EMU_TRUE_OR_RETURN_UNEXPECTED( maybe, unex ) EMU_TRUE_OR_RETURN(maybe, ::emu::unexpected(unex))

/// Return expected error value if it exists, continue otherwise.
#define EMU_TRUE_OR_RETURN_ERROR( expected ) \
    EMU_TRUE_OR_RETURN(expected, ::emu::unexpected(::std::move(expected).error()))

// ################
// # assign macro #
// ################

/// Take a truthy dereferenceable type. If true, dereference,
/// else return the error type associated. (default is false)
/// for optional types, will return emu::nullopt
/// for expected<T, E>, will return the error E
#define EMU_UNWRAP(maybe)                                                   \
    ({                                                                      \
        auto&& value__ = (maybe);                                           \
        if (EMU_UNLIKELY(not value__)) return ::emu::unwrap_error(value__); \
        *EMU_FWD(value__);                                                  \
    })

/// Assign value with dereference if truthy or return nullopt
#define EMU_UNWRAP_OR_RETURN_FALSE(maybe)             \
    ({                                                \
        auto&& value__ = (maybe);                     \
        EMU_TRUE_OR_RETURN_FALSE(value__);            \
        *EMU_FWD(value__);                            \
    })

/// Assign value with dereference if truthy or return nullopt
#define EMU_UNWRAP_OR_RETURN_NULLOPT(maybe)          \
    ({                                               \
        auto&& value__ = (maybe);                    \
        EMU_TRUE_OR_RETURN_NULLOPT(value__);         \
        *EMU_FWD(value__);                           \
    })

/// Assign value with dereference if truthy or return unexpected
#define EMU_UNWRAP_OR_RETURN_UNEXPECTED(maybe, ... )         \
    ({                                                       \
        auto&& value__ = (maybe);                            \
        EMU_TRUE_OR_RETURN_UNEXPECTED(value__, __VA_ARGS__); \
        *EMU_FWD(value__);                                   \
    })

/// Assign value with dereference if truthy or return error
#define EMU_UNWRAP_OR_RETURN_ERROR(expected )        \
    ({                                               \
        auto&& value__ = (expected);                 \
        EMU_TRUE_OR_RETURN_ERROR(value__);           \
        *EMU_FWD(value__);                           \
    })
