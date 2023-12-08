#include <gtest/gtest.h>

#include <emu/capsule.hpp>
#include <emu/scoped.hpp>

#include <vector>
#include <array>

using namespace emu;

namespace
{

    template<typename T>
    concept construct_capsule_from = std::is_constructible_v<capsule, T>;

    TEST(Capsule, StaticTest)
    {
        static_assert(construct_capsule_from<int>);

        // Cannot build capsule from lvalue
        static_assert(not construct_capsule_from<int&>);
        static_assert(not construct_capsule_from<const int&>);

        // Can build capsule from rvalue (move)
        static_assert(construct_capsule_from<int&&>);
        static_assert(construct_capsule_from<const int&&>);
    }

    TEST(Capsule, EmptyCapsule)
    {
        {
            capsule c;

            EXPECT_EQ(c.use_count(), 0);

            capsule c2 = c;

            EXPECT_EQ(c.use_count(), 0);
            EXPECT_EQ(c2.use_count(), 0);
        }

        {
            capsule c;
            auto& c_ref = c;
            capsule c2 = c_ref;

            EXPECT_EQ(c2.use_count(), 0);
        }

        {
            auto l = [] { return capsule(); };
            capsule c(l());

            EXPECT_EQ(c.use_count(), 0);
        }

    }

    TEST(Capsule, SimpleCapsule)
    {
        {
            // put a random value into capsule
            capsule c(5);

            EXPECT_EQ(c.use_count(), 1);
        }

        {
            std::shared_ptr<int> p = std::make_shared<int>(5);
            EXPECT_EQ(p.use_count(), 1);

            // capsule does not copy lvalue by default.
            // object copy needs to be explicitly requested.
            capsule c = std::shared_ptr<int>(p);
            EXPECT_EQ(c.use_count(), 1);
            EXPECT_EQ(p.use_count(), 2);

            capsule c2 = c;
            EXPECT_EQ(c.use_count(), 2);
            EXPECT_EQ(c2.use_count(), 2);
            // New capsule does not copy the object.
            EXPECT_EQ(p.use_count(), 2);

            c.reset();
            EXPECT_EQ(c.use_count(), 0);

            c2.reset();
            EXPECT_EQ(c2.use_count(), 0);

            EXPECT_EQ(p.use_count(), 1);
        }

        {
            int called_nb = 0;
            scoped s([&]{ called_nb++; });

            capsule c = std::move(s);

            EXPECT_EQ(called_nb, 0);

            c.reset();

            EXPECT_EQ(called_nb, 1);
        }
    }

} // namespace