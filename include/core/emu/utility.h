#pragma once

#include <emu/config.h>
#include <emu/macro.h>
#include <emu/type_traits.h>

#include <type_traits>
#include <algorithm>
#include <functional>
#include <utility>
#include <memory>

#define EMU_FWD(v) ::emu::fwd<decltype(v)>(v)

namespace emu
{

    template<typename T>
    constexpr T&& fwd(std::remove_reference_t<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr T&& fwd(std::remove_reference_t<T>&& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr auto mv(T&& t) noexcept -> std::remove_reference_t<T>&& {
        return static_cast<std::remove_reference_t<T>&&>(t);
    }

  //###################### SIZE ########################

    /**
    * Calculate the ceil result of a / b
    * @param  a numerator
    * @param  b denominator
    * @return   ceil division of a / b
    */
    template <typename T>
    EMU_HODE constexpr
    T ceil(T a, T b) noexcept { return (a + b - 1) / b; }

    template<typename T>
    EMU_HODE constexpr
    T next_mul(T a, T b) { return ( (a - 1) / b + 1) * b; }

    template <typename T>
    EMU_HODE constexpr
    T align(T size) noexcept { return next_mul(size, 32); }

    template<std::size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & min(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t1 : t2;
    }

    template<std::size_t key_id, typename Tuple>
    EMU_HODE constexpr
    const Tuple & max(const Tuple & t1, const Tuple & t2) noexcept {
        using std::get;
        return get<key_id>(t1) < get<key_id>(t2) ? t2 : t1;
    }

    /**
     * @brief Return the generalized number of item per group. This number is the same for every group.
     * The sum of the result for every block may be equal or superior of item_nb
     * Esamples :
     * item_per_group_local_nb(100, 10) -> 10
     * item_per_group_local_nb(89, 10) -> 9
     * @param item_nb
     * @param group_size
     * @return int item_per_group_nb
     */
    EMU_HODE inline
    int item_per_group_nb(int item_nb, int group_size) { return ceil(item_nb, group_size); }

    /**
     * @brief Share a resource between a group. Guaranty to minimize the number of resource
     * for each group element and to handle exactly the right number of available item
     * Examples :
     * Item are perfectly shared between all groups
     * item_per_group_local_nb(100, [0, .., 9], 10) -> [10, .., 10]
     * The last group handle only 1 element to unsure item is not handled two times
     * item_per_group_local_nb(7, [0, 1, 2, 3], 4) -> [2, 2, 2, 1]
     * The last group remain idle because the is not element at all to handle
     * item_per_group_local_nb(5, [0, 1, 2, 3], 4) -> [2, 2, 1, 0]
     *
     * @param item_nb Total number of item to handle
     * @param group_id The id of the current group
     * @param group_size The number of element in the group
     * @return int item_per_group_local_nb
     */
    EMU_HODE inline
    int item_per_group_local_nb(int item_nb, int group_id, int group_size) {
        int item_pg = item_per_group_nb(item_nb, group_size);

        return std::max(0, std::min(item_nb, (group_id + 1) * item_pg) - group_id * item_pg);
    }

    template<typename T, typename Alloc>
    constexpr auto rebind_alloc(const Alloc& alloc) {
        return typename std::allocator_traits<Alloc>::template rebind_alloc<T>(alloc);
    }

} // namespace emu
