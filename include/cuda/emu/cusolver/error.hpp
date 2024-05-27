#pragma once

#include <emu/detail/error.hpp>

#include <cusolverDn.h>

namespace emu::cusolver
{
    using status_t = cusolverStatus_t;

    /**
     * Aliases for cusolver status codes
     *
     * @note unfortunately, this enum can't inherit from status_t
     */
    enum status : std::underlying_type<status_t>::type
    {
        success          = CUSOLVER_STATUS_SUCCESS,
        not_initialized  = CUSOLVER_STATUS_NOT_INITIALIZED,
        alloc_failed     = CUSOLVER_STATUS_ALLOC_FAILED,
        invalid_value    = CUSOLVER_STATUS_INVALID_VALUE,
        arch_mismatch    = CUSOLVER_STATUS_ARCH_MISMATCH,
        execution_failed = CUSOLVER_STATUS_EXECUTION_FAILED,
        internal_error   = CUSOLVER_STATUS_INTERNAL_ERROR,
        not_supported    = CUSOLVER_STATUS_MATRIX_TYPE_NOT_SUPPORTED
    };

    std::string describe(status_t error_code);

    EMU_GENERATE_ERROR_HANDLER("Cusolver", runtime_error, std::runtime_error, status_t, status::success, describe)

} // namespace emu::cusolver
