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

    struct capsule;

    template<typename T, typename F>
    struct scoped;

namespace detail
{

    template<typename ElementType, std::size_t Extent, typename LocationPolicy, typename ActualType>
    struct basic_span;

    template<typename ElementType, std::size_t Extent, typename LocationPolicy, typename ActualType>
    struct basic_container;

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename LocationPolicy, typename ActualType>
    struct basic_mdspan;

    template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename LocationPolicy, typename ActualType>
    struct basic_mdcontainer;

} // namespace detail

    template<typename ElementType, std::size_t Extent>
    struct container;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer;

namespace host
{

    template<typename ElementType, std::size_t Extent>
    struct span;

    template<typename ElementType, std::size_t Extent>
    struct container;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdspan;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer;

} // namespace host

namespace cuda::device
{

    template<typename ElementType, std::size_t Extent>
    struct span;

    template<typename ElementType, std::size_t Extent>
    struct container;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdspan;

    template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
    struct mdcontainer;

} // namespace cuda::device

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
