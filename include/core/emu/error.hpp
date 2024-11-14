#pragma once

#include <emu/fwd.hpp>
#include <emu/expected.hpp>

#include <stdexcept>
#include <system_error>
#include <cerrno>

namespace emu
{

    struct error_category: public std::error_category
    {
        [[nodiscard]] std::string message( int ev ) const override;
        [[nodiscard]] const char * name() const noexcept override;

        static const std::error_category& instance();
    };

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

        pointer_device_not_found,
        pointer_maps_file_not_found,
    };

    /**
     * @brief Return a std::error_code from a emu::error
     *
     * @param e The emu::error
     * @return The std::error_code
     */
    std::error_code make_error_code( errc e );

    // special case to be used with the error macro below to work with std::error_code.
    inline std::error_code make_error_code( std::error_code e ) { return e; }

    // Takes a errno value and return a std::error_code
    std::error_code make_errno_code( int e );

    /**
     * @brief Return a unexpected<std::error_code> from a multiple types of error
     *
     * This allow to create result in an error state without knowing the value_type.
     *
     * @param e The emu::error
     * @return The std::error_code
     */
    unexpected<std::error_code> make_unexpected( std::error_code e );
    unexpected<std::error_code> make_unexpected( errc e );
    unexpected<std::error_code> make_unexpected( std::errc e );

    [[noreturn]] void throw_error( std::error_code e );
    [[noreturn]] void throw_error( errc e );
    [[noreturn]] void throw_error( std::errc e );

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

// #################################
// # error template handling macro #
// #################################

// Take into status + success_value, return errc if status is not success_value
#define EMU_SUCCESS_OR_RETURN_UN_EC(expr, success_value)                \
    do {                                                                \
        auto&& status__ = (expr);                                       \
        if (EMU_UNLIKELY((status__) != (success_value))) {              \
            using ::emu::make_error_code;                               \
            return ::emu::make_unexpected(make_error_code(status__));   \
        }                                                               \
    } while (false)

#define EMU_SUCCESS_OR_RETURN_UN_EC_LOG(expr, success_value, ...)       \
    do {                                                                \
        auto&& status__ = (expr);                                       \
        if (EMU_UNLIKELY((status__) != (success_value))) {              \
            EMU_COLD_LOGGER(__VA_ARGS__);                               \
            using ::emu::make_error_code;                               \
            return ::emu::make_unexpected(make_error_code(status__));   \
        }                                                               \
    } while (false)

#define EMU_SUCCESS_OR_THROW(expr, success_value)                       \
    do {                                                                \
        auto&& status__ = (expr);                                       \
        if (EMU_UNLIKELY((status__) != (success_value))) {              \
            using ::emu::make_error_code;                               \
            ::emu::throw_error(make_error_code(status__));              \
        }                                                               \
    } while (false)

#define EMU_SUCCESS_OR_THROW_LOG(expr, success_value, ...)              \
    do {                                                                \
        auto&& status__ = (expr);                                       \
        if (EMU_UNLIKELY((status__) != (success_value))) {              \
            EMU_COLD_LOGGER(__VA_ARGS__);                               \
            using ::emu::make_error_code;                               \
            ::emu::throw_error(make_error_code(status__));              \
        }                                                               \
    } while (false)

// ########################
// # error handling macro #
// ########################

/// emu provides a set of macro that use the standard error code system and the standard system_error exception.
/// The first keyword will determine the behavior when there is no error. *_TRUE_* macro will do nothing and continue
/// execution. *_UNWRAP_* will dereference for assignment or return.
/// The second keyword will determine what to do with the error. It can be either *_RETURN_UN_EC_* which stand for
/// "return unexpected error code". It relies on ADL to find the `make_error_code` function in the same
/// namespace than the errc argument.


// #########################
// # EMU_TRUE_OR_XXX macro #
// #########################

/// This category of macro will check a truthy expression `expr`.
/// If the expression is true, it will continue execution.
/// If the expression is false, it will propagate the error `errc`.


#define EMU_TRUE_OR_RETURN_UN_EC(expr, errc)                        \
    do {                                                            \
        if (EMU_UNLIKELY(not (expr))) {                             \
            using ::emu::make_error_code;                           \
            return ::emu::make_unexpected(make_error_code(errc));   \
        }                                                           \
    } while (false)

#define EMU_TRUE_OR_RETURN_UN_EC_LOG(expr, errc, ...)               \
    do {                                                            \
        if (EMU_UNLIKELY(not (expr))) {                             \
            EMU_COLD_LOGGER(__VA_ARGS__);                           \
            using ::emu::make_error_code;                           \
            return ::emu::make_unexpected(make_error_code(errc));   \
        }                                                           \
    } while (false)

#define EMU_TRUE_OR_THROW(expr, errc)                   \
    do {                                                \
        if (EMU_UNLIKELY(not (expr))) {                 \
            using ::emu::make_error_code;               \
            ::emu::throw_error(make_error_code(errc));  \
        }                                               \
    } while (false)

