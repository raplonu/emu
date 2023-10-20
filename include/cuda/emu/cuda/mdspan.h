#pragma once

#include <emu/mdspan.h>
#include <emu/cuda/location.h>

namespace emu
{

namespace cuda
{

    template <typename ElementType, typename Extents, typename LayoutPolicy = layout_right, typename AccessorPolicy = default_accessor<ElementType>>
    using mdspan = emu::detail::mdspan<ElementType, cuda::location::device, Extents, LayoutPolicy, AccessorPolicy>;

    template<typename ElementType> using mdspan_1d = mdspan<ElementType, dextents<std::size_t, 1>>;
    template<typename ElementType> using mdspan_2d = mdspan<ElementType, dextents<std::size_t, 2>>;
    template<typename ElementType> using mdspan_3d = mdspan<ElementType, dextents<std::size_t, 3>>;

    template<typename ElementType> using mdspan_1d_c = mdspan<ElementType, dextents<std::size_t, 1>>;
    template<typename ElementType> using mdspan_2d_c = mdspan<ElementType, dextents<std::size_t, 2>>;
    template<typename ElementType> using mdspan_3d_c = mdspan<ElementType, dextents<std::size_t, 3>>;

    template<typename ElementType> using mdspan_1d_f = mdspan<ElementType, dextents<std::size_t, 1>, layout_f>;
    template<typename ElementType> using mdspan_2d_f = mdspan<ElementType, dextents<std::size_t, 2>, layout_f>;
    template<typename ElementType> using mdspan_3d_f = mdspan<ElementType, dextents<std::size_t, 3>, layout_f>;

    template<typename ElementType> using mdspan_1d_s = mdspan<ElementType, dextents<std::size_t, 1>, layout_stride>;
    template<typename ElementType> using mdspan_2d_s = mdspan<ElementType, dextents<std::size_t, 2>, layout_stride>;
    template<typename ElementType> using mdspan_3d_s = mdspan<ElementType, dextents<std::size_t, 3>, layout_stride>;

} // namespace cuda

} // namespace emu
