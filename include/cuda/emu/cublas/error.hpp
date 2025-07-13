#pragma once

#include <emu/error.hpp>

#include <cublas_v2.h>

namespace emu
{

namespace cublas
{

    /**
     * @brief The error category for CUBLAS.
     */
    using status_t = cublasStatus_t;

    EMU_GENERATE_ERROR_CATEGORY(error_category, "cublas", ec) {
        return cublasGetStatusString(static_cast<status_t>(ec));
    }

    EMU_GENERATE_ERROR_EXCEPTION(status_t, runtime_error, std::runtime_error, error_category);

} // namespace cublas

    EMU_GENERATE_ERROR_FUNCTION(cublas::status_t, cublas::error_category, cublas::runtime_error);

} // namespace emu


#define EMU_CUBLAS_CHECK_RETURN_EC(expr) \
    EMU_CHECK_OR_RETURN_EC(expr)

#define EMU_CUBLAS_CHECK_RETURN_UN_EC(expr) \
    EMU_CHECK_OR_RETURN_UN_EC(expr)

#define EMU_CUBLAS_CHECK_OR_THROW(expr) \
    EMU_CHECK_OR_THROW(expr)

#define EMU_CUBLAS_CHECK_OR_THROW_WHAT(expr, WHAT) \
    EMU_CHECK_OR_THROW_WHAT(expr, WHAT)
