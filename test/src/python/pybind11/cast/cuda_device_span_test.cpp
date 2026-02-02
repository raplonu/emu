#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/layout_test_helper.hpp>
#include <test_utility/python/accessor_test_helper.hpp>

#include <emu/cuda/device/span.hpp>

#include <emu/pybind11/cast/span.hpp>
#include <emu/pybind11/cast/container.hpp> // Only used to check cast conflicts

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView
    );

    namespace py = pybind11;

    template<typename T>
    struct cuda_device_span_test_helper : emu_test::ViewTestHelperBase< cuda_device_span_test_helper<T> >
    {
        using accessor_helper = emu_test::cuda_accessor_test_helper;
        using layout_helper = emu_test::layout_contiguous_helper;

        using element_type = T;
        using extent_type = emu::dims<1>;

        using view_type = emu::cuda::device::span<element_type>;
        using const_view_type = emu::cuda::device::span<const element_type>;

        static view_type get_view(const emu::cuda::device::unique_ptr<element_type[]>& data, std::size_t size) {
            return view_type(data.get(), size);
        }

        static const_view_type get_const_view(const emu::cuda::device::unique_ptr<element_type[]>& data, std::size_t size) {
            return const_view_type(data.get(), size);
        }

    };

    using CudaDeviceSpanTestsList = emu_test::cartesian_product_test_type<
        cuda_device_span_test_helper,
        emu_test::DataTypeList
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(CudaDeviceSpanPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                CudaDeviceSpanTestsList);  // Type list

} // namespace
