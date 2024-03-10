#pragma once

#include <cstddef>

namespace std
{

    template <typename T>
    struct optional;

} // namespace std


namespace std::experimental
{

    class layout_right;
    class layout_left;
    class layout_stride;

    template <class IndexType, size_t... Extents>
    class extents;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    class mdspan;

} // namespace std::experimental

namespace tl
{

    template <typename T>
    struct optional;

    template <typename T, typename E>
    struct expected;

} // namespace tl

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
