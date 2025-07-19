#pragma once

#include <emu/macro.hpp>
#include <emu/assert.hpp>
#include <emu/expected.hpp>

#include <fmt/base.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace emu {}

/**
 * @brief Defines a struct based on `std::error_category`.
 * @param CATEGORY_T The name of the category struct to be created.
 * @param CATEGORY_NAME A string literal for the name of the category.
 * @details This macro only defines the struct and its `name()` and `instance()` methods.
 *          The `message(int) const override` method must be defined separately.
 * @see EMU_GENERATE_ERROR_CATEGORY
 * @see EMU_GENERATE_ERROR_INFRA
 *
 * @code
 * // Defines a struct MyErrorCategory
 * EMU_GENERATE_ERROR_CATEGORY_DEFINITION(MyErrorCategory, "my_error");
 *
 * // Definition of the message function must follow
 * std::string MyErrorCategory::message(int ev) const {
 *     // ... implementation ...
 * }
 * @endcode
 */
#define EMU_GENERATE_ERROR_CATEGORY_DEFINITION(CATEGORY_T, CATEGORY_NAME)  \
struct CATEGORY_T : public ::std::error_category {                         \
    [[nodiscard]] inline ::std::string message(int ev) const override;     \
    [[nodiscard]] const char * name() const noexcept override {            \
        return CATEGORY_NAME;                                              \
    }                                                                      \
    static const ::std::error_category& instance() {                       \
        static const CATEGORY_T instance;                                  \
        return instance;                                                   \
    }                                                                      \
}

/**
 * @brief Defines a `std::error_category` and starts the `message` function implementation.
 * @param CATEGORY_T The name of the category struct to be created.
 * @param CATEGORY_NAME A string literal for the name of the category.
 * @param ERROR_CODE The name of the integer variable that will hold the error value in the `message` function body.
 * @details This macro combines the definition of the category struct with the signature of the `message` function,
 *          allowing you to immediately provide its implementation body.
 * @see EMU_GENERATE_ERROR_INFRA
 *
 * @code
 * // Defines MyErrorCategory and starts the message() implementation
 * EMU_GENERATE_ERROR_CATEGORY(MyErrorCategory, "my_error", ev) {
 *     switch (static_cast<MyErrc>(ev)) {
 *         case MyErrc::SomeError: return "Some error occurred";
 *         default: return "Unknown error";
 *     }
 * }
 * @endcode
 */
#define EMU_GENERATE_ERROR_CATEGORY(CATEGORY_T, CATEGORY_NAME, ERROR_CODE) \
EMU_GENERATE_ERROR_CATEGORY_DEFINITION(CATEGORY_T, CATEGORY_NAME);         \
::std::string CATEGORY_T::message(int ERROR_CODE) const

/**
 * @brief Defines a custom exception class.
 * @param ERROR_STATUS_T The enum type for the error status (e.g., `MyErrc`).
 * @param EXCEPTION_T The name of the exception class to be created.
 * @param EXCEPTION_BASE_T The base class for the exception (e.g., `std::runtime_error`).
 * @param CATEGORY_T The error category struct associated with this exception.
 * @details The generated exception class will have two constructors: one that takes an error code,
 *          and another that takes an error code and a custom `what_arg` string.
 * @see EMU_GENERATE_ERROR_INFRA
 *
 * @code
 * // Assumes MyErrorCategory and MyErrc are defined
 * EMU_GENERATE_ERROR_EXCEPTION(MyErrc, MyException, std::runtime_error, MyErrorCategory);
 *
 * // throw MyException(MyErrc::SomeError);
 * @endcode
 */
#define EMU_GENERATE_ERROR_EXCEPTION(ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, CATEGORY_T) \
struct EXCEPTION_T : EXCEPTION_BASE_T {                                                         \
    EXCEPTION_T(ERROR_STATUS_T ec) :                                                            \
        EXCEPTION_BASE_T(fmt::format("{} error: {}",                                            \
            CATEGORY_T::instance().name(),                                                      \
            CATEGORY_T::instance().message(static_cast<int>(ec)))),                             \
            code_(ec)                                                                           \
    {}                                                                                          \
    template <typename... T>                                                                    \
    EXCEPTION_T(ERROR_STATUS_T ec, ::fmt::format_string<T...> fmt, T&&... args) :               \
        EXCEPTION_BASE_T(::fmt::format("{} error: {}; {}",                                      \
            CATEGORY_T::instance().name(),                                                      \
            CATEGORY_T::instance().message(static_cast<int>(ec)),                               \
            fmt::format(fmt, EMU_FWD(args)...)                                                  \
        )),                                                                                     \
        code_(ec)                                                                               \
    {}                                                                                          \
    ERROR_STATUS_T code() const { return code_; }                                               \
private:                                                                                        \
    ERROR_STATUS_T code_;                                                                       \
}

