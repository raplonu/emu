#pragma once

/**
 * @file assert.hpp
 * @brief Assertion macros for the emu library.
 *
 * This file provides assertion macros that wrap Boost.Assert functionality
 * with additional compiler-specific optimizations and contract checking.
 */

#include <emu/config.hpp>
#include <emu/macro.hpp>

#include <boost/assert.hpp>

/**
 * @def EMU_ASSERT(cond)
 * @brief Assert that a condition is true.
 *
 * This macro asserts that the given condition is true. In debug builds,
 * if the condition is false, the program will terminate with an error message.
 * In release builds, this macro may be compiled out for performance.
 *
 * @param cond The condition to check
 *
 * @par Example:
 * @code
 * void divide(int a, int b) {
 *     EMU_ASSERT(b != 0);  // Assert that divisor is not zero
 *     return a / b;
 * }
 * @endcode
 */
#define EMU_ASSERT(cond) BOOST_ASSERT(cond)
/**
 * @def EMU_ASSERT_MSG(cond, msg)
 * @brief Assert that a condition is true with a custom message.
 *
 * This macro asserts that the given condition is true and provides a custom
 * error message if the assertion fails. This is useful for providing more
 * descriptive error information.
 *
 * @param cond The condition to check
 * @param msg The error message to display if the assertion fails
 *
 * @par Example:
 * @code
 * void allocate_memory(size_t size) {
 *     EMU_ASSERT_MSG(size > 0, "Memory size must be positive");
 *     EMU_ASSERT_MSG(size < MAX_ALLOC, "Memory size exceeds maximum allocation");
 *     // ... allocation code
 * }
 * @endcode
 */
#define EMU_ASSERT_MSG(cond, msg) BOOST_ASSERT_MSG(cond, msg)

/**
 * @def EMU_VERIFY(cond)
 * @brief Verify that a condition is true.
 *
 * This macro verifies that the given condition is true. Unlike EMU_ASSERT,
 * EMU_VERIFY is never compiled out, even in release builds. The condition
 * is always evaluated and checked.
 *
 * @param cond The condition to verify
 *
 * @par Example:
 * @code
 * bool initialize_system() {
 *     bool success = init_subsystem();
 *     EMU_VERIFY(success);  // Always check initialization succeeded
 *     return success;
 * }
 * @endcode
 */
#define EMU_VERIFY(cond) BOOST_VERIFY(cond)

/**
 * @def EMU_VERIFY_MSG(cond, msg)
 * @brief Verify that a condition is true with a custom message.
 *
 * This macro verifies that the given condition is true and provides a custom
 * error message if verification fails. The verification is never compiled out.
 *
 * @param cond The condition to verify
 * @param msg The error message to display if verification fails
 *
 * @par Example:
 * @code
 * void critical_operation() {
 *     bool result = perform_critical_task();
 *     EMU_VERIFY_MSG(result, "Critical task failed - system may be unstable");
 * }
 * @endcode
 */
#define EMU_VERIFY_MSG(cond, msg) BOOST_VERIFY_MSG(cond, msg)

/**
 * @def EMU_ASSERT_IS_VOID
 * @brief Indicates whether assertions are compiled out.
 *
 * This macro is defined when assertions are compiled out (typically in release builds).
 * It can be used for conditional compilation when assertion behavior needs to be
 * detected at compile time.
 *
 * @par Example:
 * @code
 * void debug_function() {
 * #ifndef EMU_ASSERT_IS_VOID
 *     // This code only runs when assertions are active
 *     expensive_debug_check();
 * #endif
 * }
 * @endcode
 */
#define EMU_ASSERT_IS_VOID BOOST_ASSERT_IS_VOID

/**
 * @def EMU_ASSERT_ASSUME(cond)
 * @brief Assert a condition and provide compiler optimization hint.
 *
 * This macro combines assertion checking with compiler optimization hints.
 * It asserts the condition in debug builds and tells the compiler to assume
 * the condition is true for optimization purposes.
 *
 * @param cond The condition to assert and assume
 *
 * @par Example:
 * @code
 * void process_array(int* arr, size_t size) {
 *     EMU_ASSERT_ASSUME(arr != nullptr);  // Assert and assume non-null
 *     EMU_ASSERT_ASSUME(size > 0);        // Assert and assume positive size
 *
 *     // Compiler can optimize knowing arr is non-null and size > 0
 *     for (size_t i = 0; i < size; ++i) {
 *         arr[i] = arr[i] * 2;
 *     }
 * }
 * @endcode
 */
