#pragma once

#include <gtest/gtest.h>

#include <emu/tensor_traits.hpp>

#include <test_utility/python/core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace
{

    TYPED_TEST_P(PythonViewTest, CppToPythonToCppView)
    {
        namespace py = pybind11;
        using namespace emu_test;

        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using element_type = typename TestFixture::element_type;

        using tensor_t = emu::tensor_traits<view_type>;
        using ctensor_t = emu::tensor_traits<const_view_type>;

        TestFixture::for_each_test_value([](const view_type& view){
            // c++ to python
            py::object obj = py::cast(view);

            {
                auto data = TestFixture::accessor_helper::get_array_interface(obj)["data"].template cast<py::tuple>();

                auto ptr = data[0].template cast<uintptr_t>();
                auto read_only = data[1].template cast<bool>();

                EXPECT_FALSE(read_only);
                EXPECT_EQ(obj.attr("itemsize").template cast<size_t>(), sizeof(element_type));
                EXPECT_EQ(obj.attr("size").template cast<size_t>(), tensor_t::size(view));
                EXPECT_EQ(obj.attr("ndim").template cast<size_t>(), tensor_t::rank);
                EXPECT_EQ(reinterpret_cast<const element_type*>(ptr), tensor_t::data_handle(view));

                for (size_t i = 0; i < tensor_t::rank; ++i) {
                    SCOPED_TRACE("extent/stride index: " + fmt::to_string(i));

                    EXPECT_EQ(obj.attr("shape").attr("__getitem__")(i).template cast<size_t>(), tensor_t::extent(view, i));
                    EXPECT_EQ(obj.attr("strides").attr("__getitem__")(i).template cast<size_t>(), tensor_t::stride(view, i) * sizeof(element_type));
                }
            }

            // python to c++
            auto view2 = py::cast<view_type>(obj);

            EXPECT_EQ(tensor_t::data_handle(view2), tensor_t::data_handle(view));
            EXPECT_EQ(tensor_t::size(view2), tensor_t::size(view));
        });

        TestFixture::for_each_const_test_value([](const const_view_type& cview){
            // c++ to python
            py::object obj = py::cast(cview);

            {
                auto data = TestFixture::accessor_helper::get_array_interface(obj)["data"].template cast<py::tuple>();

                auto ptr = data[0].template cast<uintptr_t>();
                auto read_only = data[1].template cast<bool>();

                if (TestFixture::accessor_helper::support_read_only) {
                    EXPECT_TRUE(read_only);
                }

                EXPECT_EQ(obj.attr("itemsize").template cast<size_t>(), sizeof(element_type));
                EXPECT_EQ(obj.attr("size").template cast<size_t>(), ctensor_t::size(cview));
                EXPECT_EQ(obj.attr("ndim").template cast<size_t>(), ctensor_t::rank);
                EXPECT_EQ(reinterpret_cast<const element_type*>(ptr), ctensor_t::data_handle(cview));

                for (size_t i = 0; i < ctensor_t::rank; ++i) {
                    SCOPED_TRACE("extent/stride index: " + fmt::to_string(i));

                    EXPECT_EQ(obj.attr("shape").attr("__getitem__")(i).template cast<size_t>(), ctensor_t::extent(cview, i));
                    EXPECT_EQ(obj.attr("strides").attr("__getitem__")(i).template cast<size_t>(), ctensor_t::stride(cview, i) * sizeof(element_type));
                }
            }

            // python to c++
            auto cview2 = py::cast<const_view_type>(obj);

            EXPECT_EQ(ctensor_t::data_handle(cview2), ctensor_t::data_handle(cview));
            EXPECT_EQ(ctensor_t::size(cview2), ctensor_t::size(cview));
        });

    }

} // namespace
