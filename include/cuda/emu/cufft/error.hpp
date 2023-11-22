#pragma once

#include <emu/detail/error.hpp>

#include <cufft.h>

namespace emu::cufft
{
    using status_t = cufftResult;

    enum status : std::underlying_type<status_t>::type
    {
        success = CUFFT_SUCCESS,
        invalid_plan = CUFFT_INVALID_PLAN,
        alloc_failed = CUFFT_ALLOC_FAILED,
        invalid_type = CUFFT_INVALID_TYPE,
        invalid_value = CUFFT_INVALID_VALUE,
        internal_error = CUFFT_INTERNAL_ERROR,
        exec_failed = CUFFT_EXEC_FAILED,
        setup_failed = CUFFT_SETUP_FAILED,
        invalid_size = CUFFT_INVALID_SIZE,
        unaligned_data = CUFFT_UNALIGNED_DATA,
        incomplete_parameter_list = CUFFT_INCOMPLETE_PARAMETER_LIST,
        invalid_device = CUFFT_INVALID_DEVICE,
        parse_error = CUFFT_PARSE_ERROR,
        no_workspace = CUFFT_NO_WORKSPACE,
        not_implemented = CUFFT_NOT_IMPLEMENTED,
        license_error = CUFFT_LICENSE_ERROR,
        not_supported = CUFFT_NOT_SUPPORTED
    };

    std::string describe(status_t error_code);

    EMU_GENERATE_ERROR_HANDLER("Cufft", runtime_error, std::runtime_error, status_t, status::success, describe)

} // namespace emu::cufft
