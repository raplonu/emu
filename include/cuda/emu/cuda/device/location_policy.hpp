#pragma once

#include <emu/location_policy.hpp>
// #include <emu/optional.hpp>

// #include <cuda/api/pointer.hpp>

namespace emu
{

namespace cuda
{

    struct device_location_policy
    {
        template <typename T>
        static constexpr bool validate_source = spe::enable_cuda_device_range<rm_cvref<T>>;

        // static optional<DLDevice> dl_device_of(void* ptr) {
        //     auto ptr_descriptor = cuda::memory::pointer::wrap(ptr);
        //     auto type = ptr_descriptor.get_attribute<CU_POINTER_ATTRIBUTE_MEMORY_TYPE>();
        //     switch (type) {
        //         case cuda::memory::type_t::host_:
        //             return {kDLCUDAHost, 0};
        //         case cuda::memory::type_t::device_:
        //             return {kDLCUDA, ptr_descriptor.device().id()};
        //         case cuda::memory::type_t::managed_:
        //             return {kDLCUDAManaged, ptr_descriptor.device().id()};
        //         default:
        //             nullopt;
        //     }
        // }

    };

} // namespace cuda

} // namespace emu
