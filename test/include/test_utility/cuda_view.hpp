#pragma once

#include <test_utility/view.hpp>
#include <emu/cuda.hpp>

namespace emu_test::md_helper
{

    template <typename T>
    auto get_device_array() {
        auto device_span = ::cuda::memory::device::make_unique_span<T>(buffer_size);

        auto vec = get_vector<T>();

        cuda::memory::copy(device_span, vec.data(), vec.size() * sizeof(T));
        return device_span;
    }

} // namespace emu_test::md_helper
