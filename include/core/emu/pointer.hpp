#pragma once

#include <emu/utility.hpp>
#include <emu/detail/dlpack_types.hpp>
#include <emu/error.hpp>
#include <emu/optional.hpp>
#include <emu/span.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <filesystem>

namespace emu
{

    struct pointer_descriptor
    {
        std::string location;
        std::filesystem::perms permissions;
        std::span<byte> base_region;
    };

    optional<pointer_descriptor> pointer_descritor_of(const byte* b_ptr);

    optional<std::span<byte>> region_from_location(std::string_view location);

    result<dlpack::device_t> get_device_of_pointer(const byte * ptr);

    template<typename T>
    result<dlpack::device_t> get_device_of_pointer(const T * ptr)
    {
        return get_device_of_pointer(reinterpret_cast<const byte *>(ptr));
    }

namespace detail
{
    using device_finder_engine = std::function<optional<dlpack::device_t>(const byte* ptr)>;

    void register_device_finder(device_finder_engine finder);

} // namespace detail


} // namespace emu

#define EMU_REGISTER_DEVICE_FINDER(NAME, FUNC)             \
    extern "C" __attribute__ ((constructor)) void NAME() { \
        ::emu::detail::register_device_finder(FUNC);       \
    }