/**
 * @brief Generates helper functions for a given error type.
 * @param ERROR_STATUS_T The enum type for the error status (e.g., `MyErrc`).
 * @param CATEGORY_T The error category struct.
 * @param EXCEPTION_T The exception class.
 * @details This macro generates the following functions for the given `ERROR_STATUS_T`:
 *          - `make_error_code(ERROR_STATUS_T)`
 *          - `make_unexpected(ERROR_STATUS_T)`
 *          - `throw_error(ERROR_STATUS_T)`
 *          - `throw_error(ERROR_STATUS_T, std::string_view)`
 * @see EMU_GENERATE_ERROR_INFRA
 *
 * @code
 * // Assumes MyErrc, MyErrorCategory, and MyException are defined
 * EMU_GENERATE_ERROR_FUNCTION(MyErrc, MyErrorCategory, MyException);
 *
 * // auto ec = make_error_code(MyErrc::SomeError);
 * @endcode
 */
#define EMU_GENERATE_ERROR_FUNCTION(ERROR_STATUS_T, CATEGORY_T, EXCEPTION_T)           \
inline ::std::error_code make_error_code(ERROR_STATUS_T e) {                           \
    return { static_cast<int>(e), CATEGORY_T::instance() };                            \
}                                                                                      \
inline ::emu::unexpected<::std::error_code> make_unexpected(ERROR_STATUS_T e) {        \
    return ::emu::unexpected<::std::error_code>{ make_error_code(e) };                 \
}                                                                                      \
[[noreturn]] inline void throw_error(ERROR_STATUS_T e) {                               \
    throw EXCEPTION_T(e);                                                              \
}                                                                                      \
template <typename... T>                                                               \
[[noreturn]] inline void throw_error(ERROR_STATUS_T e, ::fmt::format_string<T...> fmt, T&&... args) {  \
    throw EXCEPTION_T(e, fmt, EMU_FWD(args)...);                                                    \
}

/**
 * @brief Generates the complete error handling infrastructure for a new error type.
 * @param CATEGORY_T The name of the category struct to be created.
 * @param CATEGORY_NAME A string literal for the name of the category.
 * @param ERROR_STATUS_T The enum type for the error status (e.g., `MyErrc`).
 * @param EXCEPTION_T The name of the exception class to be created.
 * @param EXCEPTION_BASE_T The base class for the exception (e.g., `std::runtime_error`).
 * @param ERROR_CODE The name of the integer variable for the `message` function body.
 * @details This is a convenience macro that combines `EMU_GENERATE_ERROR_CATEGORY_DEFINITION`,
 *          `EMU_GENERATE_ERROR_EXCEPTION`, and `EMU_GENERATE_ERROR_FUNCTION`, and starts
 *          the implementation of the `message` function.
 *
 * @code
 * // Define the error enum
 * enum class MyErrc { Success = 0, SomeError };
 *
 * // Generate all the infrastructure and implement the message function
 * EMU_GENERATE_ERROR_INFRA(MyErrorCategory, "my_error", MyErrc, MyException, std::runtime_error, ev) {
 *     switch (static_cast<MyErrc>(ev)) {
 *         case MyErrc::Success: return "Success";
 *         case MyErrc::SomeError: return "Some error occurred";
 *         default: return "Unknown error";
 *     }
 * }
 *
 * // Also need to specialize std::is_error_code_enum
 * namespace std {
 *   template <> struct is_error_code_enum<MyErrc> : true_type {};
 * }
 * @endcode
 */
#define EMU_GENERATE_ERROR_INFRA(CATEGORY_T, CATEGORY_NAME, ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, ERROR_CODE) \
EMU_GENERATE_ERROR_CATEGORY_DEFINITION(CATEGORY_T, CATEGORY_NAME);                                                     \
EMU_GENERATE_ERROR_EXCEPTION(ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, CATEGORY_T);                               \
EMU_GENERATE_ERROR_FUNCTION(ERROR_STATUS_T, CATEGORY_T, EXCEPTION_T)                                                   \
::std::string CATEGORY_T::message(int ERROR_CODE) const
