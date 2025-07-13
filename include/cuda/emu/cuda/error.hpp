#pragma once

#include <emu/error.hpp>

#include <cuda_runtime_api.h>
#include <cuda.h>

namespace emu
{

namespace cuda
{

    using status_t = cudaError_t;

    EMU_GENERATE_ERROR_CATEGORY(error_category, "cuda", ec) {
        return cudaGetErrorString(static_cast<status_t>(ec));
    }

    EMU_GENERATE_ERROR_EXCEPTION(status_t, runtime_error, std::runtime_error, error_category);

} // namespace cuda

    EMU_GENERATE_ERROR_FUNCTION(cuda::status_t, cuda::error_category, cuda::runtime_error);

} // namespace emu

#define EMU_CUDA_CHECK_RETURN_EC(expr) \
    EMU_CHECK_OR_RETURN_EC(expr)

#define EMU_CUDA_CHECK_RETURN_UN_EC(expr) \
    EMU_CHECK_OR_RETURN_UN_EC(expr)

#define EMU_CUDA_CHECK_OR_THROW(expr) \
    EMU_CHECK_OR_THROW(expr)

#define EMU_CUDA_CHECK_OR_THROW_WHAT(expr, WHAT) \
    EMU_CHECK_OR_THROW_WHAT(expr, WHAT)
