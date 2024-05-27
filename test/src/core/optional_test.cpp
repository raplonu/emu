#include <gtest/gtest.h>

#include <emu/optional.hpp>

using namespace emu;

template<typename T>
struct Toto;

namespace
{
    TEST(optional_test, optional_as) {
        optional<float> o1(42.5), o2(nullopt), o3(in_place);

        EXPECT_EQ(o1.map(as<int>).has_value(), true);
        EXPECT_EQ(*o1.map(as<int>), 42);

        EXPECT_EQ(o2.map(as<int>).has_value(), false);

        EXPECT_EQ(o3.map(as<int>).has_value(), true);
        EXPECT_EQ(*o3.map(as<int>), 0);

        EXPECT_EQ(as<int>(o1).has_value(), true);
        EXPECT_EQ(*as<int>(o1), 42);

        EXPECT_EQ(as<int>(o2).has_value(), false);

        EXPECT_EQ(as<int>(o3).has_value(), true);
        EXPECT_EQ(*as<int>(o3), 0);
    }

} // namespace
