#pragma once

#include <test_utility/view.hpp>
#include <emu/cuda.hpp>

namespace emu_test::md_helper
{

    template <typename T>
    auto get_device_array() {
        auto device_ptr = emu::cuda::device::make_unique<T[]>(buffer_size);

        auto vec = get_vector<T>();

        emu::cuda::copy_n(vec.data(), vec.size(), device_ptr.get());
        return device_ptr;
    }

} // namespace emu_test::md_helper
