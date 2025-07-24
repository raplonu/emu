#pragma once

#include <emu/error/generation.hpp>

#include <cusolverDn.h>

namespace emu
{

namespace cusolver
{
    using status_t = cusolverStatus_t;

    EMU_GENERATE_ERROR_CATEGORY(error_category, "cusolver", ec) {
        int nth = static_cast<int>(ec);

        // More about this strange convention: https://docs.nvidia.com/cuda/cusolver/index.html#convention-of-info
        return fmt::format("Error, invalid argument at {}", nth);
    }

    EMU_GENERATE_ERROR_EXCEPTION(status_t, runtime_error, std::runtime_error, error_category);

} // namespace cusolver

    EMU_GENERATE_ERROR_FUNCTION(cusolver::status_t, cusolver::error_category, cusolver::runtime_error);

} // namespace emu
