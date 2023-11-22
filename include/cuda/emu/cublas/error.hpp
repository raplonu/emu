#pragma once

#include <emu/detail/error.hpp>

#include <cublas_v2.h>

namespace emu
{

namespace cublas
{

    /**
     * @brief The error category for CUBLAS.
     */
    using status_t = cublasStatus_t;

    /**
     * Aliases for cublas status codes
     *
     * @note unfortunately, this enum can't inherit from status_t
     */
    enum status : std::underlying_type<status_t>::type
    {
        success          = CUBLAS_STATUS_SUCCESS,
        not_initialized  = CUBLAS_STATUS_NOT_INITIALIZED,
        alloc_failed     = CUBLAS_STATUS_ALLOC_FAILED,
        invalid_value    = CUBLAS_STATUS_INVALID_VALUE,
        arch_mismatch    = CUBLAS_STATUS_ARCH_MISMATCH,
        mapping_error    = CUBLAS_STATUS_MAPPING_ERROR,
        execution_failed = CUBLAS_STATUS_EXECUTION_FAILED,
        internal_error   = CUBLAS_STATUS_INTERNAL_ERROR,
        not_supported    = CUBLAS_STATUS_NOT_SUPPORTED,
        license_error    = CUBLAS_STATUS_LICENSE_ERROR
    };

    std::string describe(status_t error_code);

    EMU_GENERATE_ERROR_HANDLER("Cublas", runtime_error, std::runtime_error, status_t, status::success, describe)

} // namespace cublas

} // namespace emu