#define EMU_ASSERT_ASSUME(cond) \
    do {                        \
        EMU_ASSERT(cond);       \
        EMU_ASSUME(cond);       \
    } while(0)

/**
 * @def EMU_ASSERT_ASSUME_MSG(cond, msg)
 * @brief Assert a condition with message and provide compiler optimization hint.
 *
 * This macro combines assertion checking with a custom message and compiler
 * optimization hints. It asserts the condition with a message in debug builds
 * and tells the compiler to assume the condition is true.
 *
 * @param cond The condition to assert and assume
 * @param msg The error message to display if assertion fails
 *
 * @par Example:
 * @code
 * double safe_sqrt(double x) {
 *     EMU_ASSERT_ASSUME_MSG(x >= 0.0, "Square root requires non-negative input");
 *
 *     // Compiler can optimize knowing x is non-negative
 *     return std::sqrt(x);
 * }
 * @endcode
 */
#define EMU_ASSERT_ASSUME_MSG(cond, msg) \
    do {                                 \
        EMU_ASSERT_MSG(cond, msg);       \
        EMU_ASSUME(cond);                \
    } while(0)

/**
 * @def EMU_UNREACHABLE
 * @brief Indicates that a code path should never be reached.
 *
 * This macro marks code as unreachable, which helps with compiler optimizations
 * and can catch logic errors. If this code is reached during execution,
 * the behavior is undefined.
 *
 * @par Example:
 * @code
 * enum class Color { RED, GREEN, BLUE };
 *
 * const char* color_to_string(Color c) {
 *     switch (c) {
 *         case Color::RED:   return "red";
 *         case Color::GREEN: return "green";
 *         case Color::BLUE:  return "blue";
 *     }
 *     EMU_UNREACHABLE;  // All enum values handled above
 * }
 * @endcode
 */
#define EMU_UNREACHABLE __builtin_unreachable()

/**
 * @def EMU_ASSUME_UNREACHABLE
 * @brief Assert that code should never be reached and mark as unreachable.
 *
 * This macro combines assertion checking with unreachable code marking.
 * It asserts false (indicating the code should not be reached) and provides
 * compiler optimization hints.
 *
 * @par Example:
 * @code
 * void handle_error_code(int error) {
 *     if (error == 0) {
 *         // Handle success
 *         return;
 *     } else if (error > 0) {
 *         // Handle positive error codes
 *         throw std::runtime_error("Positive error");
 *     } else {
 *         // Handle negative error codes
 *         throw std::runtime_error("Negative error");
 *     }
 *
 *     EMU_ASSUME_UNREACHABLE;  // All cases handled above
 * }
 * @endcode
 */
#define EMU_ASSUME_UNREACHABLE EMU_ASSERT_ASSUME(false)

/**
 * @def EMU_ASSUME_UNREACHABLE_MSG(msg)
 * @brief Assert unreachable code with custom message.
 *
 * This macro marks code as unreachable with a custom error message.
 * It combines assertion checking with unreachable code marking and
 * provides a descriptive message for debugging.
 *
 * @param msg The error message to display if this code is reached
 *
 * @par Example:
 * @code
 * void process_validated_input(int value) {
 *     // Assume value has been validated to be in range [1, 10]
 *
 *     if (value >= 1 && value <= 5) {
 *         handle_low_value(value);
 *     } else if (value >= 6 && value <= 10) {
 *         handle_high_value(value);
 *     } else {
 *         EMU_ASSUME_UNREACHABLE_MSG("Input validation failed - value out of range");
 *     }
 * }
 * @endcode
 */
#define EMU_ASSUME_UNREACHABLE_MSG(msg) EMU_ASSERT_ASSUME_MSG(false, msg)

#define EMU_EXPECTS(cond) Expects(cond)
#define EMU_ENSURES(cond) Ensures(cond)
