#include <emu/pointer.hpp>
#include <emu/utility.hpp>
#include <emu/charconv.hpp>

#include <filesystem>
#include <list>
#include <string>
namespace emu
{

namespace detail
{

    using device_finder_set = std::list<device_finder_engine>;

    device_finder_set& get_device_finders() {
        static device_finder_set finders;
        return finders;
    }

    void register_device_finder(device_finder_engine finder) {
        get_device_finders().push_back(std::move(finder));
    }

    // linux memory maps location
    constexpr auto maps_path = "/proc/self/maps";

} // namespace detail

    pointer_descriptor from_line(std::string_view line) {
        auto not_empty = [](auto sv) { return not sv.empty(); };

        auto tokens = split_string_no_empty(line);

        auto begin = tokens.begin();

        std::uintptr_t start = 0, stop = 0;
        {
            auto memory_range = split_string(*begin++, "-");
            auto it = memory_range.begin();

            EMU_UNWRAP_RES_OR_THROW(emu::from_chars(*it, start, 16));

            ++it;

            EMU_UNWRAP_RES_OR_THROW(emu::from_chars(*it, stop, 16));

        }

        auto size = stop - start;

        auto permissions_str = *begin++;

        std::filesystem::perms permissions = std::filesystem::perms::none;

        if (permissions_str.size() == 4) {
            if (permissions_str[0] == 'r') permissions |= std::filesystem::perms::owner_read;
            if (permissions_str[1] == 'w') permissions |= std::filesystem::perms::owner_write;
            if (permissions_str[2] == 'x') permissions |= std::filesystem::perms::owner_exec;
            // if (permissions_str[3] == 'p') permissions |= # not supported
        }

        std::advance(begin, 3);

        std::string_view location = *begin;

        if (location.empty())
            location = "[anonymous]";

        return pointer_descriptor{.location = std::string(location), .permissions = permissions, .base_region = std::span{reinterpret_cast<byte*>(start), size}};
    }

    optional<pointer_descriptor> pointer_descritor_of(const byte* b_ptr) {
        std::ifstream file{detail::maps_path};  // create regular file

        EMU_TRUE_OR_RETURN_NULLOPT(file);

        std::string line;
        while (std::getline(file, line)) {
            auto desc = from_line(line);

            auto region = desc.base_region;

            if (region.data() <= b_ptr and b_ptr < region.data() + region.size()) {
                return desc;
            }
        }
        return nullopt;
    }

    optional<std::span<byte>> region_from_location(std::string_view location) {
        std::ifstream file{detail::maps_path};  // create regular file

        EMU_TRUE_OR_RETURN_NULLOPT(file);

        std::string line;
        while (std::getline(file, line)) {
            auto desc = from_line(line);

            if (desc.location == location) {
                return desc.base_region;
            }
        }
        return nullopt;
    }

    result<dlpack::device_t> get_device_of_pointer(const byte * ptr) {
        for (auto const& finder : detail::get_device_finders()) {
            EMU_UNWRAP_RETURN_IF_TRUE(finder(ptr));
        }

        auto desc = EMU_UNWRAP_OR_RETURN_UN_EC(pointer_descritor_of(ptr), errc::pointer_desc_not_found);

        if (desc.location == "[heap]" or desc.location == "[stack]") {
            return dlpack::device_t{dlpack::device_type_t::kDLCPU, 0};
        }

        return make_unexpected(errc::pointer_device_not_found);
    }

} // namespace emu
