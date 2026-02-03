#pragma once

#include <gtest/gtest.h>

#include <emu/tensor_traits.hpp>

#include <test_utility/python/core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/embed.h>

namespace
{

    TYPED_TEST_P(PythonViewTest, CppToPythonToCppCapsule)
    {
        namespace py = pybind11;
        using namespace emu_test;

        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using element_type = typename TestFixture::element_type;

        TestFixture::for_each_test_value([](const view_type& view){
            using tensor_t = emu::tensor_traits<view_type>;

            auto& capsule = view.capsule();

            EXPECT_EQ(capsule.use_count(), 1);

            {
                // c++ to python
                py::object obj = py::cast(view);

                EXPECT_EQ(capsule.use_count(), 2);

                {
                    // python to c++
                    auto view2 = py::cast<view_type>(obj);

                    EXPECT_EQ(tensor_t::data_handle(view2), tensor_t::data_handle(view));
                    EXPECT_EQ(capsule.use_count(), 3);
                    EXPECT_EQ(capsule, view2.capsule());
                }

                // view2 is destroyed. Capsule use_count decreases.
                EXPECT_EQ(capsule.use_count(), 2);

            }

            // obj is destroyed. Capsule use_count will decrease at some point. Cannot guarantee when.
            EXPECT_LE(capsule.use_count(), 2);
        });

        TestFixture::for_each_const_test_value([](const const_view_type& cview){
            using ctensor_t = emu::tensor_traits<const_view_type>;

            auto& capsule = cview.capsule();

            EXPECT_EQ(capsule.use_count(), 1);

            {
                // c++ to python
                py::object obj = py::cast(cview);

                EXPECT_EQ(capsule.use_count(), 2);

                {
                    // python to c++
                    auto cview2 = py::cast<const_view_type>(obj);

                    EXPECT_EQ(ctensor_t::data_handle(cview2), ctensor_t::data_handle(cview));
                    EXPECT_EQ(capsule.use_count(), 3);
                    EXPECT_EQ(capsule, cview2.capsule());
                }

                // cview2 is destroyed. Capsule use_count decreases.
                EXPECT_EQ(capsule.use_count(), 2);

            }

            // obj is destroyed. Capsule use_count will decrease at some point. Cannot guarantee when.
            EXPECT_LE(capsule.use_count(), 2);
        });

    }

} // namespace
