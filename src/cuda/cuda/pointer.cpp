#include <emu/pointer.hpp>

#include <emu/utility.hpp>
#include <emu/cuda.hpp>

#include <fmt/format.h>

namespace emu::cuda
{

    optional<dlpack::device_t> get_device_of_pointer(const byte * ptr) {
        auto* v_ptr = v_ptr_of(ptr);

        auto type = cu::memory::type_of(v_ptr);

        EMU_TRUE_OR_RETURN_NULLOPT(type != cu::memory::type_t::non_cuda);

        auto device_id = cu::memory::pointer::wrap(v_ptr).device().id();

        auto is_managed = cu::memory::pointer::detail_::get_attribute<CU_POINTER_ATTRIBUTE_IS_MANAGED>(v_ptr);

        // type_of use cuPointerGetAttribute which is not able to distinguish between device and managed memory
        // so we need to check if the memory is managed
        if (is_managed) type = cu::memory::type_t::managed_;

        switch (type) {
            case cu::memory::type_t::host_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDAHost, 0};
            case cu::memory::type_t::device_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDA, device_id};
            case cu::memory::type_t::managed_:
                return optional<dlpack::device_t>{in_place, dlpack::device_type_t::kDLCUDAManaged, device_id};
            default:
                return nullopt;
        }
    }

} // namespace emu::cuda

EMU_REGISTER_DEVICE_FINDER(emu_cuda_device_pointer, emu::cuda::get_device_of_pointer)
