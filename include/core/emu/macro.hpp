#pragma once

#include <emu/config.hpp>

#include <boost/preprocessor/cat.hpp>

#include <type_traits>

/// Specify noinline attribute regardless of compiler.
/// gcc allow cold path attribute.
/// See https://gcc.gnu.org/onlinedocs/gcc/Common-Function-Attributes.html#Common-Function-Attributes
#if EMU_GCC or EMU_CUDACC
#define EMU_NOINLINE __attribute__((__noinline__,cold))
#else // compilator is neither gcc nor nvcc.
#define EMU_NOINLINE BOOST_NOINLINE
#endif

#define EMU_FORCEINLINE BOOST_FORCEINLINE

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

#define EMU_COMMA ,

#define EMU_CONCAT(a, b) BOOST_PP_CAT(a, b)
#define EMU_CONCAT_2(a, b)          BOOST_PP_CAT(a, b)
#define EMU_CONCAT_3(a, b, c)       BOOST_PP_CAT(a, EMU_CONCAT_2(b, c))
#define EMU_CONCAT_4(a, b, c, d)    BOOST_PP_CAT(a, EMU_CONCAT_3(b, c, d))
#define EMU_CONCAT_5(a, b, c, d, e) BOOST_PP_CAT(a, EMU_CONCAT_4(b, c, d, e))

#define EMU_UNIQUE_NAME(base) EMU_CONCAT(base, __COUNTER__)

/**
 * @brief Return false if condition is false, continue otherwise.
 *
 */
#define EMU_TRUE_OR_RETURN_FALSE( expr__... ) if (!(expr__)) return false
#define EMU_TRUE_OR_RETURN_NULLOPT( expr__... ) if (!(expr__)) return emu::nullopt
// for EMU_TRUE_OR_RETURN_UNEXPECTED, see emu/expected.hpp