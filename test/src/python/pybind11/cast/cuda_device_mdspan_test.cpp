#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/layout_test_helper.hpp>
#include <test_utility/python/accessor_test_helper.hpp>

#include <emu/cuda/device/mdspan.hpp>

#include <emu/pybind11/cast/mdspan.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView);

    template<typename T, typename ExtentType, typename LayoutHelper>
    struct cuda_device_mdspan_test_helper : emu_test::ViewTestHelperBase< cuda_device_mdspan_test_helper<T, ExtentType, LayoutHelper> >
    {
        using accessor_helper = emu_test::cuda_accessor_test_helper;
        using layout_helper = LayoutHelper;

        using element_type = T;
        using extent_type = ExtentType;
        using layout_type = typename layout_helper::layout_type;
        using accessor_type = typename accessor_helper::template accessor_type<element_type>;
        using const_accessor_type = typename accessor_helper::template accessor_type<const element_type>;

        using view_type = emu::cuda::device::mdspan<element_type, ExtentType, layout_type, accessor_type>;
        using const_view_type = emu::cuda::device::mdspan<const element_type, ExtentType, layout_type, const_accessor_type>;

        static view_type get_view(const emu::cuda::device::unique_ptr<element_type[]>& data, auto mapping) {
            return view_type(data.get(), mapping);
        }

        static const_view_type get_const_view(const emu::cuda::device::unique_ptr<element_type[]>& data, auto mapping) {
            return const_view_type(data.get(), mapping);
        }
    };

    using CudaDeviceMdSpanTestsList = emu_test::cartesian_product_test_type<
        cuda_device_mdspan_test_helper,
        emu_test::DataTypeList,
        emu_test::ExtentTypeList,
        emu_test::LayoutHelperList
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(CudaDeviceMdSpanPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                CudaDeviceMdSpanTestsList);  // Type list

} // namespace
