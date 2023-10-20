#pragma once

#include <cstddef>

namespace std::experimental
{

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    class mdspan;

} // namespace std::experimental

namespace emu
{

namespace location
{

    struct host;

} // namespace location

    template<typename T, typename F>
    struct scoped;

namespace detail
{

    template<typename T, typename Location, std::size_t Extent>
    struct span;

    template <typename ElementType, typename Location, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdspan;

} // namespace detail

namespace cuda::location
{

    struct device;

} // namespace cuda::location

} // namespace emu
