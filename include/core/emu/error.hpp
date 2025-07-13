#pragma once

#include <emu/fwd.hpp>
#include <emu/expected.hpp>
#include <emu/error/generation.hpp>

#include <stdexcept>
#include <system_error>
#include <cerrno>

namespace emu
{

    enum class errc
    {
        success = 0,

        dlpack_rank_mismatch,
        dlpack_type_mismatch,
        dlpack_strides_not_supported,
        dlpack_read_only,
        dlpack_unkown_device_type,
        dlpack_unkown_data_type_code,
        dlpack_bits_too_large,

        numeric_dl_data_type_not_supported,

        pointer_desc_not_found,
        pointer_device_not_found,
        pointer_maps_file_not_found,

        cuda_pointer_unregistered,

        not_implemented
    };

    std::string_view describe(errc ev);

    EMU_GENERATE_ERROR_INFRA(
        error_category,
        "emu",
        errc,
        runtime_error,
        std::runtime_error,
        ev
    ) { return std::string(describe(static_cast<errc>(ev))); }

    /// special case to be used with the error macro below to work with std::error_code.
    inline std::error_code make_error_code( std::error_code e ) { return e; }
    /**
    * @brief Return a unexpected<std::error_code> from a multiple types of error
    *
    * This allow to create result in an error state without knowing the value_type.
    *
    * @param e The emu::error
    * @return The std::error_code
    */
    unexpected<std::error_code> make_unexpected( std::error_code e );

    [[noreturn]] void throw_error( std::error_code e );
    [[noreturn]] void throw_error( std::error_code e, const std::string& what_arg );

    unexpected<std::error_code> make_unexpected( std::errc e );
    [[noreturn]] void throw_error( std::errc e );
    [[noreturn]] void throw_error( std::errc e, const std::string& what_arg );

    /**
     * @brief This is a helper type that is used to bypass the optional/expected limitation of not being able to hold a reference.
     *
     * @tparam T
     */
    template<typename T>
    using return_type = std::conditional_t<is_ref<T>, std::reference_wrapper<rm_ref<T>>, T>;

    template<typename T>
    using result = expected<return_type<T>, std::error_code>;

namespace detail
{

    struct pretty_error_code
    {
        std::error_code ec;
    };

} // namespace detail

    inline detail::pretty_error_code pretty(std::error_code ec) noexcept {
        return {ec};
    }

} // namespace emu

template <>
struct std::is_error_code_enum< emu::errc > : std::true_type {};

template<typename Char>
struct fmt::formatter<emu::detail::pretty_error_code, Char>
{
    constexpr auto parse(format_parse_context& ctx) -> format_parse_context::iterator {
        return ctx.end();
    }

    auto format(const emu::detail::pretty_error_code& error, format_context& ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "{}/{}: {}",
            error.ec.category().name(),
            error.ec.value(),
            error.ec.message()
        );
    }

};


/**
 * @brief Returns a `std::error_code` from a function.
 * @param ec The error code to return. This can be a `std::errc`, `emu::errc`, or `std::error_code`.
 *
 * @code
 * std::error_code my_function() {
 *     EMU_RETURN_EC(std::errc::invalid_argument);
 * }
 * @endcode
 */
#define EMU_RETURN_EC(ec)             \
do {                              \
        using ::emu::make_error_code; \
        return make_error_code(ec);   \
    } while (false)

/**
 * @brief Returns a `emu::unexpected<std::error_code>` from a function.
 * @param ec The error code to return. This can be a `std::errc`, `emu::errc`, or `std::error_code`.
 *
 * @code
 * emu::result<int> my_function() {
 *     EMU_RETURN_UN_EC(std::errc::invalid_argument);
 * }
 * @endcode
 */
#define EMU_RETURN_UN_EC(ec)                                \
do {                                                    \
        using ::emu::make_error_code;                       \
        return ::emu::make_unexpected(make_error_code(ec)); \
    } while (false)

/**
 * @brief Throws a `std::runtime_error` with a message derived from the error code.
 * @param ec The error code to throw. This can be a `std::errc`, `emu::errc`, or `std::error_code`.
 *
 * @code
 * void my_function() {
 *     EMU_THROW(std::errc::invalid_argument);
 * }
 * @endcode
 */
#define EMU_THROW(ec)                               \
do {                                            \
        using ::emu::throw_error;                   \
        [&] () EMU_NOINLINE { throw_error(ec); }(); \
    } while (false)

/**
 * @brief Throws a `std::runtime_error` with a custom message.
 * @param ec The error code to throw. This can be a `std::errc`, `emu::errc`, or `std::error_code`.
 * @param what_arg The custom message.
 *
 * @code
 * void my_function() {
 *     EMU_THROW_WHAT(std::errc::invalid_argument, "My custom message");
 * }
 * @endcode
 */
#define EMU_THROW_WHAT(ec, what_arg)                          \
do {                                                      \
        using ::emu::throw_error;                             \
        [&] () EMU_NOINLINE { throw_error(ec, what_arg); }(); \
    } while (false)


