#include <test_utility/view.hpp>
#include <gtest/gtest.h>

#include <test_utility/python/core.hpp>
#include <test_utility/python/view_test.hpp>
#include <test_utility/python/container_test.hpp>

#include <emu/pybind11/cast/mdcontainer.hpp>

namespace
{
    REGISTER_TYPED_TEST_SUITE_P(
        PythonViewTest,  // The first argument is the test case name.
        // The rest of the arguments are the test names.
        CppToPythonToCppView, CppToPythonToCppContainer);

    namespace py = pybind11;

    template<typename T, std::size_t N, typename LayoutPolicy>
    struct mdcontainer_of {

        using data_type = T;

        using view_type = emu::mdcontainer<data_type, emu::dims<N>, LayoutPolicy>;
        using const_view_type = emu::mdcontainer<const data_type, emu::dims<N>, LayoutPolicy>;

        constexpr static std::size_t rank = N;
        constexpr static bool support_read_only = true;

        static view_type get_view() {
            return view_type(emu_test::md_helper::get_vector<data_type>(), emu_test::md_helper::get_mapping<rank, LayoutPolicy>());
        }

        static const_view_type get_const_view() {
            return const_view_type(emu_test::md_helper::get_vector<data_type>(), emu_test::md_helper::get_mapping<rank, LayoutPolicy>());
        }

        static py::dict get_array_interface(const py::object& obj) {
            return obj.attr("__array_interface__").cast<py::dict>();
        }

    };

    using MDContainerTestsList = testing::Types<
        mdcontainer_of<int, 1, emu::layout_right>,
        mdcontainer_of<int, 2, emu::layout_right>,
        mdcontainer_of<int, 3, emu::layout_right>,
        mdcontainer_of<int, 1, emu::layout_left>,
        mdcontainer_of<int, 2, emu::layout_left>,
        mdcontainer_of<int, 3, emu::layout_left>,
        mdcontainer_of<int, 1, emu::layout_stride>,
        mdcontainer_of<int, 2, emu::layout_stride>,
        mdcontainer_of<int, 3, emu::layout_stride>,

        mdcontainer_of<float, 1, emu::layout_right>,
        mdcontainer_of<float, 2, emu::layout_right>,
        mdcontainer_of<float, 3, emu::layout_right>,
        mdcontainer_of<float, 1, emu::layout_left>,
        mdcontainer_of<float, 2, emu::layout_left>,
        mdcontainer_of<float, 3, emu::layout_left>,
        mdcontainer_of<float, 1, emu::layout_stride>,
        mdcontainer_of<float, 2, emu::layout_stride>,
        mdcontainer_of<float, 3, emu::layout_stride>,

        mdcontainer_of<std::complex<float>, 1, emu::layout_right>,
        mdcontainer_of<std::complex<float>, 2, emu::layout_right>,
        mdcontainer_of<std::complex<float>, 3, emu::layout_right>,
        mdcontainer_of<std::complex<float>, 1, emu::layout_left>,
        mdcontainer_of<std::complex<float>, 2, emu::layout_left>,
        mdcontainer_of<std::complex<float>, 3, emu::layout_left>,
        mdcontainer_of<std::complex<float>, 1, emu::layout_stride>,
        mdcontainer_of<std::complex<float>, 2, emu::layout_stride>,
        mdcontainer_of<std::complex<float>, 3, emu::layout_stride>
    >;

    INSTANTIATE_TYPED_TEST_SUITE_P(MDContainerPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                MDContainerTestsList);  // Type list

} // namespace
