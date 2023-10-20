// #ifndef EMU_ITERATOR_COORDINATE_H
// #define EMU_ITERATOR_COORDINATE_H

// #include <emu/functional/misc.h>
// #include <emu/iterator/function.cuh>

// #include <thrust/iterator/counting_iterator.h>
// #include <thrust/iterator/permutation_iterator.h>

// namespace emu
// {

// namespace iterator
// {

//     // struct Coordinate
//     // {
//     //     SubApItX sub_ap_x;
//     //     SubApItY sub_ap_y;

//     //     constexpr EMU_HODE
//     //     auto operator()(std::size_t sub_ap_id, std::size_t sub_ap_pos) const noexcept {
//     //         auto sub_ap_id_x  = sub_ap_x[sub_ap_id];
//     //         auto sub_ap_id_y  = sub_ap_y[sub_ap_id];

//     //         auto sub_ap_pos_x =
//     //         auto sub_ap_pos_y =
//     //         return thrust::make_tuple();
//     //     }
//     // };

//     template<typename SubApItX, typename SubApItY>
//     struct Coordinate
//     {
//         SubApItX    sub_ap_x;
//         SubApItY    sub_ap_y;
//         std::size_t sub_ap_size_x;
//         std::size_t sub_ap_size;


//         constexpr EMU_HODE
//         auto operator()(std::size_t id) const noexcept {
//             auto sub_ap_id    = id / sub_ap_size;
//             auto sub_ap_pos   = id % sub_ap_size;
//             auto sub_ap_id_x  = sub_ap_x[sub_ap_id];
//             auto sub_ap_id_y  = sub_ap_y[sub_ap_id];
//             auto sub_ap_pos_x = id % sub_ap_size_x;
//             auto sub_ap_pos_y = sub_ap_pos / sub_ap_size_x;
//             auto it_x         = sub_ap_id_x + sub_ap_pos_x;
//             auto it_y         = sub_ap_id_y + sub_ap_pos_y;

//             return thrust::make_tuple(it_x, it_y);
//         }
//     };

//     template<typename SubApItX, typename SubApItY>
//     constexpr EMU_HODE
//     auto make_coordinate(SubApItX sub_ap_x, SubApItY sub_ap_y, std::size_t sub_ap_size_x, std::size_t sub_ap_size_y) noexcept {
//         return Coordinate<SubApItX, SubApItY>{sub_ap_x, sub_ap_y, sub_ap_size_x, sub_ap_size_x * sub_ap_size_y};
//     }


//     /**
//      * @brief Generate an iterator of tuple that contains the x and y position of each valid pixel accross the different subaperture.
//      *
//      * @tparam SubApItX     RandomAccessIterator of integral type.
//      * @tparam SubApItY     RandomAccessIterator of integral type.
//      * @param sub_ap_x      Iterator of x position of each subaperture.
//      * @param sub_ap_y      Iterator of y position of each subaperture.
//      * @param sub_ap_size_x Size of a subaperture along x.
//      * @param sub_ap_size_y Size of a subaperture along y.
//      * @return iterator of tuple containing x and y value of each valid pixel.
//      */
//     template<typename SubApItX, typename SubApItY>
//     EMU_HODE constexpr
//     auto make_xy_pos_iterator(SubApItX sub_ap_x, SubApItY sub_ap_y, std::size_t sub_ap_size_x, std::size_t sub_ap_size_y) noexcept {
//         // const auto sub_ap_size = sub_ap_size_x * sub_ap_size_y;

//         auto count        = thrust::make_counting_iterator(0);

//         // auto sub_ap_id    = emu::iterator::make_transform_iterator(functional::div_by(sub_ap_size), count);
//         // auto sub_ap_pos   = emu::iterator::make_transform_iterator(functional::mod_by(sub_ap_size), count);

//         // auto sub_ap_id_x  = thrust::make_permutation_iterator(sub_ap_x, sub_ap_id);
//         // auto sub_ap_id_y  = thrust::make_permutation_iterator(sub_ap_y, sub_ap_id);

//         // auto sub_ap_pos_x = emu::iterator::make_transform_iterator(functional::mod_by(sub_ap_size_x), count);
//         // auto sub_ap_pos_y = emu::iterator::make_transform_iterator(functional::div_by(sub_ap_size_x), sub_ap_pos);

//         // auto it_x         = emu::iterator::make_transform_iterator(functional::Adder{}, sub_ap_id_x, sub_ap_pos_x);
//         // auto it_y         = emu::iterator::make_transform_iterator(functional::Adder{}, sub_ap_id_y, sub_ap_pos_y);

//         // return thrust::make_zip_iterator(thrust::make_tuple(it_x, it_y));

//         return emu::iterator::make_transform_iterator(make_coordinate(sub_ap_x, sub_ap_y, sub_ap_size_x, sub_ap_size_y), count);
//     }



//     // template<typename SubApItX, typename SubApItY, typename SubApPosItX, typename SubApPosItY>
//     // auto make_sh_xy_pos(SubApItX sub_ap_x, SubApItY sub_ap_y, SubApPosItX sub_ap_size_x, SubApPosItY sub_ap_size_y, std::size_t sub_ap_size) {
//     //     auto count        = thrust::make_counting_iterator(0);

//     //     auto sub_ap_id    = emu::make_transform_iterator(div_by(sub_ap_size), count);
//     //     auto sub_ap_pos   = emu::make_transform_iterator(mod_by(sub_ap_size), count);

//     //     auto sub_ap_id_x  = thrust::make_permutation_iterator(sub_ap_x, sub_ap_id);
//     //     auto sub_ap_id_y  = thrust::make_permutation_iterator(sub_ap_y, sub_ap_id);

//     //     auto sub_ap_pos_x = emu::make_transform_iterator(mod_by(sub_ap_size_x), count);
//     //     auto sub_ap_pos_y = emu::make_transform_iterator(div_by(sub_ap_size_x), sub_ap_pos);

//     //     auto it_x         = emu::make_transform_iterator(Adder{}, sub_ap_id_x, sub_ap_pos_x);
//     //     auto it_y         = emu::make_transform_iterator(Adder{}, sub_ap_id_y, sub_ap_pos_y);

//     //     return thrust::make_zip_iterator(thrust::make_tuple(it_x, it_y));
//     // }


//     template<typename SubApItX, typename SubApItY>
//     EMU_HODE
//     auto make_pos_iterator(SubApItX sub_ap_x, SubApItY sub_ap_y, std::size_t sub_ap_size_x, std::size_t sub_ap_size_y, std::size_t frame_size) noexcept {
//         // x & y position of each valid pixels
//         auto xy_pos = emu::iterator::make_xy_pos_iterator(sub_ap_x, sub_ap_y, sub_ap_size_x, sub_ap_size_y);

//         // pixel position of each valid pixels
//         return emu::iterator::make_transform_iterator(emu::functional::make_apply(emu::functional::xy_to_pixel_pos{frame_size}), xy_pos);
//     }

// } // namespace iterator


// } // namespace emu

// #endif //EMU_ITERATOR_COORDINATE_H