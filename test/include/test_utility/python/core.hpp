#pragma once

#include <gtest/gtest.h>

#include <emu/info.hpp>

#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>

#include <complex>

namespace emu_test
{
    namespace mp = boost::mp11;

    template<template<typename...> class OutputType, class... Lists>
    using cartesian_product_test_type =
        mp::mp_rename<
            mp::mp_product<
                OutputType,
                Lists...
            >,
            testing::Types
        >;

    template<typename Derived>
    struct ViewTestHelperBase {

        template<typename Fn>
        static void for_each_test_value(Fn&& fn) {
            using element_type = typename Derived::element_type;

            using accessor_helper = typename Derived::accessor_helper;
            using extent_type = typename Derived::extent_type;
            using layout_helper = typename Derived::layout_helper;


            for (auto [mapping, required_data_size] : layout_helper::template get_mappings<extent_type>()) {
                auto data = accessor_helper::template allocate_data<element_type>(required_data_size);

                auto view = Derived::get_view(data, mapping);

                SCOPED_TRACE(emu::info_str(view));
                fn(view);
            }
        }

        template<typename Fn>
        static void for_each_const_test_value(Fn&& fn) {
            using element_type = typename Derived::element_type;

            using accessor_helper = typename Derived::accessor_helper;
            using extent_type = typename Derived::extent_type;
            using layout_helper = typename Derived::layout_helper;

            for (auto [mapping, required_data_size] : layout_helper::template get_mappings<extent_type>()) {
                auto data = accessor_helper::template allocate_data<element_type>(required_data_size);

                auto cview = Derived::get_const_view(data, mapping);

                SCOPED_TRACE(emu::info_str(cview));
                fn(cview);
            }
        }

    };

    using DataTypeList = std::tuple< int, float, double, std::complex<float>, std::complex<double> >; // We test for these data types


} // namespace emu_test

namespace
{

    template<typename ViewTestHelper>
    struct PythonViewTest : ViewTestHelper, testing::Test { };

    TYPED_TEST_SUITE_P(PythonViewTest);

} // namespace
