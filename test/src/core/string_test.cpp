#include <gtest/gtest.h>

#include <emu/string.h>

using namespace emu;

namespace
{
    TEST(string_test, empty_str_with_char) {
        std::string str("");
        char token = ' ';

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(res[0].size(), 0);
        EXPECT_EQ(res[0].data(), str.data());
    }

    TEST(string_test, empty_str_with_str) {
        std::string str("");
        std::string token(" ");

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(res[0].size(), 0);
        EXPECT_EQ(res[0].data(), str.data());
    }

    TEST(string_test, non_empty_str_with_char_no_split) {
        std::string str("x");
        char token = ' ';

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0].data(), str.data());
    }

    TEST(string_test, non_empty_str_with_str_no_split) {
        std::string str("x");
        std::string token(" ");

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 1);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0].data(), str.data());
    }

    TEST(string_test, non_empty_str_with_char_empty_split) {
        std::string str("x");
        char token = 'x';

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 0);
        EXPECT_EQ(res[1].size(), 0);
    }

    TEST(string_test, non_empty_str_with_str_empty_split) {
        std::string str("x");
        std::string token("x");

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 0);
        EXPECT_EQ(res[1].size(), 0);
    }


    TEST(string_test, non_empty_str_with_char_non_empty_split) {
        std::string str("axa");
        char token = 'x';

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0].data(), str.data());
        EXPECT_EQ(res[1].size(), 1);
        EXPECT_EQ(res[1].data(), str.data() + 2);
    }

    TEST(string_test, non_empty_str_with_str_non_empty_split) {
        std::string str("axa");
        std::string token("x");

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0].data(), str.data());
        EXPECT_EQ(res[1].size(), 1);
        EXPECT_EQ(res[1].data(), str.data() + 2);
    }

    TEST(string_test, non_empty_str_with_long_str_non_empty_split) {
        std::string str("axxxa");
        std::string token("xxx");

        std::vector<emu::string_view> res = split(str, token);

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0].data(), str.data());
        EXPECT_EQ(res[1].size(), 1);
        EXPECT_EQ(res[1].data(), str.data() + 4 );
    }

    TEST(string_test, split_literal) {
        std::vector<emu::string_view> res = split("axxxb", "xxx");

        EXPECT_EQ(res.size(), 2);
        EXPECT_EQ(res[0].size(), 1);
        EXPECT_EQ(res[0][0], 'a');
        EXPECT_EQ(res[1].size(), 1);
        EXPECT_EQ(res[1][0], 'b');
    }


}

