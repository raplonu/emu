#ifndef EMU_MISC_LOCATION_H
#define EMU_MISC_LOCATION_H

namespace emu
{

namespace location
{

    struct host_t {
        template<typename T>
        using default_allocator_type = std::allocator<T>;

        template<typename T>
        default_allocator_type<T> make_default_allocator() {
            return default_allocator_type<T>();
        }
    };

} // namespace location

    template<>
    struct is_location<location::host_t> : std::true_type {};

} // namespace emu

#endif //EMU_MISC_LOCATION_H