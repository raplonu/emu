#pragma once

#include <emu/error.hpp>

#include <cusolverDn.h>

namespace emu
{

namespace cusolver
{
    using status_t = cusolverStatus_t;

    const char* describe(status_t error_code);

    EMU_GENERATE_ERROR_CATEGORY(error_category, "cusolver", ec) {
        return describe(static_cast<status_t>(ec));
    }

    EMU_GENERATE_ERROR_EXCEPTION(status_t, runtime_error, std::runtime_error, error_category);

} // namespace cusolver

    EMU_GENERATE_ERROR_FUNCTION(cusolver::status_t, cusolver::error_category, cusolver::runtime_error);

} // namespace emu


#define EMU_CUSOLVER_CHECK_RETURN_EC(expr) \
    EMU_CHECK_OR_RETURN_EC(expr)

#define EMU_CUSOLVER_CHECK_RETURN_UN_EC(expr) \
    EMU_CHECK_OR_RETURN_UN_EC(expr)

#define EMU_CUSOLVER_CHECK_OR_THROW(expr) \
    EMU_CHECK_OR_THROW(expr)

#define EMU_CUSOLVER_CHECK_OR_THROW_WHAT(expr, WHAT) \
    EMU_CHECK_OR_THROW_WHAT(expr, WHAT)
