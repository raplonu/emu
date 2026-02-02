#pragma once

#include <emu/detail/mdspan_types.hpp>

#include <pybind11/pybind11.h>

#include <memory>

namespace emu_test
{

    struct default_accessor_test_helper {
        template<typename ElementType>
        using accessor_type = emu::default_accessor<ElementType>;

        template<typename ElementType>
        static std::unique_ptr<ElementType[]> allocate_data(std::size_t size) {
            if (size == 0) // Not sure if this is necessary, but just to be safe
                return std::unique_ptr<ElementType[]>(nullptr);
            else
                return std::make_unique<ElementType[]>(size);
        }

        static constexpr bool support_read_only = true;

        static pybind11::dict get_array_interface(const pybind11::object& obj) {
            return obj.attr("__array_interface__").cast<pybind11::dict>();
        }
    };

} // namespace emu_test

#ifdef EMU_CUDA

#include <emu/cuda/device/accessor.hpp>
#include <emu/cuda/memory.hpp>

namespace emu_test
{

    struct cuda_accessor_test_helper {
        template<typename ElementType>
        using accessor_type = emu::default_accessor<ElementType>;

        template<typename ElementType>
        static emu::cuda::device::unique_ptr<ElementType[]> allocate_data(std::size_t size) {
            if (size == 0) // Not sure if this is necessary, but just to be safe
                return emu::cuda::device::unique_ptr<ElementType[]>(nullptr);
            else
                return emu::cuda::device::make_unique<ElementType[]>(size);
        }

        // cupy – which is the python counterpart of cuda spans – does not support read-only arrays.
        static constexpr bool support_read_only = false;

        static pybind11::dict get_array_interface(const pybind11::object& obj) {
            return obj.attr("__cuda_array_interface__").cast<pybind11::dict>();
        }
    };

} // namespace emu_test

#endif // EMU_CUDA
