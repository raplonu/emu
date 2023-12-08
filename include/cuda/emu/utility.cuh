#pragma once

#include <emu/utility.hpp>
#include <emu/macro.cuh>
#include <emu/type_traits.hpp>

namespace emu
{
    // template <typename T>
    // EMU_HODE constexpr T ceil    (T a, T b) noexcept { return (a + b - 1) / b; }

    // template<typename T>
    // EMU_HODE constexpr T next_mul(T a, T b) noexcept { return ( (a - 1) / b + 1) * b; }

    template <typename T>
    EMU_HODE constexpr T max     (T a, T b) noexcept { return (a < b) ? b : a; }

    template <typename T>
    EMU_HODE constexpr T min     (T a, T b) noexcept { return (a < b) ? a : b; }

    template <typename T>
    EMU_HODE constexpr
    T align(T size) noexcept { return next_mul(size, 32); }

    EMU_DEVICE inline bool is_not_last_block() { return EMU_BID < (EMU_SIZE_G - 1); }

    /**
     * @brief Return the generalized number of item per group. This number is the same for every group.
     * The sum of the result for every block may be equal or superior of intemNb
     * Esamples :
     * item_per_group_local_nb(100, 10) -> 10
     * item_per_group_local_nb(89, 10) -> 9
     * @param item_nb
     * @param group_size
     * @return std::size_t item_per_group_nb
     */
    EMU_DEVICE inline
    std::size_t item_per_group_nb(std::size_t item_nb, std::size_t group_size) { return ceil(item_nb, group_size); }

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
     * @return std::size_t item_per_group_local_nb
     */
    EMU_DEVICE inline
    std::size_t item_per_group_local_nb(std::size_t item_nb, std::size_t group_id, std::size_t group_size) {
        std::size_t item_pg = item_per_group_nb(item_nb, group_size);

        return max(0, int(min(item_nb, std::size_t((group_id + 1) * item_pg))) - int(group_id * item_pg));
    }

    /**
     * @brief Return the generalized number of item per kernel blocks
     *
     * @param item_nb
     * @return std::size_t item_per_block_nb
     */
    EMU_DEVICE inline
    std::size_t item_per_block_nb(std::size_t item_nb) { return item_per_group_nb(item_nb, EMU_SIZE_G); }

    /**
     * @brief Return the specific number of item per kernel blocks
     *
     * @param item_nb
     * @return std::size_t item_per_block_nb
     */
    EMU_DEVICE inline
    std::size_t item_per_block_local_nb(std::size_t item_nb) { return item_per_group_local_nb(item_nb, EMU_BID, EMU_SIZE_G); }

    /**
     * @brief Get the position of thread considering custom_item_per_block_nb item per block.
     * 2 threads in 2 group can share same position. Need to test the validity of a thread
     *
     * @param custom_item_per_block_nb
     * @return std::size_t thread_item_pos_custom
     */
    EMU_DEVICE inline
    std::size_t thread_item_pos_custom(std::size_t custom_item_per_block_nb) { return EMU_TID + EMU_BID * custom_item_per_block_nb; }

    /**
     * @brief Get the position of a thread considering a total number of item to be shared trough the GPU
     * 2 threads in 2 group can share same position. Need to test the validity of a thread
     *
     * @param item_nb
     * @return std::size_t thread_item_posz
     */
    EMU_DEVICE inline
    std::size_t thread_item_pos(std::size_t item_nb) { return thread_item_pos_custom(item_per_block_nb(item_nb)); }

    /**
     * @brief Test if a thread have items to handle
     *
     * @param item_nb
     * @param item_per_block_nb
     * @return bool is_active
     */
    EMU_DEVICE inline
    bool is_active(std::size_t item_nb, std::size_t item_per_block_nb) {
        return (thread_item_pos_custom(item_per_block_nb) < item_nb) && (EMU_TID < item_per_block_nb);
    }

    /**
     * @brief Test if a thread have items to handle
     *
     * @param item_nb
     * @return bool is_active
     */
    EMU_DEVICE inline
    bool is_active(std::size_t item_nb) {
        return is_active(item_nb, item_per_block_nb(item_nb));
    }

}
