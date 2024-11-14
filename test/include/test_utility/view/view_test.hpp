#pragma once

#include <test_utility/view/core.hpp>

namespace
{

    TYPED_TEST_P(ViewTest, ViewConstruct)
    {
        using view_type = typename TestFixture::view_type;
        using const_view_type = typename TestFixture::const_view_type;
        using data_type = typename TestFixture::data_type;
        {
            const view_type con;

            EXPECT_EQ(con.data(), nullptr);
            EXPECT_EQ(con.size(), 0);
        }
        {
            int arr[] = {1, 2, 3, 4, 5}; // NOLINT
            const view_type con = arr;

            EXPECT_EQ(con.data(), arr);
            EXPECT_EQ(con.size(), 5);
        }
        {
            std::array arr = {1, 2, 3, 4, 5};
            const view_type con( arr );

            EXPECT_EQ(con.data(), arr.data());
            EXPECT_EQ(con.size(), arr.size());
        }
        {
            //It's not possible to construct a view or a container from a std::array rvalue.
            //because it is not possible to get the pointer after moving the array
            //into the capsule.
            static_assert(not std::constructible_from<view_type, std::array<int, 5>>);
            static_assert(not std::constructible_from<view_type, std::array<int, 5>&&>);
        }
        {
            std::vector<data_type> vec(3, 0);

            view_type con( vec );

            EXPECT_EQ(con.data(), vec.data());
            EXPECT_EQ(con.size(), vec.size());
        }

        {
            const view_type con{ std::vector(5, 0) };

            EXPECT_NE(con.data(), nullptr);
            EXPECT_EQ(con.size(), 5);
        }

        {
            std::vector<data_type> vec(3, 0);

            const_view_type con( vec );

            EXPECT_EQ(con.data(), vec.data());
            EXPECT_EQ(con.size(), vec.size());
        }
    }

} // namespace
