#ifndef EMU_UTILS_H
#define EMU_UTILS_H

#include <emu/macro.h>

#include <type_traits>

#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/zip_iterator.h>

namespace emu
{
    template<typename T>
    constexpr T&& fwd(typename std::remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }    

    template<typename It1, typename It2, typename BinaryOp>
    EMU_HODE auto make_binary_transform(It1 it1, It2 it2, BinaryOp binaryOp) {
        return thrust::make_transform_iterator(
            thrust::make_zip_iterator(
                thrust::make_tuple(it1, it2)),
                [binaryOp](/*thrust::tuple<T1, T2>*/ auto t) {
                    return binaryOp(thrust::get<0>(t), thrust::get<1>(t));
                });
    }
}

#endif //EMU_UTILS_H