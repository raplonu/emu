#include <gtest/gtest.h>
#include <utility_test.hpp>

#include <emu/container.hpp>
#include <emu/scoped.hpp>

#include <emu/info.hpp>
#include <emu/mdspan.hpp>

struct Toto {

};

auto format_as(const Toto& t) {
    return "toto";
}

namespace
{
    TEST(Container, Construct)
    {
        {
            emu::container<int> con;

            EXPECT_EQ(con.data(), nullptr);
            EXPECT_EQ(con.size(), 0);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            int arr[5] = {1, 2, 3, 4, 5};
            emu::container con = arr;

            EXPECT_EQ(con.data(), arr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            std::array arr = {1, 2, 3, 4, 5};
            emu::container con = arr;

            EXPECT_EQ(con.data(), arr.data());
            EXPECT_EQ(con.size(), arr.size());
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            //It's not possible to construct a container from a std::array rvalue.
            //because it is not possible to get the pointer after moving the array
            //into the capsule.
            static_assert(not std::constructible_from<emu::container<int>, std::array<int, 5>>);
            static_assert(not std::constructible_from<emu::container<int>, std::array<int, 5>&&>);
        }
        {
            std::vector vec = {1, 2, 3, 4, 5};
            emu::container con = vec;

            EXPECT_EQ(con.data(), vec.data());
            EXPECT_EQ(con.size(), vec.size());
            EXPECT_EQ(con.use_count(), 0);
        }
        {
            emu::container con = std::vector{1, 2, 3, 4, 5};

            EXPECT_NE(con.data(), nullptr);
            EXPECT_EQ(con.size(), 5);
            EXPECT_EQ(con.use_count(), 1);
        }
    }

    TEST(Container, Wrap)
    {
        std::array<int, 5> arr = {1, 2, 3, 4, 5};

        emu::container con = arr;

        EXPECT_EQ(con.data(), arr.data());
        EXPECT_EQ(con.size(), arr.size());
        EXPECT_EQ(con.use_count(), 0);
    }

    TEST(Container, AllocateSimple)
    {
        auto con = emu::make_container<int>(5);

        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }

    TEST(Container, FreeCapsule)
    {
        std::array<int, 5> arr = {1, 2, 3, 4, 5};

        bool called = false;

        {
            emu::container con(arr.data(), arr.size(), emu::scoped([&called]() { called = true; }));
        }

        EXPECT_TRUE(called);
    }

    TEST(Container, TakeOwnership)
    {
        emu::spy_flag sf;

        int value = 42;
        emu::SpyContainer spy_con{&sf, &value};

        {
            // Take ownership of the container.
            emu::container con = std::move(spy_con);

            EXPECT_EQ(con.use_count(), 1);

            EXPECT_EQ(con[0], 42);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container is moved away.
            EXPECT_TRUE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_TRUE(sf.destroyed);

    }

    TEST(Container, TakeRef)
    {
        emu::spy_flag sf;

        int value = 42;
        emu::SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by reference.
            emu::container con = spy_con;

            EXPECT_EQ(con.use_count(), 0);

            EXPECT_EQ(con[0], 42);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        // and not destroyed.
        EXPECT_FALSE(sf.destroyed);

    }

    TEST(Container, ConstReferenceContainerTest)
    {
        emu::spy_flag sf;

        int value = 42;
        const emu::SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by constant reference.
            emu::container con = spy_con;

            EXPECT_EQ(con.use_count(), 0);

            EXPECT_EQ(con[0], 42);
            value = 0;
            EXPECT_EQ(con[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_FALSE(sf.destroyed);
    }

} // namespace
