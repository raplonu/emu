#pragma once

#include <cuda/mdspan>

namespace emu::cuda
{

namespace managed
{

    template <typename Accessor>
    using accessor = ::cuda::managed_accessor<Accessor>;

} // namespace managed

namespace cpts
{

    template <typename T>
    concept managed_accessor = ::cuda::is_managed_accessor_v<T>;

    template <typename T>
    concept managed_accessible = ::cuda::is_managed_accessible_v<T>;

} // namespace cpts

} // namespace emu::cuda