#define EMU_TRUE_OR_THROW_LOG(expr, errc, ...)          \
    do {                                                \
        if (EMU_UNLIKELY(not (expr))) {                 \
            EMU_COLD_LOGGER(__VA_ARGS__);               \
            using ::emu::make_error_code;               \
            ::emu::throw_error(make_error_code(errc));  \
        }                                               \
    } while (false)


// ###########################
// # EMU_UNWRAP_OR_XXX macro #
// ###########################

/// This category of macro will check a dereferenceable expression `expr`.
/// If the expression is true, it will continue execution and unwrap it.
/// If the expression is false, it will propagate the error `errc`.

// Take into bool + errc, return errc if bool is false, unwrap expr if true
#define EMU_UNWRAP_OR_RETURN_UN_EC(expr, errc)                      \
    ({                                                              \
        auto&& value__ = (expr);                                    \
        EMU_TRUE_OR_RETURN_UN_EC(value__, errc);                    \
        *EMU_FWD(value__);                                          \
    })

#define EMU_UNWRAP_OR_RETURN_UN_EC_LOG(expr, errc, ...)             \
    ({                                                              \
        auto&& value__ = (expr);                                    \
        EMU_TRUE_OR_RETURN_UN_EC_LOG(value__, errc, __VA_ARGS__);   \
        *EMU_FWD(value__);                                          \
    })

#define EMU_UNWRAP_OR_THROW(expr, errc)                             \
    ({                                                              \
        auto&& value__ = (expr);                                    \
        EMU_TRUE_OR_THROW(value__, errc);                           \
        *EMU_FWD(value__);                                          \
    })

#define EMU_UNWRAP_OR_THROW_LOG(expr, errc, ...)                    \
    ({                                                              \
        auto&& value__ = (expr);                                    \
        EMU_TRUE_OR_THROW_LOG(value__, errc, __VA_ARGS__);          \
        *EMU_FWD(value__);                                          \
    })


// #############################
// # EMU_NO_ERRNO_OR_XXX macro #
// #############################

/// This category of macro will check errno.
/// If errno is 0, it will continue execution.
/// If errno is not 0, it will propagate it.

#define EMU_NO_ERRNO_OR_RETURN_UN_EC()                                                      \
    do {                                                                                    \
        if (EMU_UNLIKELY(errno)) {                                                          \
            return ::emu::make_unexpected(emu::make_errno_code(std::exchange(errno, 0)));   \
        }                                                                                   \
    } while(false)

#define EMU_NO_ERRNO_OR_RETURN_UN_EC_LOG(...)                                               \
    do {                                                                                    \
        if (EMU_UNLIKELY(errno)) {                                                          \
            EMU_COLD_LOGGER(__VA_ARGS__);                                                   \
            return ::emu::make_unexpected(emu::make_errno_code(std::exchange(errno, 0)));   \
        }                                                                                   \
    } while(false)

#define EMU_NO_ERRNO_OR_THROW()                                                \
    do {                                                                       \
        if (EMU_UNLIKELY(errno)) {                                             \
            ::emu::throw_error(emu::make_errno_code(std::exchange(errno, 0))); \
        }                                                                      \
    } while(false)

#define EMU_NO_ERRNO_OR_THROW_LOG(...)                                         \
    do {                                                                       \
        if (EMU_UNLIKELY(errno)) {                                             \
            EMU_COLD_LOGGER(__VA_ARGS__);                                      \
            ::emu::throw_error(emu::make_errno_code(std::exchange(errno, 0))); \
        }                                                                      \
    } while(false)

// ###############################
// # EMU_TRUE_OR_ERRNO_XXX macro #
// ###############################

/// This category of macro will check a truthy expression `expr`.
/// If the expression is true, it will continue execution.
/// If the expression is false, it will propagate the error `errno`.

#define EMU_TRUE_OR_RETURN_UN_EC_ERRNO(expr)                                                \
    do {                                                                                    \
        if (EMU_UNLIKELY(not (expr))) {                                                     \
            return ::emu::make_unexpected(emu::make_errno_code(std::exchange(errno, 0)));   \
        }                                                                                   \
    } while (false)

#define EMU_TRUE_OR_RETURN_UN_EC_ERRNO_LOG(expr, ...)                                       \
    do {                                                                                    \
        if (EMU_UNLIKELY(not (expr))) {                                                     \
            EMU_COLD_LOGGER(__VA_ARGS__);                                                   \
            return ::emu::make_unexpected(emu::make_errno_code(std::exchange(errno, 0)));   \
        }                                                                                   \
    } while (false)

#define EMU_TRUE_OR_THROW_ERRNO(expr)                                           \
    do {                                                                        \
        if (EMU_UNLIKELY(not (expr))) {                                         \
            ::emu::throw_error(emu::make_errno_code(std::exchange(errno, 0)));  \
        }                                                                       \
    } while (false)

