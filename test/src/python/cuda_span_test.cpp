#include <gtest/gtest.h>

#include <test_utility/cuda_view.hpp>
#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>

#include <emu/cuda/device/span.hpp>

#include <emu/pybind11/cast/span.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView);

    namespace py = pybind11;

    struct span_of_int {

        using data_type = int;

        using view_type = emu::cuda::device::span<int>;
        using const_view_type = emu::cuda::device::span<const int>;

        constexpr static std::size_t rank = 1;
        constexpr static bool support_read_only = false;

        static view_type get_view() {
            // The vector stays alive for the duration of the test.
            static auto device_vec = emu_test::md_helper::get_device_array<data_type>();
            return view_type(device_vec.get());
        }

        static const_view_type get_const_view() {
            // The vector stays alive for the duration of the test.
            static auto device_vec = emu_test::md_helper::get_device_array<data_type>();
            return const_view_type(device_vec.get());
        }

        static py::dict get_array_interface(py::object obj) {
            return obj.attr("__cuda_array_interface__").cast<py::dict>();
        }


    };

    using SpanTestsList = testing::Types<span_of_int>;

    INSTANTIATE_TYPED_TEST_SUITE_P(CudaSpanPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                SpanTestsList);  // Type list

} // namespace
