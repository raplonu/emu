#include <emu/detail/dlpack_types.hpp>
#include <emu/error.hpp>
#include <emu/macro.hpp>
#include <emu/optional.hpp>
#include <emu/pointer.hpp>
#include <emu/utility.hpp>
#include <emu/string.hpp>

#include <dlpack/dlpack.h>

#include <filesystem>
#include <list>
#include <string>
#include <bit>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <ranges>
#include <system_error>
#include <utility>

namespace emu
{

namespace detail
{

    using device_finder_set = std::list<device_finder_engine>;

namespace
{
    device_finder_set& get_device_finders() {
        static device_finder_set finders;
        return finders;
    }

} // namespace

    void register_device_finder(device_finder_engine finder) {
        get_device_finders().push_back(std::move(finder));
    }

    // linux memory maps location
    constexpr auto maps_path = "/proc/self/maps";

} // namespace detail

namespace
{

    std::pair<std::uintptr_t, std::uintptr_t> memory_range_from_string(std::string_view sv) {
        auto tokens = split_string(sv, "-");
        auto begin = tokens.begin();

        std::uintptr_t start = 0, stop = 0;

        EMU_TRUE_OR_THROW(begin != tokens.end(), errc::pointer_parsing_error);
        EMU_UNWRAP_RES_OR_THROW(emu::from_chars(*begin, start, 16));

        EMU_TRUE_OR_THROW(++begin != tokens.end(), errc::pointer_parsing_error);
        EMU_UNWRAP_RES_OR_THROW(emu::from_chars(*begin, stop, 16));

        return {start, stop};
    }

    std::filesystem::perms permissions_from_string(std::string_view sv) {
        using std::filesystem::perms;
        perms permissions = perms::none;

        if (sv.size() == 4) {
            if (sv[0] == 'r') permissions |= perms::owner_read;
            if (sv[1] == 'w') permissions |= perms::owner_write;
            if (sv[2] == 'x') permissions |= perms::owner_exec;
            // if (sv[3] == 'p') permissions |= # not supported
        }

        return permissions;
    }

    pointer_descriptor from_line(std::string_view line) {
        auto tokens = split_string_no_empty(line);

        auto begin = tokens.begin();

        const auto [start, stop] = memory_range_from_string(*begin++);

        auto size = stop - start;

        auto permissions_str = *begin++;

        const auto permissions = permissions_from_string(permissions_str);

        std::advance(begin, 3);

        const std::string_view location = *begin;

        return pointer_descriptor{
            .location = std::string(location),
            .permissions = permissions,
            .base_region = std::span{std::bit_cast<byte*>(start), size}
        };
    }

} // namespace



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
        // Some region are unamed. It is thus impossible to access the range.
        if (location.empty()) return nullopt;

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
