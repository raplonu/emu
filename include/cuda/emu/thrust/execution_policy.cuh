#ifndef EMU_THRUST_EXECUTION_POLICY_CUH
#define EMU_THRUST_EXECUTION_POLICY_CUH

#include <emu/macro.h>

#include <cuda/api_wrappers.hpp>

#include <thrust/execution_policy.h>

namespace emu
{

namespace thrust
{

namespace detail
{

    struct execute_on_stream_async :
        ::thrust::cuda_cub::execute_on_stream_base<execute_on_stream_async>
    {
        using base_t = ::thrust::cuda_cub::execute_on_stream_base<execute_on_stream_async>;

        using base_t::base_t;

        friend EMU_HODE
        ::cuda::status_t synchronize_stream(execute_on_stream_async&) noexcept
        {
            return cudaSuccess;
        }
    };

    struct par_t : ::thrust::cuda_cub::par_t {
        using base_t = ::thrust::cuda_cub::par_t;

        using base_t::base_t;
        using base_t::stream_attachment_type;

        stream_attachment_type on(const ::cuda::stream_t & stream) const
        {
            stream.device().make_current();
            return base_t::on(stream.id());
        }
    };

    struct par_async_t {

        execute_on_stream_async on(const ::cuda::stream_t & stream) const
        {
            stream.device().make_current();
            return execute_on_stream_async(stream.id());
        }

    };

} // detail

    extern detail::par_t par;
    extern detail::par_async_t par_async;

} // namespace thrust

} // namespace emu

#endif //EMU_THRUST_EXECUTION_POLICY_CUH