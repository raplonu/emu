#pragma once

#include <emu/location_policy.hpp>

namespace emu::host
{

namespace detail
{

    struct ctx_manager
    {
        void* data = nullptr;
        int64_t* shane_and_strides = nullptr;
    };

} // namespace detail


    struct source_policy
    {
        template <typename T>
        static constexpr bool validate_source = spe::enable_host_range<rm_cvref<T>>;

        // static constexpr DLDevice dl_device_of(void*) { return {kDLCPU, 0}; }

        // template<cpts::mdspan MdSpan>
        // DLManagedTensor dlpack(const MdSpan& m) {

        //     return
        // }
    };

} // namespace emu::host
