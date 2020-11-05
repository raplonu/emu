#ifndef EMU_ITERATOR_COORDINATE_H
#define EMU_ITERATOR_COORDINATE_H

#include <emu/functional/misc.h>
#include <emu/functional/misc.h>
#include <emu/iterator/function.cuh>

#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/permutation_iterator.h>

namespace emu
{

namespace iterator
{

    template<typename SubApItX, typename SubApItY>
    auto make_xy_pos_iterator(SubApItX sub_ap_x, SubApItY sub_ap_y, std::size_t sub_ap_size_x, std::size_t sub_ap_size_y) {
        auto count        = thrust::make_counting_iterator(0);

        auto sub_ap_id    = emu::iterator::make_transform_iterator(functional::div_by(sub_ap_size_x * sub_ap_size_y), count);
        auto sub_ap_pos   = emu::iterator::make_transform_iterator(functional::mod_by(sub_ap_size_x * sub_ap_size_y), count);

        auto sub_ap_id_x  = thrust::make_permutation_iterator(sub_ap_x, sub_ap_id);
        auto sub_ap_id_y  = thrust::make_permutation_iterator(sub_ap_y, sub_ap_id);

        auto sub_ap_pos_x = emu::iterator::make_transform_iterator(functional::mod_by(sub_ap_size_x), count);
        auto sub_ap_pos_y = emu::iterator::make_transform_iterator(functional::div_by(sub_ap_size_x), sub_ap_pos);

        auto it_x         = emu::iterator::make_transform_iterator(functional::Adder{}, sub_ap_id_x, sub_ap_pos_x);
        auto it_y         = emu::iterator::make_transform_iterator(functional::Adder{}, sub_ap_id_y, sub_ap_pos_y);

        return thrust::make_zip_iterator(thrust::make_tuple(it_x, it_y));
    }

    // template<typename SubApItX, typename SubApItY, typename SubApPosItX, typename SubApPosItY>
    // auto make_sh_xy_pos(SubApItX sub_ap_x, SubApItY sub_ap_y, SubApPosItX sub_ap_size_x, SubApPosItY sub_ap_size_y, std::size_t sub_ap_size) {
    //     auto count        = thrust::make_counting_iterator(0);

    //     auto sub_ap_id    = emu::make_transform_iterator(div_by(sub_ap_size), count);
    //     auto sub_ap_pos   = emu::make_transform_iterator(mod_by(sub_ap_size), count);

    //     auto sub_ap_id_x  = thrust::make_permutation_iterator(sub_ap_x, sub_ap_id);
    //     auto sub_ap_id_y  = thrust::make_permutation_iterator(sub_ap_y, sub_ap_id);

    //     auto sub_ap_pos_x = emu::make_transform_iterator(mod_by(sub_ap_size_x), count);
    //     auto sub_ap_pos_y = emu::make_transform_iterator(div_by(sub_ap_size_x), sub_ap_pos);

    //     auto it_x         = emu::make_transform_iterator(Adder{}, sub_ap_id_x, sub_ap_pos_x);
    //     auto it_y         = emu::make_transform_iterator(Adder{}, sub_ap_id_y, sub_ap_pos_y);

    //     return thrust::make_zip_iterator(thrust::make_tuple(it_x, it_y));
    // }



} // namespace iterator


} // namespace emu

#endif //EMU_ITERATOR_COORDINATE_H