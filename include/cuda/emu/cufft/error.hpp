#pragma once

#include <emu/error/generation.hpp>
#include <emu/assert.hpp>

#include <cufft.h>

namespace emu
{

namespace cufft
{
    using status_t = cufftResult;

    inline const char* describe(status_t error_code) {
        switch (error_code) {
            case CUFFT_SUCCESS:
                return "The cuFFT operation was successful";
            case CUFFT_INVALID_PLAN:
                return "cuFFT was passed an invalid plan handle";
            case CUFFT_ALLOC_FAILED:
                return "cuFFT failed to allocate GPU or CPU memory";
            case CUFFT_INVALID_TYPE:
                return "No longer used";
            case CUFFT_INVALID_VALUE:
                return "User specified an invalid pointer or parameter";
            case CUFFT_INTERNAL_ERROR:
                return "Driver or internal cuFFT library error";
            case CUFFT_EXEC_FAILED:
                return "Failed to execute an FFT on the GPU";
            case CUFFT_SETUP_FAILED:
                return "The cuFFT library failed to initialize";
            case CUFFT_INVALID_SIZE:
                return "User specified an invalid transform size";
            case CUFFT_UNALIGNED_DATA:
                return "No longer used";
            case CUFFT_INCOMPLETE_PARAMETER_LIST:
                return "Missing parameters in call";
            case CUFFT_INVALID_DEVICE:
                return "Execution of a plan was on different GPU than plan creation";
            case CUFFT_PARSE_ERROR:
                return "Internal plan database error";
            case CUFFT_NO_WORKSPACE:
                return "No workspace has been provided prior to plan execution";
            case CUFFT_NOT_IMPLEMENTED:
                return "Function does not implement functionality for parameters given.";
            case CUFFT_LICENSE_ERROR:
                return "Used in previous versions.";
            case CUFFT_NOT_SUPPORTED:
                return "Operation is not supported for parameters given.";
        }
        EMU_UNREACHABLE;
    }

    EMU_GENERATE_ERROR_CATEGORY(error_category, "cufft", ec) {
        return describe(static_cast<status_t>(ec));
    }

    EMU_GENERATE_ERROR_EXCEPTION(status_t, runtime_error, std::runtime_error, error_category);

} // namespace cufft

    EMU_GENERATE_ERROR_FUNCTION(cufft::status_t, cufft::error_category, cufft::runtime_error);

} // namespace emu
