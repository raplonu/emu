#include <gtest/gtest.h>
#include <emu/cstring_view.hpp>

#include <fmt/format.h>

#include <string>

namespace
{
    TEST(CStringView, FromLiteral)
    {
        const emu::cstring_view csv("Hello, World!");
        EXPECT_EQ(csv.size(), 13);
        EXPECT_EQ(csv[0], 'H');
        EXPECT_EQ(csv[12], '!');
    }

    TEST(CStringView, FromString)
    {
        const std::string str = "Hello, World!";
        const emu::cstring_view csv(str);
        EXPECT_EQ(csv.size(), 13);
        EXPECT_EQ(csv[0], 'H');
        EXPECT_EQ(csv[12], '!');
    }

    TEST(CStringView, Format)
    {
        const emu::cstring_view csv("Hello, World!");
        const std::string str = fmt::format("{}", csv);
        EXPECT_EQ(str, "Hello, World!");
    }

} // namespace
