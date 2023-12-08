#pragma once

#include <cstddef>

namespace std::experimental
{

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    class mdspan;

} // namespace std::experimental

namespace emu
{

    template<typename T, typename F>
    struct scoped;

namespace cuda
{

    template<typename T, std::size_t Extent>
    struct span;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdspan;

} // namespace cuda

namespace spe
{
    // see emu/info.hpp
    template <typename T>
    struct info_t;

    // see emu/functor.hpp
    template<typename From>
    struct map;

} // namespace spe

    template<typename T>
    constexpr auto rank(const T&);

} // namespace emu
