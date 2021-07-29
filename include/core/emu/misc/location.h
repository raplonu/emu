#ifndef EMU_MISC_LOCATION_H
#define EMU_MISC_LOCATION_H

namespace emu
{

namespace location
{

    struct host_t {};

} // namespace location

    template<>
    struct is_location<location::host_t> : std::true_type {};

} // namespace emu

#endif //EMU_MISC_LOCATION_H