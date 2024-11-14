#pragma once

#include <gtest/gtest.h>
#include <test_utility/python/core.hpp>
#include <test_utility/view.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>

namespace
{

    TYPED_TEST_P(PythonViewTest, CppToPythonToCppView)
    {
        namespace py = pybind11;
        using namespace emu_test;

        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;

        {
            auto view = TestFixture::get_view();

            // c++ to python
            auto obj = py::cast(view);

            {
                auto data = TestFixture::get_array_interface(obj)["data"].template cast<py::tuple>();

                auto ptr = data[0].template cast<uintptr_t>();
                auto read_only = data[1].template cast<bool>();

                EXPECT_FALSE(read_only);
                EXPECT_EQ(obj.attr("itemsize").template cast<size_t>(), sizeof(data_type));
                EXPECT_EQ(obj.attr("size").template cast<size_t>(), view.size());
                EXPECT_EQ(obj.attr("ndim").template cast<size_t>(), TestFixture::rank);
                EXPECT_EQ(reinterpret_cast<const data_type*>(ptr), data_of(view));
            }

            // python to c++
            auto view2 = py::cast<view_type>(obj);

            EXPECT_EQ(data_of(view2), data_of(view));
            EXPECT_EQ(view2.size(), view.size());
        }
        {
            auto cview = TestFixture::get_const_view();

            // c++ to python
            auto obj = py::cast(cview);

            {
                auto data = TestFixture::get_array_interface(obj)["data"].template cast<py::tuple>();

                auto ptr = data[0].template cast<uintptr_t>();
                auto read_only = data[1].template cast<bool>();

                if (TestFixture::support_read_only) {
                    EXPECT_TRUE(read_only);
                }

                EXPECT_EQ(obj.attr("itemsize").template cast<size_t>(), sizeof(data_type));
                EXPECT_EQ(obj.attr("size").template cast<size_t>(), cview.size());
                EXPECT_EQ(obj.attr("ndim").template cast<size_t>(), TestFixture::rank);
                EXPECT_EQ(reinterpret_cast<const data_type*>(ptr), data_of(cview));
            }

            // python to c++
            auto cview2 = py::cast<const_view_type>(obj);

            EXPECT_EQ(data_of(cview2), data_of(cview));
            EXPECT_EQ(cview2.size(), cview.size());
        }
    }

} // namespace
