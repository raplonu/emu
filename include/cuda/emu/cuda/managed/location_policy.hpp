#pragma once

#include <emu/assert.hpp>
#include <emu/cuda.hpp>
#include <emu/location_policy.hpp>
#include <emu/detail/dlpack_types.hpp>
#include <emu/cuda/pointer.hpp>

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
            EMU_ASSERT_MSG(emu::cuda::get_memory_type(v_ptr) == emu::cuda::memory_type_t::device_, "Pointer is not a device pointer");

            return {.device_type=dlpack::device_type_t::kDLCUDA, .device_id=emu::cuda::get_device_of_pointer(v_ptr)};
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
