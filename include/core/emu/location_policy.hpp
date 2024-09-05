#pragma once

#include <emu/type_traits.hpp>
#include <emu/utility.hpp>
#include <emu/detail/dlpack_types.hpp>

namespace emu
{

    struct no_location_policy
    {
        template <typename>
        static constexpr bool validate_source = true;

        //Note: experimental. Should not be used.
        static constexpr dlpack::device_t device_of(const byte*) noexcept {
            //TODO: Maybe try to detect the device type.
            return {.device_type=dlpack::device_type_t::kDLCPU, .device_id=0};
        }

        // static constexpr bool compatible_device(dlpack::device_t device) noexcept {
        //     return true;
        // }

        // Use kDLExtDev to indicate that the device is not known.
        // It is not meant to be used like this, but has long as it is only
        // used for implementation details, it should be fine.
        static constexpr dlpack::device_type_t device_type = dlpack::device_type_t::kDLExtDev;

    };

namespace detail
{
    template<typename T>
    struct location_type_of { using type = no_location_policy; };

    template<typename T>
        requires( requires { typename T::location_type; })
    struct location_type_of<T> { using type = typename T::location_type; };

} // namespace detail

    template<typename T>
    using location_type_of = typename detail::location_type_of<T>::type;

} // namespace emu
