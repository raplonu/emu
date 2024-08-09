#include <gtest/gtest.h>
#include <test_utility/spy.hpp>

#include <emu/range.hpp>
#include <emu/utility.hpp>

#include <array>

namespace
{
    TEST(range_test, from_array)
    {
        int input[] = {1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_array_of_const)
    {
        const int input[3] = {1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_array_of_volatile)
    {
        volatile int input[3] = {1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_std_array)
    {
        std::array<int, 3> input{1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_std_array_of_const)
    {
        std::array<const int, 3> input{1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_std_array_of_volatile)
    {
        std::array<volatile int, 3> input{1, 2, 3};
        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_movable_std_array)
    {
        {
            emu_test::spy_flag flag;
            // If it is from a rvalue of requested type, sinply move it.
            std::array<emu_test::Spy, 1> input{{&flag}};

            auto v = emu::as_vector(std::move(input));
            static_assert(std::is_same_v<decltype(v), std::vector<emu_test::Spy>>);

            // If a vector is moved, elements are not copied nor moved.
            EXPECT_TRUE(flag.copied);
            EXPECT_FALSE(flag.moved);
        }

        {
            emu_test::spy_flag flag;
            // If it is from a rvalue of requested type, sinply move it.
            std::array<emu_test::Spy, 1> input{{&flag}};

            auto v = emu::as_vector<emu_test::Spy>(std::move(input));
            static_assert(std::is_same_v<decltype(v), std::vector<emu_test::Spy>>);

            // If a vector is moved, elements are not copied nor moved.
            EXPECT_TRUE(flag.copied);
            EXPECT_FALSE(flag.moved);
        }
    }

    TEST(range_test, from_vector)
    {
        std::vector<int> input{1, 2, 3};

        {
            auto v = emu::as_vector(input);
            static_assert(std::is_same_v<decltype(v), std::vector<int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], input[0]);
            EXPECT_EQ(v[1], input[1]);
            EXPECT_EQ(v[2], input[2]);
        }

        {
            auto v = emu::as_vector<unsigned int>(input);
            static_assert(std::is_same_v<decltype(v), std::vector<unsigned int>>);

            using std::size;
            EXPECT_EQ(size(v), size(input));
            EXPECT_EQ(v[0], static_cast<unsigned int>(input[0]));
            EXPECT_EQ(v[1], static_cast<unsigned int>(input[1]));
            EXPECT_EQ(v[2], static_cast<unsigned int>(input[2]));
        }
    }

    TEST(range_test, from_movable_vector)
    {
        {
            emu_test::spy_flag flag;
            // If it is from a rvalue of requested type, sinply move it.
            std::vector<emu_test::Spy> input; input.emplace_back(&flag);

            auto v = emu::as_vector(std::move(input));
            static_assert(std::is_same_v<decltype(v), std::vector<emu_test::Spy>>);

            // If a vector is moved, elements are not copied nor moved.
            EXPECT_FALSE(flag.copied);
            EXPECT_FALSE(flag.moved);
        }

        {
            emu_test::spy_flag flag;
            // If it is from a rvalue of requested type, sinply move it.
            std::vector<emu_test::Spy> input; input.emplace_back(&flag);

            auto v = emu::as_vector<emu_test::Spy>(std::move(input));
            static_assert(std::is_same_v<decltype(v), std::vector<emu_test::Spy>>);

            // If a vector is moved, elements are not copied nor moved.
            EXPECT_FALSE(flag.copied);
            EXPECT_FALSE(flag.moved);
        }
    }
} // namespace
