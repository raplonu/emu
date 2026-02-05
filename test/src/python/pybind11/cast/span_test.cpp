#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/layout_test_helper.hpp>
#include <test_utility/python/accessor_test_helper.hpp>

#include <emu/span.hpp>

#include <emu/pybind11/cast/span.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView
    );

    template<typename T>
    struct span_test_helper : emu_test::ViewTestHelperBase< span_test_helper<T> >
    {
        using accessor_helper = emu_test::default_accessor_test_helper;
        using layout_helper = emu_test::layout_contiguous_helper;

        using element_type = T;
        using extent_type = emu::dims<1>;

        using view_type = emu::span<element_type>;
        using const_view_type = emu::span<const element_type>;

        static view_type get_view(const std::unique_ptr<element_type[]>& data, std::size_t size) {
            return view_type(data.get(), size);
        }

        static const_view_type get_const_view(const std::unique_ptr<element_type[]>& data, std::size_t size) {
            return const_view_type(data.get(), size);
        }

    };

    using SpanTestsList = emu_test::cartesian_product_test_type<
        span_test_helper,
        emu_test::DataTypeList
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(SpanPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                SpanTestsList);  // Type list

} // namespace
