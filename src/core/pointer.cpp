#include <emu/pointer.hpp>
#include <emu/utility.hpp>

#include <filesystem>
#include <list>
#include <string>
#include <charconv>
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
        get_device_finders().push_back(finder);
    }

    // linux memory maps location
    constexpr auto maps_path = "/proc/self/maps";

    optional<std::string> location_of(const byte* b_ptr) {
        auto ptr = reinterpret_cast<std::uintptr_t>(b_ptr);

        std::ifstream file{maps_path};  // create regular file

        EMU_TRUE_OR_RETURN_NULLOPT(file);

        auto not_empty = [](auto sv) { return not sv.empty(); };

        std::string line;
        while (std::getline(file, line)) {
            auto tokens = split_string(line)
                        | std::views::filter(not_empty); // ignore empty strings

            auto begin = tokens.begin();

            std::uintptr_t start, stop;
            {
                auto memory_range = split_string(*begin++, "-");
                auto it = memory_range.begin();
                {
                    auto [ptr, ec] = std::from_chars((*it).begin(), (*it).end(), start, 16);
                    // EMU_TRUE_OR_RETURN_UN_EC(ec == std::errc{}, ec);
                    EMU_TRUE_OR_RETURN_NULLOPT(ec == std::errc{});
                }
                ++it;
                {
                    auto [ptr, ec] = std::from_chars((*it).begin(), (*it).end(), stop, 16);
                    // EMU_TRUE_OR_RETURN_UN_EC(ec == std::errc{}, ec);
                    EMU_TRUE_OR_RETURN_NULLOPT(ec == std::errc{});
                }
            }

            auto perm = *begin++;
            *begin++;
            *begin++;
            *begin++;
            std::string_view location = *begin;

            if (start <= ptr and ptr < stop){
                if (location.empty())
                    return "anonymous";
                else
                    return std::string(location.begin(), location.end());
            }

        }
        return "unknown";
    }

    optional<dlpack::device_t> device_from_location(std::string_view location) {
        if (location == "[heap]" or location == "[stack]") {
            return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCPU, 0};
        }
        return nullopt;
    }

} // namespace detail

    result<dlpack::device_t> get_device_of_pointer(const byte * ptr) {
        for (auto const& finder : detail::get_device_finders()) {
            EMU_UNWRAP_RETURN_IF_TRUE(finder(ptr));
        }
        //TODO: location_of may fail for other reasons than not finding the pointer
        // It should not be considered an error.
        EMU_UNWRAP_RETURN_IF_TRUE(detail::location_of(ptr).and_then(detail::device_from_location));

        return make_unexpected(error::pointer_device_not_found);
    }

} // namespace emu
