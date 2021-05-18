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

#define EMU_COMMA ,

#define EMU_CONCAT(a, b) EMU_CONCAT_INNER(a, b)
#define EMU_CONCAT_INNER(a, b) a ## b

#define EMU_UNIQUE_NAME(base) EMU_CONCAT(base, __COUNTER__)

#define EMU_SFINAL_CONDITIONAL(TRAIT_NAME, TYPE_PARAM, TEST, IF_TRUE, IF_FALSE) \
namespace detail {                                                              \
    struct TRAIT_NAME##Impl {                                                   \
        template<typename TYPE_PARAM>                                           \
        static IF_TRUE  test(decltype((TEST), 0));                              \
        template<typename TYPE_PARAM>                                           \
        static IF_FALSE test(...);                                              \
    };                                                                          \
}

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
 * EMU_GENERATE_TRAITS_HAS(HasOperatorStar, T, *std::declval<T>());
 * static_assert( HasOperatorStar<int*>::value, "");
 * static_assert( HasOperatorStar<std::vector<int>::iterator>::value, "");
 * static_assert(!HasOperatorStar<int>::value, "");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: Useful functions to "produce" value in the expression are:
 * `std::declval`, `declref` and `declcref` (see utility.hpp).
 */
#define EMU_GENERATE_TRAITS_HAS(TRAIT_NAME, TYPE_PARAM, EXPR)                                   \
EMU_SFINAL_CONDITIONAL(TRAIT_NAME, TYPE_PARAM, EXPR, std::true_type, std::false_type) \
template<typename T>                                                                            \
constexpr bool TRAIT_NAME = decltype(detail::TRAIT_NAME##Impl::test<T>(0))::value

/*
 * Generates a trait evaluating to `TYPE` iff the given `TYPE`
 * is valid, `ELSE` otherwise.
 *
 * The generated trait is a template alias, the underlying trait being in the
 * `detail` namespace.
 *
 * Example: Generating a trait getting (for a type parameter `T`) `T::element_type` or `*t`
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * #include <type_traits>
 *
 * EMU_GENERATE_TRAITS_GET_TYPE_OR(ElementTypeOrStar, T, typename T::value_type, decltype(*std::declval<T>()));
 * static_assert( std::is_same_v<ElementTypeOrStar<int*>, int&>, "");
 * static_assert( std::is_same_v<HasOperatorStar<std::unique_ptr<int>>, int>, "");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: Useful functions to "produce" value in the expression are:
 * `std::declval`, `declref` and `declcref` (see utility.hpp).
 */
#define EMU_GENERATE_TRAITS_GET_TYPE_OR(TRAIT_NAME, TYPE_PARAM, TYPE, ELSE) \
EMU_SFINAL_CONDITIONAL(TRAIT_NAME, TYPE_PARAM, true, TYPE, ELSE)            \
template<typename T>                                                        \
using TRAIT_NAME = decltype(detail::TRAIT_NAME##Impl::test<T>(0))

/*
 * Generates a trait evaluating to `VALUE` iff the given `VALUE`
 * is valid, `ELSE` otherwize.
 *
 * The generated trait is a template alias, the underlying trait being in the
 * `detail` namespace.
 *
 * Also, because of the use of `integral_constant`,
 * the standard header `<type_traits>` must already be included.
 *
 * Example: Generating a trait getting (for a type parameter `T`) `T::value` or `-1`
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * #include <type_traits>
 *
 * EMU_GENERATE_TRAITS_GET_VALUE_OR(ValueOrMinus1, T, T::value, -1);
 * static_assert( ValueOrMinus1<std::true_type> == true, "");
 * static_assert( ValueOrMinus1<int*> == -1, "");
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: Useful functions to "produce" value in the expression are:
 * `std::declval`, `declref` and `declcref` (see utility.hpp).
 */
#define EMU_GENERATE_TRAITS_GET_VALUE_OR(TRAIT_NAME, TYPE_PARAM, VALUE, ELSE)     \
EMU_SFINAL_CONDITIONAL(TRAIT_NAME, TYPE_PARAM, VALUE,                             \
    std::integral_constant<decltype(VALUE) EMU_COMMA VALUE>,                      \
    std::integral_constant<decltype(ELSE ) EMU_COMMA ELSE >)                      \
template<typename T>                                                              \
constexpr auto TRAIT_NAME = decltype(detail::TRAIT_NAME##Impl::test<T>(0))::value

/**
 * @brief Return false if condition is false, continue otherwise.
 *
 */
#define EMU_TRUE_OR_RETURN_FALSE( expr__ ) if (!(expr__)) return false


#endif //EMU_MACRO_H