/**
 * @brief Checks if an expression is true, otherwise returns an error code.
 * @param expr The expression to check.
 * @param ec The error code to return if the expression is false.
 *
 * @code
 * std::error_code my_function(int* ptr) {
 *     EMU_TRUE_OR_RETURN_EC(ptr != nullptr, std::errc::invalid_argument);
 *     return {};
 * }
 * @endcode
 */
#define EMU_TRUE_OR_RETURN_EC(expr, ec) \
do {                                \
        if (EMU_UNLIKELY(not (expr))) { \
            EMU_RETURN_EC(ec);          \
        }                               \
    } while (false)

/**
 * @brief Checks if an expression is true, otherwise returns an unexpected error code.
 * @param expr The expression to check.
 * @param ec The error code to return if the expression is false.
 *
 * @code
 * emu::result<void> my_function(int* ptr) {
 *     EMU_TRUE_OR_RETURN_UN_EC(ptr != nullptr, std::errc::invalid_argument);
 *     return {};
 * }
 * @endcode
 */
#define EMU_TRUE_OR_RETURN_UN_EC(expr, ec) \
do {                                   \
        if (EMU_UNLIKELY(not (expr))) {    \
            EMU_RETURN_UN_EC(ec);          \
        }                                  \
    } while (false)

/**
 * @brief Checks if an expression is true, otherwise throws an exception.
 * @param expr The expression to check.
 * @param ec The error code to throw if the expression is false.
 *
 * @code
 * void my_function(int* ptr) {
 *     EMU_TRUE_OR_THROW(ptr != nullptr, std::errc::invalid_argument);
 * }
 * @endcode
 */
#define EMU_TRUE_OR_THROW(expr, ec)     \
do {                                \
    if (EMU_UNLIKELY(not (expr))) { \
        EMU_THROW(ec);              \
    }                               \
} while (false)

/**
 * @brief Checks if an expression is true, otherwise throws an exception with a custom message.
 * @param expr The expression to check.
 * @param ec The error code to throw if the expression is false.
 * @param WHAT The custom message.
 *
 * @code
 * void my_function(int* ptr) {
 *    EMU_TRUE_OR_THROW_WHAT(ptr != nullptr, std::errc::invalid_argument, "Pointer is null");
 * }
 * @endcode
 */
#define EMU_TRUE_OR_THROW_WHAT(expr, ec, WHAT) \
do {                                       \
    if (EMU_UNLIKELY(not (expr))) {        \
        EMU_THROW_WHAT(ec, WHAT);          \
    }                                      \
} while (false)

/**
 * @brief Checks the status of an expression, and returns if it's an error.
 * @param expr The expression to check. The expression should return a `std::error_code`.
 *
 * @code
 * std::error_code another_function();
 *
 * std::error_code my_function() {
 *     EMU_CHECK_OR_RETURN_EC(another_function());
 *     return {};
 * }
 * @endcode
 */
#define EMU_CHECK_OR_RETURN_EC(expr)  \
do {                              \
        auto&& status__ = (expr);     \
        if (EMU_UNLIKELY(status__)) { \
            EMU_RETURN_EC(status__);  \
        }
    } while (false)

/**
 * @brief Checks the status of an expression, and returns an unexpected error if it's an error.
 * @param expr The expression to check. The expression should return a `std::error_code`.
 *
 * @code
 * std::error_code another_function();
 *
 * emu::result<void> my_function() {
 *     EMU_CHECK_OR_RETURN_UN_EC(another_function());
 *     return {};
 * }
 * @endcode
 */
#define EMU_CHECK_OR_RETURN_UN_EC(expr) \
do {                                \
        auto&& status__ = (expr);       \
        if (EMU_UNLIKELY(status__)) {   \
            EMU_RETURN_UN_EC(status__); \
        }
    } while (false)

/**
 * @brief Checks the status of an expression, and throws an exception if it's an error.
 * @param expr The expression to check. The expression should return a `std::error_code`.
 *
 * @code
 * std::error_code another_function();
 *
 * void my_function() {
 *     EMU_CHECK_OR_THROW(another_function());
 * }
 * @endcode
 */
#define EMU_CHECK_OR_THROW(expr)      \
do {                                  \
        auto&& status__ = (expr);     \
        if (EMU_UNLIKELY(status__)) { \
            EMU_THROW(status__);      \
        }
    } while (false)

/**
 * @brief Checks the status of an expression, and throws an exception with a custom message if it's an error.
 * @param expr The expression to check. The expression should return a `std::error_code`.
 * @param WHAT The custom message.
 *
 * @code
 * std::error_code another_function();
 *
 * void my_function() {
 *     EMU_CHECK_OR_THROW_WHAT(another_function(), "another_function failed");
 * }
 * @endcode
 */
#define EMU_CHECK_OR_THROW_WHAT(expr, WHAT) \
do {                                        \
        auto&& status__ = (expr);           \
        if (EMU_UNLIKELY(status__)) {       \
            EMU_THROW_WHAT(status__, WHAT); \
        }
    } while (false)


