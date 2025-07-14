#include <gtest/gtest.h>
#include <emu/string.hpp>
#include <vector>
#include <string>

namespace
{
    TEST(String, FromChars)
    {
        int value = 0;
        auto result = emu::from_chars("42", value);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(value, 42);
        EXPECT_TRUE(result.value().empty());
    }

    TEST(String, FromCharsWithRemaining)
    {
        int value = 0;
        auto result = emu::from_chars("42 and more", value);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(value, 42);
        EXPECT_EQ(result.value(), " and more");
    }

    TEST(String, FromCharsHex)
    {
        int value = 0;
        auto result = emu::from_chars("2a", value, 16);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(value, 42);
        EXPECT_TRUE(result.value().empty());
    }

    TEST(String, FromCharsInvalid)
    {
        int value = 0;
        auto result = emu::from_chars("not a number", value);
        EXPECT_FALSE(result.has_value());
    }

    TEST(String, SplitString)
    {
        const std::string test_str = "this is a test";
        auto parts = emu::split_string(test_str);
        const std::vector<std::string_view> expected = {"this", "is", "a", "test"};
        auto it = parts.begin();
        for(const auto& exp : expected)
        {
            ASSERT_NE(it, parts.end());
            EXPECT_EQ(*it++, exp);
        }
        EXPECT_EQ(it, parts.end());
    }

    TEST(String, SplitStringWithEmpty)
    {
        const std::string test_str = "this  is a test";
        auto parts = emu::split_string(test_str);
        const std::vector<std::string_view> expected = {"this", "", "is", "a", "test"};
        auto it = parts.begin();
        for(const auto& exp : expected)
        {
            ASSERT_NE(it, parts.end());
            EXPECT_EQ(*it++, exp);
        }
        EXPECT_EQ(it, parts.end());
    }

    TEST(String, SplitStringNoEmpty)
    {
        const std::string test_str = "this  is a test";
        auto parts = emu::split_string_no_empty(test_str);
        const std::vector<std::string_view> expected = {"this", "is", "a", "test"};
        auto it = parts.begin();
        for(const auto& exp : expected)
        {
            ASSERT_NE(it, parts.end());
            EXPECT_EQ(*it++, exp);
        }
        EXPECT_EQ(it, parts.end());
    }

    TEST(String, SplitStringCustomDelim)
    {
        const std::string test_str = "this,is,a,test";
        auto parts = emu::split_string(test_str, ",");
        const std::vector<std::string_view> expected = {"this", "is", "a", "test"};
        auto it = parts.begin();
        for(const auto& exp : expected)
        {
            ASSERT_NE(it, parts.end());
            EXPECT_EQ(*it++, exp);
        }
        EXPECT_EQ(it, parts.end());
    }

} // namespace
