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

    struct mdcontainer_1d_of_int {

        using data_type = int;

        using view_type = emu::mdcontainer_1d<int>;
        using const_view_type = emu::mdcontainer_1d<const int>;

        constexpr static std::size_t rank = 1;

        static view_type get_view() {
            return view_type(emu_test::md_helper::get_vector<data_type>(), emu_test::md_helper::buffer_size);
        }

        static const_view_type get_const_view() {
            return const_view_type(emu_test::md_helper::get_vector<data_type>(), emu_test::md_helper::buffer_size);
        }

    };

    using MDContainerTestsList = testing::Types<mdcontainer_1d_of_int>;

    INSTANTIATE_TYPED_TEST_SUITE_P(MDContainerPythonTests,    // Instance name
                                PythonViewTest,             // Test case name
                                MDContainerTestsList);  // Type list

} // namespace







// #include <gtest/gtest.h>
// #include <emu/pybind11.hpp>
// #include <emu/pybind11/cast/mdcontainer.hpp>

// #include <pybind11/embed.h> // everything needed for embedding
// #include <pybind11/pytypes.h>

// namespace py = pybind11;

// namespace {

// struct TestCase1 {

//         std::array<size_t, 6> buffer = {1, 2, 3, 4, 5, 6};
//         emu::mdspan md(buffer.data(), 6);

//         std::array<size_t, 1> expected_shape{6};
//         std::array<size_t, 1> expected_strides{1};



// };

// template<typename T>
// struct MdSpanTest : testing::Test {



// };

//     TEST(MdContainer, PythonToCpp) {
//         // py::scoped_interpreter guard{}; // start the interpreter and keep it alive

//         // auto locals = py::dict();

//         // py::exec(R"(
//         //     import numpy as np
//         //     array = np.array([1, 2, 3, 4, 5, 6], dtype=np.int32)
//         // )", py::globals(), locals);

//         // auto py_array = locals["array"].cast<py::array>();
//         // auto md = locals["array"].cast<emu::mdspan_1d<int>>();

//         // EXPECT_EQ(md.size(), 6);
//         // EXPECT_EQ(md[0], 1);
//         // EXPECT_EQ(md[1], 2);
//         // EXPECT_EQ(md[2], 3);

//         // EXPECT_EQ(static_cast<const int*>(py_array.data()), md.data_handle());
//         // {




//         //     auto res = py::eval("'abc'").cast<cstring_view>();

//         //     EXPECT_EQ(res, "abc"_csv);
//         // }
//         // {
//         //     auto res = py::eval("''").cast<cstring_view>();

//         //     EXPECT_EQ(res, ""_csv);
//         // }
//         // {
//         //     //TODO: more investigation needed
//         //     // Test is "disable" because of the following error:
//         //     // Cannnot return a view since the underlying buffer cannot be kept alive.
//         //     // auto res = py::eval("'💩'").cast<emu::cu16string_view>();
//         //     auto res = py::eval("'💩'").cast<std::u16string>();

//         //     EXPECT_EQ(res, u"💩"_csv);
//         // }
//     }

//     TEST(MdContainer, CasterTest) {
//         using namespace py::literals;


//         std::array buffer = {1, 2, 3, 4, 5, 6};
//         emu::mdspan md(buffer.data(), 6);

//         py::detail::type_caster<decltype(md)> caster;

//         auto py_array = caster.cast(md, py::return_value_policy::automatic, py::none());

//         auto shape = py_array.attr("shape").cast<py::tuple>();
//         EXPECT_EQ(shape.size(), 1);
//         EXPECT_EQ(shape[0].cast<int>(), 6);

//         auto dtype = py_array.attr("dtype").cast<py::dtype>();
//         EXPECT_EQ(dtype.itemsize(), 4);

//         auto array = py_array.cast<py::array>();
//         EXPECT_EQ(static_cast<const int*>(array.data()), buffer.data());
//     }

//     TEST(MdContainer, CppToPython) {
//         using namespace py::literals;

//         py::scoped_interpreter guard{}; // start the interpreter and keep it alive

//         std::array buffer = {1, 2, 3, 4, 5, 6};

//         auto test_module = py::module_::create_extension_module("test_module", "doc", new py::module_::module_def);

//         test_module.def("test", [&] { return emu::mdspan(buffer.data(), 6); });

//         auto locals = py::dict("module"_a=test_module);

//         py::exec(R"(
//             array = module.test()
//             shape = array.shape
//             dtype = array.dtype

//         )", py::globals(), locals);

//         EXPECT_TRUE(py::eval("module.test().shape == (6,)", locals).cast<bool>());
//     }

// } // namespace
