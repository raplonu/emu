#pragma once

#include <emu/assert.hpp>
#include <emu/cuda.hpp>
#include <emu/location_policy.hpp>
#include <emu/detail/dlpack_types.hpp>
// #include <cuda/api/pointer.hpp>

namespace emu
{

namespace cuda
{

    struct device_location_policy
    {
        template <typename T>
        static constexpr bool validate_source = spe::enable_cuda_device_range<rm_cvref<T>>;

        static dlpack::device_t device_of(const byte* ptr) noexcept
        {
            auto v_ptr = reinterpret_cast<void*>(const_cast<byte*>(ptr));

            //TODO: Maybe accept managed/unified and array as well.

            // Assume that the pointer is a device pointer in release builds
            EMU_ASSERT_MSG(cu::memory::type_of(v_ptr) == cu::memory::type_t::device_, "Pointer is not a device pointer");

            auto ptr_descriptor = cu::memory::pointer::wrap(v_ptr);
            return {.device_type=dlpack::device_type_t::kDLCUDA, .device_id=ptr_descriptor.device().id()};
        }

        // static bool check_device(dlpack::device_t device) noexcept {
        //     return device.device_type == dlpack::device_type_t::kDLCUDA;
        // }
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

        static constexpr dlpack::device_type_t device_type = dlpack::device_type_t::kDLCUDA;

    };

} // namespace cuda

} // namespace emu
