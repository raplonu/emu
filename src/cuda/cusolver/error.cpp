
#include <emu/cusolver/error.hpp>

namespace emu::cusolver
{

    const char* describe(status_t error_code) {
        switch (error_code) {
            case CUSOLVER_STATUS_SUCCESS:
                return "Operation completed successfully.";
            case CUSOLVER_STATUS_NOT_INITIALIZED:
                return "Solver library was not initialized.";
            case CUSOLVER_STATUS_ALLOC_FAILED:
                return "Memory allocation failed during operation.";
            case CUSOLVER_STATUS_INVALID_VALUE:
                return "An invalid value was provided to the solver.";
            case CUSOLVER_STATUS_ARCH_MISMATCH:
                return "The device architecture does not match requirements.";
            case CUSOLVER_STATUS_MAPPING_ERROR:
                return "A memory mapping error occurred.";
            case CUSOLVER_STATUS_EXECUTION_FAILED:
                return "The solver failed to execute the operation.";
            case CUSOLVER_STATUS_INTERNAL_ERROR:
                return "An internal error occurred in the solver.";
            case CUSOLVER_STATUS_MATRIX_TYPE_NOT_SUPPORTED:
                return "The matrix type is not supported by the solver.";
    #if CUDART_VERSION >= 11000
            case CUSOLVER_STATUS_NOT_SUPPORTED:
                return "This operation is not supported.";
            case CUSOLVER_STATUS_ZERO_PIVOT:
                return "A zero pivot was encountered during factorization.";
            case CUSOLVER_STATUS_INVALID_LICENSE:
                return "The solver license is invalid.";
    #endif
            default:
                return "Unknown cuSolver error code.";
        }
    }

} // namespace emu::cusolver
