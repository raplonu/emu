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

    struct managed_location_policy
    {
        template <typename T>
        static constexpr bool validate_source = true; // TODO: Use spe::enable_cuda_managed_range<rm_cvref<T>> when available

        static dlpack::device_t device_of(const byte* ptr) noexcept
        {
            auto v_ptr = reinterpret_cast<void*>(const_cast<byte*>(ptr));

            // Assume that the pointer is a managed pointer in release builds
            EMU_ASSERT_MSG(emu::cuda::get_memory_type(v_ptr) == emu::cuda::memory_type_t::managed_, "Pointer is not a managed pointer");

            return {.device_type=dlpack::device_type_t::kDLCUDAManaged, .device_id=emu::cuda::get_device_of_pointer(v_ptr)};
        }

        static constexpr dlpack::device_type_t device_type = dlpack::device_type_t::kDLCUDAManaged;

    };

} // namespace cuda

} // namespace emu
