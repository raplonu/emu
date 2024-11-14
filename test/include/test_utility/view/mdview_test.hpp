#pragma once

#include <test_utility/view/core.hpp>
#include <test_utility/view.hpp>

namespace
{

    TYPED_TEST_P(ViewTest, MDViewConstruct)
    {
        using namespace emu_test;

        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;

        constexpr size_t rank = TestFixture::rank;
        {
            const view_type con;

            expect_address(con, nullptr);
            EXPECT_EQ(con.size(), 0);
        }
        {
            data_type arr[md_helper::buffer_size];
            md_helper::fill_c_array(arr);

            const view_type con{ arr, md_helper::get_extents<rank>() };

            check_view(con, arr, md_helper::get_extents<rank>());
        }
        {
            auto arr = md_helper::get_array<data_type>();
            const view_type con( arr, md_helper::get_extents<rank>() );

            check_view(con, arr.data(), md_helper::get_extents<rank>());
        }
        {
            //It's not possible to construct a view or a container from a std::array rvalue.
            //because it is not possible to get the pointer after moving the array
            //into the capsule.
            static_assert(not std::constructible_from<view_type, std::array<int, 5>>);
            static_assert(not std::constructible_from<view_type, std::array<int, 5>&&>);
        }
        {
            auto vec = md_helper::get_vector<data_type>();

            view_type con( vec, md_helper::get_extents<rank>() );

            check_view(con, vec.data(), md_helper::get_extents<rank>());
        }
        {
            auto vec = md_helper::get_vector<data_type>();

            const_view_type con( vec, md_helper::get_extents<rank>() );

            check_view(con, vec.data(), md_helper::get_extents<rank>());
        }
    }

}
