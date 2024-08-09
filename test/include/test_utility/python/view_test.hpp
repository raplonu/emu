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
                py::array arr = obj;
                EXPECT_TRUE(arr.writeable());
                EXPECT_EQ(arr.itemsize(), sizeof(data_type));
                EXPECT_EQ(arr.size(), view.size());
                EXPECT_EQ(arr.ndim(), TestFixture::rank);
                EXPECT_EQ(data_of(view), static_cast<const data_type*>(arr.data()));
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
                py::array arr = obj;
                EXPECT_FALSE(arr.writeable());
                EXPECT_EQ(arr.itemsize(), sizeof(data_type));
                EXPECT_EQ(arr.size(), cview.size());
                EXPECT_EQ(arr.ndim(), TestFixture::rank);
                EXPECT_EQ(data_of(cview), static_cast<const data_type*>(arr.data()));


            }

            // python to c++
            auto cview2 = py::cast<const_view_type>(obj);

            EXPECT_EQ(data_of(cview2), data_of(cview));
            EXPECT_EQ(cview2.size(), cview.size());
        }
    }

} // namespace
