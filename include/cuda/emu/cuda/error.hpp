#pragma once

#include <emu/error.hpp>

#include <cuda/api/error.hpp>

namespace emu
{

namespace cuda
{

    struct error_category: public std::error_category
    {
        [[nodiscard]] std::string message( int ev ) const override;
        [[nodiscard]] const char * name() const noexcept override;

        static std::error_category const& instance();
    };


} // namespace cuda

    std::error_code make_error_code( CUresult e );
    std::error_code make_error_code( cudaError_t e );
    std::error_code make_error_code( ::cuda::status::named_t e );

    unexpected<std::error_code> make_unexpected( CUresult e );
    unexpected<std::error_code> make_unexpected( cudaError_t e );
    unexpected<std::error_code> make_unexpected( ::cuda::status::named_t e );

    [[noreturn]] void throw_error( CUresult e );
    [[noreturn]] void throw_error( cudaError_t e );
    [[noreturn]] void throw_error( ::cuda::status::named_t e );

} // namespace emu

#define EMU_CUDA_CHECK_RETURN_UN_EC(expr) \
    EMU_SUCCESS_OR_RETURN_UN_EC(expr, cudaSuccess)

#define EMU_CUDA_CHECK_RETURN_UN_EC_LOG(expr, ...) \
    EMU_SUCCESS_OR_RETURN_UN_EC_LOG(expr, cudaSuccess, __VA_ARGS__)

#define EMU_CUDA_CHECK_THROW_ERROR(expr) \
    EMU_SUCCESS_OR_THROW(expr, cudaSuccess)

#define EMU_CUDA_CHECK_THROW_ERROR_LOG(expr, ...) \
    EMU_SUCCESS_OR_THROW_LOG(expr, cudaSuccess, __VA_ARGS__)
