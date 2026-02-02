#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/layout_test_helper.hpp>
#include <test_utility/python/accessor_test_helper.hpp>

#include <emu/container.hpp>

#include <emu/pybind11/cast/container.hpp>
#include <emu/pybind11/cast/span.hpp> // Only used to check cast conflicts

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView
    );

    namespace py = pybind11;

    template<typename T>
    struct container_test_helper : emu_test::ViewTestHelperBase< container_test_helper<T> >
    {
        using accessor_helper = emu_test::default_accessor_test_helper;
        using layout_helper = emu_test::layout_contiguous_helper;

        using element_type = T;
        using extent_type = emu::dims<1>;

        using view_type = emu::container<element_type>;
        using const_view_type = emu::container<const element_type>;

        static view_type get_view(std::unique_ptr<element_type[]>& data, std::size_t size) {
            auto ptr = data.get();

            return view_type(ptr, size, std::move(data));
        }

        static const_view_type get_const_view(std::unique_ptr<element_type[]>& data, std::size_t size) {
            auto ptr = data.get();

            return const_view_type(ptr, size, std::move(data));
        }

    };

    using ContainerTestsList = emu_test::cartesian_product_test_type<
        container_test_helper,
        emu_test::DataTypeList
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(ContainerPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                ContainerTestsList);  // Type list

} // namespace
