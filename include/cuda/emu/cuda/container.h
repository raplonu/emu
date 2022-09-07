#ifndef EMU_CUDA_CUDA_CONTAINER_H
#define EMU_CUDA_CUDA_CONTAINER_H

#include <emu/container.h>

#include <emu/cuda/misc/location.h>

namespace emu::cuda
{

    template <typename T>
    using container_t = container::detail::container_t<T, location::cuda_t, mdspan::default_accessor<T>>;

} // namespace emu::cuda

#endif // EMU_CUDA_CUDA_CONTAINER_H