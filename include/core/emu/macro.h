#ifndef EMU_MACRO_H
#define EMU_MACRO_H

#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <emu/config.h>

#include <type_traits>

/// Specify that a function may throw or not. Do nothing if noexcept is not available.
#define EMU_NOEXCEPT(cond) noexcept(cond)

/// Specify that a function may throw if the given expression may throw.
/// Do nothing if noexcept is not available.
#define EMU_NOEXCEPT_EXPR(expr) noexcept(noexcept(expr))

#if EMU_CUDA

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
    #define DEVICE_CONTEXT 1
    #define HOST_CONTEXT   0
#else
    #define DEVICE_CONTEXT 0
    #define HOST_CONTEXT   1
#endif

#else

#define DEVICE_CONTEXT     0
#define HOST_CONTEXT       1

#define EMU_HOST
#define EMU_HODE

#define EMU_HODE_CONSTEXPR constexpr

#endif

#if defined __GNUC__ or defined __CUDACC__
#define EMU_COLD_PATH __attribute__((noinline,cold))

#else // compilator is neither gcc nor nvcc
#define EMU_COLD_PATH
#error

#endif

#define EMU_UNREACHABLE __builtin_unreachable();

#define EMU_CONCAT(a, b) EMU_CONCAT_INNER(a, b)
#define EMU_CONCAT_INNER(a, b) a ## b

#define EMU_UNIQUE_NAME(base) EMU_CONCAT(base, __COUNTER__)

/*
 * Generates a trait evaluating to `std::true_type` iff the given expression
 * is valid.
 *
 * The generated trait is a template alias, the underlying trait being in the
 * `detail` namespace.
 *
 * Also, because of the use of `std::true_type` and `std::false_type`,
 * the standard header `<type_traits>` must already be included.
 *
 * Implementation From `https://github.com/aldebaran/libqi/tree/team/platform/dev/ka`
 *
 * Example: Generating a trait testing (for a type parameter `T`) the validity of `*t`
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * #include <type_traits>
 *
 * EMU_GENERATE_TRAITS_HAS(HasOperatorStar, T, *std::declval<T>())
 * static_assert( HasOperatorStar<int*>::value, "");
 * static_assert( HasOperatorStar<std::vector<int>::iterator>::value, "");
 * static_assert(!HasOperatorStar<int>::value, "");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: Useful functions to "produce" value in the expression are:
 * `std::declval`, `declref` and `declcref` (see utility.hpp).
 */
#define EMU_GENERATE_TRAITS_HAS(TRAIT_NAME, TYPE_PARAM, EXPR)   \
namespace detail {                                              \
    struct TRAIT_NAME##Impl {                                   \
    template<typename TYPE_PARAM>                               \
    static std::true_type test(decltype((EXPR), 0));            \
                                                                \
    template<typename TYPE_PARAM>                               \
    static std::false_type test(...);                           \
    };                                                          \
}                                                               \
template<typename T>                                            \
constexpr bool TRAIT_NAME = decltype(detail::TRAIT_NAME##Impl::test<T>(0))::value

#endif //EMU_MACRO_H
