#include <gtest/gtest.h>

#include <emu/capsule.hpp>
#include <emu/scoped.hpp>

using namespace emu;

// NOLINTBEGIN(performance-*)

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
            const capsule cap;

            EXPECT_EQ(cap.use_count(), 0);

            const capsule cap2 = cap;

            EXPECT_EQ(cap.use_count(), 0);
            EXPECT_EQ(cap2.use_count(), 0);
        }

        {
            const capsule cap;
            const auto& c_ref = cap;
            const capsule cap2 = c_ref;

            EXPECT_EQ(cap2.use_count(), 0);
        }

        {
            auto capsule_factory = [] { return capsule(); };
            const capsule cap(capsule_factory());

            EXPECT_EQ(cap.use_count(), 0);
        }

    }

    TEST(Capsule, SimpleValue)
    {
        // put a random value into capsule
        const capsule cap(0);

        EXPECT_EQ(cap.use_count(), 1);
    }

    TEST(Capsule, ManagedValue)
    {
        auto managed_value = std::make_shared<int>(0);
        EXPECT_EQ(managed_value.use_count(), 1);

        // capsule does not copy lvalue by default.
        // object copy needs to be explicitly requested.
        capsule cap = std::shared_ptr(managed_value);
        EXPECT_EQ(cap.use_count(), 1);
        EXPECT_EQ(managed_value.use_count(), 2);

        capsule cap2 = cap;
        EXPECT_EQ(cap.use_count(), 2);
        EXPECT_EQ(cap2.use_count(), 2);
        // New capsule does not copy the object.
        EXPECT_EQ(managed_value.use_count(), 2);

        cap.reset();
        EXPECT_EQ(cap.use_count(), 0);

        cap2.reset();
        EXPECT_EQ(cap2.use_count(), 0);

        EXPECT_EQ(managed_value.use_count(), 1);
    }

    TEST(Capsule, Scoped)
    {
        int called_nb = 0;
        scoped incr_at_destruction([&]{ called_nb++; });

        capsule cap = std::move(incr_at_destruction);

        EXPECT_EQ(called_nb, 0);

        cap.reset();

        EXPECT_EQ(called_nb, 1);
    }

} // namespace

// NOLINTEND(performance-*)
