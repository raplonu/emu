#include <gtest/gtest.h>
#include <test_utility/spy.hpp>

#include <emu/functor.hpp>

#include <emu/optional.hpp>
#include <emu/expected.hpp>

#include <vector>

namespace
{
    TEST(Functor, Vector)
    {
        {
            std::vector vec = {1, 2, 3, 4, 5};
            auto res = emu::map(vec, [](int i) { return i * 2; });

            auto expected_result = std::vector{2, 4, 6, 8, 10};
            EXPECT_EQ(res, expected_result);
        }

        {
            std::vector vec = {1, 2, 3, 4, 5};

            auto res = emu::map(vec, [](int i) { return i % 2 == 0; });

            auto expected_result = std::vector{false, true, false, true, false};
            EXPECT_EQ(res, expected_result);
        }

    }

    TEST(Functor, Optional)
    {

        {

            std::optional<int> opt = 42;

            auto res = emu::map(opt, [](int i) { return i * 2; });

            ASSERT_TRUE(res.has_value());
            EXPECT_EQ(*res, 84); // NOLINT(bugprone-unchecked-optional-access) It is checked.
        }

        {
            std::optional<int> opt = std::nullopt;

            auto res = emu::map(opt, [](int i) { return i * 2; });

            EXPECT_EQ(res, std::nullopt);
        }

    }

    TEST(Functor, Expected)
    {

        {

            emu::expected<int, std::string> e = 42;

            auto res = emu::map(e, [](int i) { return i * 2; });

            EXPECT_EQ(*res, 84);
        }

        {
            emu::expected<int, std::string> e(emu::unexpect, "error");

            auto res = emu::map(e, [](int i) { return i * 2; });

            EXPECT_EQ(res.error(), "error");
        }

    }

    TEST(Functor, As)
    {

        {
            const int i = 42;

            auto res = emu::as<double>(i);

            static_assert(std::is_same_v<decltype(res), double>);

            EXPECT_DOUBLE_EQ(res, 42.0);
        }

    }

} // namespace
