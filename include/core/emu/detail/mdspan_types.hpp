#pragma once

#include <span>
#include <experimental/mdspan>

namespace emu
{

    using std::dynamic_extent;

    namespace stdex = std::experimental;

    using stdex::layout_right;
    using stdex::layout_left;
    using stdex::layout_stride;

    using layout_c = layout_right;
    using layout_f = layout_left;

    using layout_row    = layout_right;
    using layout_column = layout_left;

    using stdex::default_accessor;

    using stdex::dextents;

    using stdex::extents;

    using stdex::full_extent_t;
    using stdex::full_extent;

    using stdex::strided_slice;

    template<std::size_t N>
    using dims = dextents<std::size_t, N>;

    using d0 = dims<0>;
    using d1 = dims<1>;
    using d2 = dims<2>;
    using d3 = dims<3>;
    using d4 = dims<4>;

} // namespace emu
