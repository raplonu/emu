/**
 * @file
 * @brief Defines utilities for pointer inspection and management.
 *
 * This file provides functions to query information about pointers, such as their
 * memory location, associated device, and memory region. It also defines a
 * mechanism for registering custom device finders to extend pointer introspection
 * capabilities to different memory spaces (e.g., host, CUDA devices).
 */
#pragma once

#include <emu/utility.hpp>
#include <emu/detail/dlpack_types.hpp>
#include <emu/error.hpp>
#include <emu/optional.hpp>
#include <emu/span.hpp>

#include <tl/optional.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <filesystem>
#include <cstddef>
#include <span>

namespace emu
{

    /**
     * @brief Describes the properties of a memory region pointed to.
     */
    struct pointer_descriptor
    {
        std::string location;                 ///< A string identifying the memory location (e.g., "host", "cuda:0").
        std::filesystem::perms permissions;   ///< The memory permissions (read, write, execute).
        std::span<byte> base_region;          ///< A span covering the entire memory region.
    };

    /**
     * @brief Retrieves the descriptor for the memory region containing the given pointer.
     * @param b_ptr A pointer to a byte in the memory region.
     * @return An optional containing the pointer_descriptor if the pointer is in a known
     *         memory region, otherwise an empty optional.
     */
    optional<pointer_descriptor> pointer_descritor_of(const byte* b_ptr);

    /**
     * @brief Retrieves the memory region associated with a given location identifier.
     * @param location A string view identifying the memory location (e.g., "[heap]").
     * @return An optional containing a span of bytes for the region if found,
     *         otherwise an empty optional.
     */
    optional<std::span<byte>> region_from_location(std::string_view location);

    /**
     * @brief Gets the DLPack device type for a given pointer.
     * @param ptr A byte pointer to the memory.
     * @return A result containing the dlpack::device_t on success, or an error if the
     *         device cannot be determined.
     */
    result<dlpack::device_t> get_device_of_pointer(const byte * ptr);

    /**
     * @brief Gets the DLPack device type for a given pointer.
     * @tparam T The type of the pointed-to object.
     * @param ptr A pointer to the memory.
     * @return A result containing the dlpack::device_t on success, or an error if the
     *         device cannot be determined.
     */
    template<typename T>
    result<dlpack::device_t> get_device_of_pointer(const T * ptr)
    {
        return get_device_of_pointer(reinterpret_cast<const byte *>(ptr));
    }

/**
 * @brief Namespace for implementation details.
 * @internal
 */
namespace detail
{
    /**
     * @brief A function type for engines that can find a device for a pointer.
     *
     * A device finder engine is a function that takes a byte pointer and returns
     * an optional dlpack::device_t. If the engine can determine the device, it
     * returns the device; otherwise, it returns an empty optional.
     */
    using device_finder_engine = std::function<optional<dlpack::device_t>(const byte* ptr)>;

    /**
     * @brief Registers a new device finder engine.
     *
     * The registered finders are used by get_device_of_pointer to determine the
     * device associated with a pointer.
     * @param finder The device finder engine to register.
     */
    void register_device_finder(device_finder_engine finder);

} // namespace detail


} // namespace emu

/**
 * @def EMU_REGISTER_DEVICE_FINDER(NAME, FUNC)
 * @brief A macro to register a device finder function.
 *
 * This macro creates a constructor function that registers the given device
 * finder engine. This ensures that the finder is available before main() is called.
 *
 * @param NAME The unique name for the registration function.
 * @param FUNC The device finder engine (a function or callable object) to register.
 */
#define EMU_REGISTER_DEVICE_FINDER(NAME, FUNC)             \
    extern "C" __attribute__ ((constructor)) void NAME() { \
        ::emu::detail::register_device_finder(FUNC);       \
    }
