#ifndef EMU_CUDA_MDSPAN_H
#define EMU_CUDA_MDSPAN_H

#include <emu/mdspan.h>
#include <emu/cuda/misc/location.h>

namespace emu
{

namespace cuda
{

    template <typename ElementType, typename Extents, typename LayoutPolicy = mdspan::layout_right, typename AccessorPolicy = mdspan::default_accessor<ElementType>>
    using mdspan_t = mdspan::detail::mdspan_t<ElementType, location::cuda_t, Extents, LayoutPolicy, AccessorPolicy>;

    template<typename ElementType> using mdspan_1d_t = mdspan_t<ElementType, mdspan::dextents<1>>;
    template<typename ElementType> using mdspan_2d_t = mdspan_t<ElementType, mdspan::dextents<2>>;
    template<typename ElementType> using mdspan_3d_t = mdspan_t<ElementType, mdspan::dextents<3>>;

    template<typename ElementType> using mdspan_1d_c_t = mdspan_t<ElementType, mdspan::dextents<1>>;
    template<typename ElementType> using mdspan_2d_c_t = mdspan_t<ElementType, mdspan::dextents<2>>;
    template<typename ElementType> using mdspan_3d_c_t = mdspan_t<ElementType, mdspan::dextents<3>>;

    template<typename ElementType> using mdspan_1d_f_t = mdspan_t<ElementType, mdspan::dextents<1>, mdspan::layout_f>;
    template<typename ElementType> using mdspan_2d_f_t = mdspan_t<ElementType, mdspan::dextents<2>, mdspan::layout_f>;
    template<typename ElementType> using mdspan_3d_f_t = mdspan_t<ElementType, mdspan::dextents<3>, mdspan::layout_f>;

    template<typename ElementType> using mdspan_1d_s_t = mdspan_t<ElementType, mdspan::dextents<1>, mdspan::layout_stride>;
    template<typename ElementType> using mdspan_2d_s_t = mdspan_t<ElementType, mdspan::dextents<2>, mdspan::layout_stride>;
    template<typename ElementType> using mdspan_3d_s_t = mdspan_t<ElementType, mdspan::dextents<3>, mdspan::layout_stride>;

} // namespace cuda

} // namespace emu

#endif //EMU_CUDA_MDSPAN_H