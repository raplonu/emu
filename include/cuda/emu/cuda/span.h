#pragma once

#include <emu/span.h>
#include <emu/cuda/location.h>

namespace emu::cuda
{

    /**
     * @brief The class template span describes an object that can refer to a contiguous sequence of objects
     * with the first element of the sequence at position zero.
     *
     * A span can either have a static extent, in which case the number of elements in the sequence is known
     * and encoded in the type, or a dynamic extent.
     * Please refer to https://en.cppreference.com/w/cpp/container/span for more detail.
     *
     */
    template<typename ElementType, std::size_t Extent = std::dynamic_extent>
    using span = emu::detail::span<ElementType, cuda::location::device, Extent>;

} // namespace emu::cuda
