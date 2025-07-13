#pragma once

#pragma once

#include <emu/macro.hpp>
#include <emu/assert.hpp>
#include <emu/expected.hpp>

#include <fmt/base.h>

#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

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


#define EMU_GENERATE_ERROR_CATEGORY(CATEGORY_T, CATEGORY_NAME, ERROR_CODE) \
EMU_GENERATE_ERROR_CATEGORY_DEFINITION(CATEGORY_T, CATEGORY_NAME);         \
::std::string CATEGORY_T::message(int ERROR_CODE) const



#define EMU_GENERATE_ERROR_EXCEPTION(ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, CATEGORY_T) \
struct EXCEPTION_T : EXCEPTION_BASE_T {                                                         \
    EXCEPTION_T(ERROR_STATUS_T ec) :                                                            \
        EXCEPTION_BASE_T(fmt::format("{} error: {}",                                            \
            CATEGORY_T::instance().name(),                                                      \
            CATEGORY_T::instance().message(static_cast<int>(ec)))),                             \
            code_(ec)                                                                           \
    {}                                                                                          \
    EXCEPTION_T(ERROR_STATUS_T ec, ::std::string_view what_arg) :                               \
        EXCEPTION_BASE_T(::fmt::format("{} error: {}; {}",                                      \
            CATEGORY_T::instance().name(),                                                      \
            CATEGORY_T::instance().message(static_cast<int>(ec)), what_arg)),                   \
            code_(ec)                                                                           \
    {}                                                                                          \
    ERROR_STATUS_T code() const { return code_; }                                               \
private:                                                                                        \
    ERROR_STATUS_T code_;                                                                       \
}

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
[[noreturn]] inline void throw_error(ERROR_STATUS_T e, ::std::string_view what_arg) {  \
    throw EXCEPTION_T(e, what_arg);                                                    \
}

#define EMU_GENERATE_ERROR_INFRA(CATEGORY_T, CATEGORY_NAME, ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, ERROR_CODE) \
EMU_GENERATE_ERROR_CATEGORY_DEFINITION(CATEGORY_T, CATEGORY_NAME);                                                     \
EMU_GENERATE_ERROR_EXCEPTION(ERROR_STATUS_T, EXCEPTION_T, EXCEPTION_BASE_T, CATEGORY_T);                               \
EMU_GENERATE_ERROR_FUNCTION(ERROR_STATUS_T, CATEGORY_T, EXCEPTION_T)                                                   \
::std::string CATEGORY_T::message(int ERROR_CODE) const
