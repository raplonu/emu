#pragma once

#include <emu/macro.hpp>
#include <emu/assert.hpp>

#include <fmt/core.h>

#include <stdexcept>
/**
 * @brief Generates exception class and utility functions for a module/context.
 *
 * This macro aims a handling C API easier by tranforming error codes into exceptions.
 * It will generate an exception class and 4 functions:
 * - bool is_success(STATUS_T error_code): return true if status code is equal to `SUCCESS` value.
 * - bool is_failure(STATUS_T error_code): return true if status code is different to `SUCCESS` value.
 * - void throw_if_error(STATUS_T status): throw EXCEPTION_T if status code is different to `SUCCESS` value.
 * - void throw_if_error(STATUS_T status, std::string message): throw EXCEPTION_T if status code is
 *   different to `SUCCESS` value with the provided message.
 *
 * @param CATEGORY         Module/context name.
 * @param EXCEPTION_T      Generated exception class name.
 * @param EXCEPTION_BASE_T Generated exception class base.
 * @param STATUS_T         Error code type.
 * @param SUCCESS          Special error code value that denotes a success.
 * @param DESCRIBE_FN      Function that returns an error message associated to an error code.
 *
 */
#define EMU_GENERATE_ERROR_HANDLER(CATEGORY, EXCEPTION_T, EXCEPTION_BASE_T, STATUS_T, SUCCESS, DESCRIBE_FN)              \
constexpr inline bool is_success(STATUS_T error_code)  { return error_code == static_cast<STATUS_T>(SUCCESS); }          \
constexpr inline bool is_failure(STATUS_T error_code)  { return error_code != static_cast<STATUS_T>(SUCCESS); }          \
                                                                                                                         \
struct EXCEPTION_T : EXCEPTION_BASE_T {                                                                                  \
    EXCEPTION_T(STATUS_T error_code) :                                                                                   \
        EXCEPTION_BASE_T(fmt::format(CATEGORY" error : {}", DESCRIBE_FN(error_code))), code_(error_code)                 \
    {}                                                                                                                   \
    EXCEPTION_T(STATUS_T error_code, const std::string& what_arg) :                                                      \
        EXCEPTION_BASE_T(fmt::format(CATEGORY" error : {}; {}", DESCRIBE_FN(error_code), what_arg)), code_(error_code)   \
    {}                                                                                                                   \
    STATUS_T code() const { return code_; }                                                                              \
private:                                                                                                                 \
    STATUS_T code_;                                                                                                      \
};                                                                                                                       \
inline void throw_if_error(STATUS_T status) noexcept(false) {                                                            \
	if (EMU_UNLIKELY(is_failure(status)))                                                                                \
        [&] () EMU_NOINLINE { throw EXCEPTION_T(status); }();                                                            \
}                                                                                                                        \
inline void throw_if_error(STATUS_T status, std::string message) noexcept(false) {                                       \
	if (EMU_UNLIKELY(is_failure(status)))                                                                                \
        [&] () EMU_NOINLINE { throw EXCEPTION_T(status, message); }();                                                   \
}
