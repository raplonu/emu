#include <emu/pointer.hpp>
#include <emu/cuda/pointer.hpp>

#include <emu/utility.hpp>
#include <emu/cuda.hpp>

#include <fmt/format.h>

namespace emu::cuda
{

    optional<dlpack::device_t> get_dlpack_device_of_pointer(const byte* ptr) {
        auto* v_ptr = v_ptr_of(ptr);

        auto type = get_memory_type(v_ptr);

        EMU_TRUE_OR_RETURN_NULLOPT(type != memory_type_t::unregistered);

        auto device_id = get_device_of_pointer(v_ptr).id();

        //TODO: check if still needed
        // auto is_managed = cu::memory::pointer::detail_::get_attribute<CU_POINTER_ATTRIBUTE_IS_MANAGED>(v_ptr);
        // // type_of use cuPointerGetAttribute which is not able to distinguish between device and managed memory
        // // so we need to check if the memory is managed
        // if (is_managed) type = cu::memory::type_t::managed_;

        switch (type) {
            case memory_type_t::host_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDAHost, 0};
            case memory_type_t::device_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDA, device_id};
            case memory_type_t::managed_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDAManaged, device_id};
            default:
                return nullopt;
        }
    }

} // namespace emu::cuda

EMU_REGISTER_DEVICE_FINDER(emu_cuda_device_pointer, emu::cuda::get_dlpack_device_of_pointer)
