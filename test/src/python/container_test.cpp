#include <test_utility/view.hpp>
#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/container_test.hpp>

#include <emu/pybind11/cast/container.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView, CppToPythonToCppContainer);

    struct container_of_int {

        using data_type = int;

        using view_type = emu::container<int>;
        using const_view_type = emu::container<const int>;

        constexpr static std::size_t rank = 1;

        static view_type get_view() {
            return view_type(emu_test::md_helper::get_vector<data_type>());
        }

        static const_view_type get_const_view() {
            return const_view_type(emu_test::md_helper::get_vector<data_type>());
        }

    };

    using ContainerTestsList = testing::Types<container_of_int>;

    INSTANTIATE_TYPED_TEST_SUITE_P(ContainerPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                ContainerTestsList);  // Type list

} // namespace