// ###########################
// # EMU_UNWRAP_OR_XXX macro #
// ###########################

/// This category of macro will check a dereferenceable expression `expr`.
/// If the expression is true, it will continue execution and unwrap it.
/// If the expression is false, it will propagate the error `ec`.

/**
 * @brief Unwraps a value from an expression or returns an error code.
 * @param expr The expression to unwrap. This should be a type that can be dereferenced, like a pointer or an optional.
 * @param ec The error code to return if the expression is false.
 * @return The unwrapped value.
 *
 * @code
 * std::error_code my_function(std::optional<int> opt) {
 *     int value = EMU_UNWRAP_OR_RETURN_EC(opt, std::errc::invalid_argument);
 *     // use value
 *     return {};
 * }
 * @endcode
 */
#define EMU_UNWRAP_OR_RETURN_EC(expr, ec)   \
    ({                                      \
        auto&& value__ = (expr);            \
        EMU_TRUE_OR_RETURN_EC(value__, ec); \
        *EMU_FWD(value__);                  \
    })

/**
 * @brief Unwraps a value from an expression or returns an unexpected error code.
 * @param expr The expression to unwrap. This should be a type that can be dereferenced, like a pointer or an optional.
 * @param ec The error code to return if the expression is false.
 * @return The unwrapped value.
 *
 * @code
 * emu::result<int> my_function(std::optional<int> opt) {
 *     int value = EMU_UNWRAP_OR_RETURN_UN_EC(opt, std::errc::invalid_argument);
 *     return value;
 * }
 * @endcode
 */
#define EMU_UNWRAP_OR_RETURN_UN_EC(expr, ec)   \
    ({                                         \
        auto&& value__ = (expr);               \
        EMU_TRUE_OR_RETURN_UN_EC(value__, ec); \
        *EMU_FWD(value__);                     \
    })

/**
 * @brief Unwraps a value from an expression or throws an exception.
 * @param expr The expression to unwrap. This should be a type that can be dereferenced, like a pointer or an optional.
 * @param ec The error code to throw if the expression is false.
 * @return The unwrapped value.
 *
 * @code
 * void my_function(std::optional<int> opt) {
 *     int value = EMU_UNWRAP_OR_THROW(opt, std::errc::invalid_argument);
 *     // use value
 * }
 * @endcode
 */
#define EMU_UNWRAP_OR_THROW(expr, ec)   \
    ({                                  \
        auto&& value__ = (expr);        \
        EMU_TRUE_OR_THROW(value__, ec); \
        *EMU_FWD(value__);              \
    })

/**
 * @brief Unwraps a value from an expression or throws an exception with a custom message.
 * @param expr The expression to unwrap. This should be a type that can be dereferenced, like a pointer or an optional.
 * @param ec The error code to throw if the expression is false.
 * @param WHAT The custom message.
 * @return The unwrapped value.
 *
 * @code
 * void my_function(std::optional<int> opt) {
 *     int value = EMU_UNWRAP_OR_THROW_WHAT(opt, std::errc::invalid_argument, "Optional is empty");
 *     // use value
 * }
 * @endcode
 */
#define EMU_UNWRAP_OR_THROW_WHAT(expr, ec, WHAT)   \
    ({                                             \
        auto&& value__ = (expr);                   \
        EMU_TRUE_OR_THROW_WHAT(value__, ec, WHAT); \
        *EMU_FWD(value__);                         \
    })



// ###############################
// # EMU_UNWRAP_RES_OR_XXX macro #
// ###############################

/// Special category that complete `EMU_UNWRAP` with the ability to
/// throw an error if the result is an error.

/**
 * @brief Unwraps a value from a `emu::result` or throws an exception.
 * @param expr The `emu::result` to unwrap.
 * @return The unwrapped value.
 *
 * @code
 * emu::result<int> another_function();
 *
 * void my_function() {
 *     int value = EMU_UNWRAP_RES_OR_THROW(another_function());
 *     // use value
 * }
 * @endcode
 */
#define EMU_UNWRAP_RES_OR_THROW(expr)                             \
    ({                                                            \
        auto&& value__ = (expr);                                  \
        EMU_TRUE_OR_THROW(value__, ::std::move(value__).error()); \
        *EMU_FWD(value__);                                        \
    })

/**
 * @brief Unwraps a value from a `emu::result` or throws an exception with a custom message.
 * @param expr The `emu::result` to unwrap.
 * @param WHAT The custom message.
 * @return The unwrapped value.
 *
 * @code
 * emu::result<int> another_function();
 *
 * void my_function() {
 *     int value = EMU_UNWRAP_RES_OR_THROW_WHAT(another_function(), "another_function failed");
 *     // use value
 * }
 * @endcode
 */
#define EMU_UNWRAP_RES_OR_THROW_WHAT(expr, WHAT)                             \
    ({                                                                       \
        auto&& value__ = (expr);                                             \
        EMU_TRUE_OR_THROW_WHAT(value__, ::std::move(value__).error(), WHAT); \
        *EMU_FWD(value__);                                                   \
    })
