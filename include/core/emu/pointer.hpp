#pragma once

#include <emu/utility.hpp>
#include <emu/detail/dlpack_types.hpp>
#include <emu/error.hpp>
#include <emu/optional.hpp>

#include <functional>

namespace emu
{

namespace detail
{
    using device_finder_engine = std::function<optional<dlpack::device_t>(const byte *)>;

    void register_device_finder(device_finder_engine finder);

} // namespace detail

    result<dlpack::device_t> get_device_of_pointer(const byte * ptr);

    template<typename T>
    result<dlpack::device_t> get_device_of_pointer(const T * ptr)
    {
        return get_device_of_pointer(reinterpret_cast<const byte *>(ptr));
    }

} // namespace emu

#define EMU_REGISTER_DEVICE_FINDER                                                         \
    struct EMU_CONCAT(device_finder_, __LINE__) {                                          \
        EMU_CONCAT(device_finder_, __LINE__)() {                                           \
            emu::detail::register_device_finder(instantiate());                            \
        }                                                                                  \
        emu::detail::device_finder_engine instantiate();                                   \
    } EMU_CONCAT(device_finder_instance_, __LINE__);                                       \
    emu::detail::device_finder_engine EMU_CONCAT(device_finder_, __LINE__)::instantiate()