#define EMU_TRUE_OR_THROW_ERRNO_LOG(expr, ...)                                  \
    do {                                                                        \
        if (EMU_UNLIKELY(not (expr))) {                                         \
            EMU_COLD_LOGGER(__VA_ARGS__);                                       \
            ::emu::throw_error(emu::make_errno_code(std::exchange(errno, 0)));  \
        }                                                                       \
    } while (false)

// #############################
// # EMU_ASSIGN_OR_XXX macro #
// #############################

/// This category of macro will check errno after calling an expression `expr`.
/// If errno is 0, it will continue execution and assign the value.
/// If errno is not 0, it will propagate it.

#define EMU_ASSIGN_OR_RETURN_UN_EC(expr)    \
    ({                                      \
        auto&& value__ = (expr);            \
        EMU_NO_ERRNO_OR_RETURN_UN_EC();     \
        EMU_FWD(value__);                   \
    })

#define EMU_ASSIGN_OR_RETURN_UN_EC_LOG(expr, ...)       \
    ({                                                  \
        auto&& value__ = (expr);                        \
        EMU_NO_ERRNO_OR_RETURN_UN_EC_LOG(__VA_ARGS__);  \
        EMU_FWD(value__);                               \
    })

#define EMU_ASSIGN_OR_THROW_ERRNO(expr)     \
    ({                                      \
        auto&& value__ = (expr);            \
        EMU_NO_ERRNO_OR_THROW();            \
        EMU_FWD(value__);                   \
    })

#define EMU_ASSIGN_OR_THROW_ERRNO_LOG(expr, ...)    \
    ({                                              \
        auto&& value__ = (expr);                    \
        EMU_NO_ERRNO_OR_THROW_LOG(__VA_ARGS__);     \
        EMU_FWD(value__);                           \
    })

// ###############################
// # EMU_RES_TRUE_OR_XXX macro #
// ###############################

#define EMU_RES_TRUE_OR_RETURN_UN_EC(expr)                                      \
    do {                                                                        \
        auto&& value__ = (expr);                                                \
        if (EMU_UNLIKELY(not (value__))) {                                      \
            using ::emu::make_error_code;                                       \
            return ::emu::make_unexpected(make_error_code(value__.error()));    \
        }                                                                       \
    } while (false)

#define EMU_RES_TRUE_OR_RETURN_UN_EC_LOG(expr, ...)                             \
    do {                                                                        \
        auto&& value__ = (expr);                                                \
        if (EMU_UNLIKELY(not (value__))) {                                      \
            EMU_COLD_LOGGER(__VA_ARGS__);                                       \
            using ::emu::make_error_code;                                       \
            return ::emu::make_unexpected(make_error_code(value__.error()));    \
        }                                                                       \
    } while (false)

#define EMU_RES_TRUE_OR_THROW(expr)                                 \
    do {                                                            \
        auto&& value__ = (expr);                                    \
        if (EMU_UNLIKELY(not (value__))) {                          \
            using ::emu::make_error_code;                           \
            ::emu::throw_error(make_error_code(value__.error()));   \
        }                                                           \
    } while (false)

#define EMU_RES_TRUE_OR_THROW_LOG(expr, ...)                        \
    do {                                                            \
        auto&& value__ = (expr);                                    \
        if (EMU_UNLIKELY(not (value__))) {                          \
            EMU_COLD_LOGGER(__VA_ARGS__);                           \
            using ::emu::make_error_code;                           \
            ::emu::throw_error(make_error_code(value__.error()));   \
        }                                                           \
    } while (false)

// ###############################
// # EMU_UNWRAP_RES_OR_XXX macro #
// ###############################

/// Special category that complete `EMU_UNWRAP` with the ability to
/// throw an error if the result is an error.

#define EMU_UNWRAP_RES_OR_RETURN_UN_EC_LOG(expr, ...)                                       \
    ({                                                                                      \
        auto&& value__ = (expr);                                                            \
        EMU_TRUE_OR_RETURN_UN_EC_LOG(value__, ::std::move(value__).error(), __VA_ARGS__);   \
        *EMU_FWD(value__);                                                                  \
    })

#define EMU_UNWRAP_RES_OR_THROW(expr)                                   \
    ({                                                                  \
        auto&& value__ = (expr);                                        \
        EMU_TRUE_OR_THROW(value__, ::std::move(value__).error());       \
        *EMU_FWD(value__);                                              \
    })

#define EMU_UNWRAP_RES_OR_THROW_LOG(expr, ...)                          \
    ({                                                                  \
        auto&& value__ = (expr);                                        \
        EMU_TRUE_OR_THROW_LOG(value__, ::std::move(value__).error(), __VA_ARGS__);       \
        *EMU_FWD(value__);                                              \
    })

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
