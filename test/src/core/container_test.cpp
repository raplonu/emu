#include <gtest/gtest.h>

#include <emu/container.h>

#include <utility_test.h>

namespace
{
    TEST(ContainerTest, WrapTest)
    {
        std::array<int, 5> arr = {1, 2, 3, 4, 5};

        auto con = emu::container::wrap(arr.data(), arr.size());

        EXPECT_EQ(con.data(), arr.data());
        EXPECT_EQ(con.size(), arr.size());

    }

    TEST(ContainerTest, CreateTest)
    {
        auto con = emu::container::create<int>(5);

        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }

    TEST(ContainerTest, CapsuleTest)
    {
        std::array<int, 5> arr = {1, 2, 3, 4, 5};

        bool called = false;

        {

            emu::container_t<int> con(arr.data(), arr.size(), emu::scoped::create([&called]() { called = true; }));

        }

        EXPECT_TRUE(called);
    }

    TEST(ContainerTest, CaptureTest)
    {
        std::array<int, 5> arr = {1, 2, 3, 4, 5};

        auto con = emu::container::wrap(arr);

        EXPECT_EQ(con.data(), arr.data());
        EXPECT_EQ(con.size(), arr.size());
    }

    struct SpyContainer : emu::Spy2
    {
        SpyContainer(emu::spy_flag* sf, int *ptr):
            emu::Spy2(sf), ptr(ptr)
        {}

        int* ptr;

        int* data()
        {
            return ptr;
        }

        const int* data() const
        {
            return ptr;
        }

        std::size_t size() const
        {
            return 1;
        }

    };

    TEST(ContainerTest, MoveTest)
    {
        emu::spy_flag sf;

        int value = 42;
        SpyContainer spy_con{&sf, &value};

        {
            // Take ownership of the container.
            auto con = emu::container::wrap(std::move(spy_con));

            EXPECT_EQ(con.data()[0], 42);
            value = 0;
            EXPECT_EQ(con.data()[0], 0);

            // The container is moved away.
            EXPECT_TRUE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_TRUE(sf.destroyed);

    }

    TEST(ContainerTest, ReferenceContainerTest)
    {
        emu::spy_flag sf;

        int value = 42;
        SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by reference.
            auto con = emu::container::wrap(spy_con);

            EXPECT_EQ(con.data()[0], 42);
            value = 0;
            EXPECT_EQ(con.data()[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_FALSE(sf.destroyed);

    }

    TEST(ContainerTest, ConstReferenceContainerTest)
    {
        emu::spy_flag sf;

        int value = 42;
        const SpyContainer spy_con{&sf, &value};

        {
            // Take spy_con by constant reference.
            auto con = emu::container::wrap(spy_con);

            EXPECT_EQ(con.data()[0], 42);
            value = 0;
            EXPECT_EQ(con.data()[0], 0);

            // The container not moved nor copied.
            EXPECT_FALSE(sf.moved);
            EXPECT_FALSE(sf.copied);
        }

        EXPECT_FALSE(sf.destroyed);

    }

} // namespace
