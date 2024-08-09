#pragma once

#include <gtest/gtest.h>
#include <test_utility/python/core.hpp>
#include <test_utility/view.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/embed.h>

namespace
{

    TYPED_TEST_P(PythonViewTest, CppToPythonToCppContainer)
    {
        namespace py = pybind11;
        using namespace emu_test;

        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;

        {
            auto container = TestFixture::get_view();

            // c++ to python
            auto obj = py::cast(container);

            {
                py::array arr = obj;
                EXPECT_TRUE(arr.writeable());
                EXPECT_EQ(arr.itemsize(), sizeof(data_type));
                EXPECT_EQ(arr.size(), container.size());
                EXPECT_EQ(arr.ndim(), TestFixture::rank);
                EXPECT_EQ(data_of(container), static_cast<const data_type*>(arr.data()));
            }

            // python to c++
            auto container2 = py::cast<view_type>(obj);

            EXPECT_EQ(data_of(container2), data_of(container));
            EXPECT_EQ(container2.size(), container.size());
            EXPECT_EQ(container2.capsule().holder, container.capsule().holder);
        }
        {
            auto ccontainer = TestFixture::get_const_view();

            // c++ to python
            auto obj = py::cast(ccontainer);

            {
                py::array arr = obj;
                EXPECT_FALSE(arr.writeable());
                EXPECT_EQ(arr.itemsize(), sizeof(data_type));
                EXPECT_EQ(arr.size(), ccontainer.size());
                EXPECT_EQ(arr.ndim(), TestFixture::rank);
                EXPECT_EQ(data_of(ccontainer), static_cast<const data_type*>(arr.data()));
            }

            // python to c++
            auto ccontainer2 = py::cast<const_view_type>(obj);

            EXPECT_EQ(data_of(ccontainer2), data_of(ccontainer));
            EXPECT_EQ(ccontainer2.size(), ccontainer.size());
        }
    }

} // namespace
