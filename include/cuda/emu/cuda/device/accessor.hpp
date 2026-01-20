#pragma once

#include <cuda/mdspan>

namespace emu::cuda
{

namespace device
{

    template <typename Accessor>
    using accessor = ::cuda::device_accessor<Accessor>;

} // namespace device


namespace cpts
{

    template <typename T>
    concept device_accessor = ::cuda::is_device_accessor_v<T>;

    template <typename T>
    concept device_accessible = ::cuda::is_device_accessible_v<T>;

} // namespace cpts


} // namespace emu::cuda
