#ifndef EMU_UTILITY_CUH
#define EMU_UTILITY_CUH

#include <emu/utility.h>
#include <emu/macro.cuh>

namespace emu
{
    EMU_DEVICE inline
    bool is_not_last_block() { return EMU_BID < (EMU_SIZE_G - 1); }

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
     * @return std::size_t thread_item_pos
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

#endif //EMU_UTILITY_CUH