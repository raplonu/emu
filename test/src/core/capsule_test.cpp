#include <gtest/gtest.h>

#include <emu/capsule.hpp>
#include <emu/scoped.hpp>

#include <test_utility/spy.hpp>

using namespace emu;

// NOLINTBEGIN(performance-*)

namespace
{

    template<typename T>
    concept construct_capsule_from = std::is_constructible_v<capsule, T>;

    // These tests are not relevent anymore since capsule accept type with value semantics like shared_ptr.
    TEST(Capsule, StaticTest)
    {
        static_assert(construct_capsule_from<int>);

        // Cannot build capsule from lvalue
        static_assert(not construct_capsule_from<int&>);
        static_assert(not construct_capsule_from<const int&>);

        // Can build capsule from rvalue (move)
        static_assert(construct_capsule_from<int&&>);
        static_assert(construct_capsule_from<const int&&>);

        // Can build capsule from explicit copy
        static_assert(construct_capsule_from<copy<int>>);
        static_assert(construct_capsule_from<copy<const int>>);
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
        capsule cap{ copy(managed_value) };
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

    struct CapsuleHolder
    {
        emu::capsule cap;

        emu::capsule& capsule() { return cap; }
    };

    TEST(Capsule, CapsuleHolder)
    {
        // Create a CapsuleHolder with a capsule that holder a single int.
        CapsuleHolder data{ emu::capsule(0) };
        EXPECT_EQ(data.capsule().use_count(), 1);

        // Create a new capsule from the CapsuleHolder. Capsule detect that CapsuleHolder hold a capsule and use it.
        const emu::capsule cap2( data );
        EXPECT_EQ(data.capsule().use_count(), 2);
        EXPECT_EQ(cap2.use_count(), 2);
        EXPECT_EQ(data.capsule().holder, cap2.holder);
    }

    struct CapsuleDerived : std::span<int>, capsule
    {
        using base = std::span<int>;

        CapsuleDerived(std::vector<int> v)
            : base(v)
            , capsule(std::move(v))
        {}
    };

    TEST(Capsule, CapsuleDerived)
    {
        CapsuleDerived cap1(std::vector<int>{1, 2, 3});
        EXPECT_EQ(cap1.use_count(), 1);
        EXPECT_EQ(cap1.size(), 3);

        // cap2 shares the same object
        // NOLINTNEXTLINE(cppcoreguidelines-slicing) - I know what I'm doing
        const capsule cap2(cap1);
        EXPECT_EQ(cap1.use_count(), 2);

        // cap3 stole the object from cap1
        const capsule cap3 = std::move(cap1);
        EXPECT_EQ(cap3.use_count(), 2);
    }

    TEST(Capsule, Scoped)
    {
        int called_nb = 0;
        scoped incr_at_destruction([&]{ called_nb++; });

        capsule cap{ std::move(incr_at_destruction) };

        EXPECT_EQ(called_nb, 0);

        cap.reset();

        EXPECT_EQ(called_nb, 1);
    }

    TEST(Capsule, MakeCapsule)
    {
        emu::capsule cap;

        auto& array = cap.emplace<std::array<int, 3>>();

        EXPECT_EQ(cap.use_count(), 1);
        EXPECT_EQ(array.size(), 3);
    }

    TEST(Capsule, MakeCapsuleNoCopy)
    {
        emu_test::SpyAddress *snitch = nullptr;

        {
            emu::capsule cap;

            auto& sa = cap.emplace<emu_test::SpyAddress>(&snitch);

            EXPECT_EQ(cap.use_count(), 1);
            EXPECT_EQ(snitch, &sa) << "The returned ref should have same address than the one snitched by SpyAddress constructor";
        }

        EXPECT_EQ(snitch, nullptr) << "The SpyAddress instance is no more, it sould have snitched one last time and set snitch to nullptr";

    }

} // namespace

// NOLINTEND(performance-*)
