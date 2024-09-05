#pragma once

#include <emu/error.hpp>

#include <cuda/api/error.hpp>

namespace emu
{

namespace cuda
{

    struct error_category: public std::error_category
    {
        const char * name() const noexcept;
        std::string message( int ev ) const;

        static std::error_category const& instance();
    };


} // namespace cuda

    std::error_code make_error_code( CUresult e );
    std::error_code make_error_code( cudaError_t e );
    std::error_code make_error_code( ::cuda::status::named_t e );

    unexpected<std::error_code> make_unexpected( CUresult e );
    unexpected<std::error_code> make_unexpected( cudaError_t e );
    unexpected<std::error_code> make_unexpected( ::cuda::status::named_t e );

    void throw_error( CUresult e );
    void throw_error( cudaError_t e );
    void throw_error( ::cuda::status::named_t e );

} // namespace emu

#define EMU_CHECK_RETURN_UN_EC(...) \
    do { auto status__ = __VA_ARGS__; EMU_TRUE_OR_RETURN_UN_EC(status__ == cudaSuccess, status__); } while(false)

#define EMU_CHECK_THROW(...) \
    do { auto status__ = __VA_ARGS__; EMU_TRUE_OR_THROW_ERROR(status__ == cudaSuccess, status__); } while(false)
