#pragma once

#include <emu/cuda/error.hpp>
#include <emu/cuda/device.hpp>
#include <emu/optional.hpp>
#include <emu/cstring_view.hpp>

#include <cuda_runtime.h>


namespace emu::cuda
{

    enum class memory_type_t : ::std::underlying_type<cudaMemoryType>::type {
        host_         = cudaMemoryTypeHost,
        device_       = cudaMemoryTypeDevice,
        managed_      = cudaMemoryTypeManaged,
        unregistered  = cudaMemoryTypeUnregistered
    };

    inline auto format_as(memory_type_t type) -> cstring_view
    {
        switch (type)
        {
        case memory_type_t::host_:
            return "host";
        case memory_type_t::device_:
            return "device";
        case memory_type_t::managed_:
            return "managed";
        case memory_type_t::unregistered:
            return "unregistered";
        default:
            return "unknown";
        }
    }

    using pointer_attribute_t = cudaPointerAttributes;

namespace detail
{

    inline pointer_attribute_t get_pointer_attributes(const void* ptr) {
        pointer_attribute_t attr;
        EMU_CUDA_CHECK_OR_THROW_WHAT(cudaPointerGetAttributes(&attr, ptr),
                                     "Failed to get pointer attributes");
        return attr;
    }

} // namespace detail

    inline memory_type_t get_memory_type(const void* ptr) {
        pointer_attribute_t attr = detail::get_pointer_attributes(ptr);
        return static_cast<memory_type_t>(attr.type);
    }

    inline device::id_t get_device_of_pointer(const void* ptr) {
        pointer_attribute_t attr = detail::get_pointer_attributes(ptr);
        if (attr.type == cudaMemoryTypeUnregistered) {
            throw_error(
                errc::cuda_pointer_unregistered,
                "Pointer is not registered with CUDA memory management."
            );
        }
        return attr.device;
    }

    inline optional<void*> get_device_pointer_of_pointer(const void* ptr) {
        pointer_attribute_t attr = detail::get_pointer_attributes(ptr);
        if (attr.devicePointer == nullptr) {
            return nullopt;
        }
        return attr.devicePointer;
    };

    inline optional<void*> get_host_pointer_of_pointer(const void* ptr) {
        pointer_attribute_t attr = detail::get_pointer_attributes(ptr);
        if (attr.hostPointer == nullptr) {
            return nullopt;
        }
        return attr.hostPointer;
    };


} // namespace emu::cuda
