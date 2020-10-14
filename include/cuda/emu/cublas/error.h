#ifndef EMU_CUBLAS_ERROR_H
#define EMU_CUBLAS_ERROR_H

#include <emu/macro.h>

#include <fmt/core.h>
#include <cublas_v2.h>

namespace emu
{

namespace cublas
{
    using status_t = cublasStatus_t;

    /**
     * Aliases for cublas status codes
     *
     * @note unfortunately, this enum can't inherit from @ref status_t
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

    constexpr inline bool is_success(status_t error_code)  { return error_code == static_cast<status_t>(status::success); }
    constexpr inline bool is_failure(status_t error_code)  { return error_code != static_cast<status_t>(status::success); }

    std::string describe(status_t error_code);

    class runtime_error : public std::runtime_error
    {
    public:
        runtime_error(status_t error_code) :
            std::runtime_error(fmt::format("Cublas error : {}", describe(error_code))),
            code_(error_code)
        {}

        runtime_error(status_t error_code, const std::string& what_arg) :
            std::runtime_error(fmt::format("Cublas error : {}; {}", describe(error_code), what_arg)),
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
 * a @ref cublas::runtime_error exception is thrown
 *
 * @param status should be @ref cublas::status::success - otherwise an exception is thrown
 * @param message An extra description message to add to the exception
 */
inline void throw_if_error(cublas::status_t status, std::string message) noexcept(false)
{
	if (is_failure(status))
        // Put the throw code in another function with cold path.
        [&] () EMU_COLD_PATH { throw runtime_error(status, message); }();
}

/**
 * Does nothing - unless the status indicates an error, in which case
 * a @ref cublas::runtime_error exception is thrown
 *
 * @param status should be @ref cublas::status::success - otherwise an exception is thrown
 */
inline void throw_if_error(cublas::status_t status) noexcept(false)
{
	if (is_failure(status))
        // Put the throw code in another function with cold path.
        [&] () EMU_COLD_PATH { throw runtime_error(status); }();
}

} // namespace cublas

} // namespace emu

#endif //EMU_CUBLAS_ERROR_H