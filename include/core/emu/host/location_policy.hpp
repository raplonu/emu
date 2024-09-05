#pragma once

#include <emu/location_policy.hpp>
#include <emu/detail/dlpack_types.hpp>

namespace emu::host
{

    struct location_policy
    {
        template <typename T>
        static constexpr bool validate_source = spe::enable_host_range<rm_cvref<T>>;

        static constexpr dlpack::device_t device_of(const byte*) noexcept {
            return {.device_type=dlpack::device_type_t::kDLCPU, .device_id=0};
        }

        // static constexpr bool check_device(dlpack::device_t device) noexcept {
        //     return device.device_type == dlpack::device_type_t::kDLCPU
        //         or device.device_type == dlpack::device_type_t::kDLCUDAHost
        //         or device.device_type == dlpack::device_type_t::kDLCUDAManaged
        //         or device.device_type == dlpack::device_type_t::kDLROCMHost
        //     ;
        // }

        // static constexpr dlpack::device_type_t device_type = dlpack::device_type_t::kDLCPU;

    };

} // namespace emu::host
