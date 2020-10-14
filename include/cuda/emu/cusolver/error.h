#ifndef EMU_CUSOLVER_ERROR_H
#define EMU_CUSOLVER_ERROR_H

#include <emu/macro.h>

#include <fmt/core.h>
#include <cusolverDn.h>

namespace emu
{

namespace cusolver
{
    using status_t = cusolverStatus_t;

    /**
     * Aliases for cusolver status codes
     *
     * @note unfortunately, this enum can't inherit from @ref status_t
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

    constexpr inline bool is_success(status_t error_code)  { return error_code == static_cast<status_t>(status::success); }
    constexpr inline bool is_failure(status_t error_code)  { return error_code != static_cast<status_t>(status::success); }

    std::string describe(status_t error_code);

    class runtime_error : public std::runtime_error
    {
    public:
        runtime_error(status_t error_code) :
            std::runtime_error(fmt::format("Cusolver error : {}", describe(error_code))),
            code_(error_code)
        {}

        runtime_error(status_t error_code, const std::string& what_arg) :
            std::runtime_error(fmt::format("Cusolver error : {}; {}", describe(error_code), what_arg)),
            code_(error_code)
        {}

        runtime_error(status error_code) :
            runtime_error(static_cast<status_t>(error_code))
        {}

        runtime_error(status error_code, const std::string& what_arg) :
            runtime_error(static_cast<status_t>(error_code), what_arg)
        {}

        /**
         * Obtain the CUDA status code which resulted in this error being thrown.
         */
        status_t code() const { return code_; }

    private:
        status_t code_;
    };

/**
 * Do nothing... unless the status indicates an error, in which case
 * a @ref cusolver::runtime_error exception is thrown
 *
 * @param status should be @ref cusolver::status::success - otherwise an exception is thrown
 * @param message An extra description message to add to the exception
 */
inline void throw_if_error(cusolver::status_t status, std::string message) noexcept(false)
{
	if (is_failure(status))
        // Put the throw code in another function with cold path.
        [&] () EMU_COLD_PATH { throw runtime_error(status, message); }();
}

/**
 * Does nothing - unless the status indicates an error, in which case
 * a @ref cusolver::runtime_error exception is thrown
 *
 * @param status should be @ref cusolver::status::success - otherwise an exception is thrown
 */
inline void throw_if_error(cusolver::status_t status) noexcept(false)
{
	if (is_failure(status))
        // Put the throw code in another function with cold path.
        [&] () EMU_COLD_PATH { throw runtime_error(status); }();
}

} // namespace cusolver

} // namespace emu

#endif //EMU_CUSOLVER_ERROR_H