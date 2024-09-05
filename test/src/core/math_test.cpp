#include <gtest/gtest.h>

#include <emu/math.hpp>

using namespace emu;

long double operator "" _ld(long double ld) { return ld; }

namespace
{
    TEST(math_test, std_math_round) {
        EXPECT_EQ(math::std::ceil (1.1f), 2.0f);
        EXPECT_EQ(math::std::floor(1.5f), 1.0f);
        EXPECT_EQ(math::std::round(1.5f), 2.0f);
        EXPECT_EQ(math::std::trunc(1.8f), 1.0f);

        EXPECT_EQ(math::std::ceil (1.1), 2.0);
        EXPECT_EQ(math::std::floor(1.5), 1.0);
        EXPECT_EQ(math::std::round(1.5), 2.0);
        EXPECT_EQ(math::std::trunc(1.8), 1.0);

        EXPECT_EQ(math::std::ceil (1.1_ld), 2.0_ld);
        EXPECT_EQ(math::std::floor(1.5_ld), 1.0_ld);
        EXPECT_EQ(math::std::round(1.5_ld), 2.0_ld);
        EXPECT_EQ(math::std::trunc(1.8_ld), 1.0_ld);

        EXPECT_EQ(math::std::ceil (-1.1f), -1.0f);
        EXPECT_EQ(math::std::floor(-1.5f), -2.0f);
        EXPECT_EQ(math::std::round(-1.5f), -2.0f);
        EXPECT_EQ(math::std::trunc(-1.8f), -1.0f);

        EXPECT_EQ(math::std::ceil (-1.1), -1.0);
        EXPECT_EQ(math::std::floor(-1.5), -2.0);
        EXPECT_EQ(math::std::round(-1.5), -2.0);
        EXPECT_EQ(math::std::trunc(-1.8), -1.0);

        EXPECT_EQ(math::std::ceil (-1.1_ld), -1.0_ld);
        EXPECT_EQ(math::std::floor(-1.5_ld), -2.0_ld);
        EXPECT_EQ(math::std::round(-1.5_ld), -2.0_ld);
        EXPECT_EQ(math::std::trunc(-1.8_ld), -1.0_ld);
    }

    TEST(math_test, host_math_as_round) {
        EXPECT_EQ(math::ceil<float> (1.1f), 2.f);
        EXPECT_EQ(math::floor<float>(1.5f), 1.f);
        EXPECT_EQ(math::round<float>(1.5f), 2.f);
        EXPECT_EQ(math::trunc<float>(1.8f), 1.f);

        EXPECT_EQ(math::ceil<double> (1.1), 2.);
        EXPECT_EQ(math::floor<double>(1.5), 1.);
        EXPECT_EQ(math::round<double>(1.5), 2.);
        EXPECT_EQ(math::trunc<double>(1.8), 1.);

        EXPECT_EQ(math::ceil<long double> (1.1_ld), 2._ld);
        EXPECT_EQ(math::floor<long double>(1.5_ld), 1._ld);
        EXPECT_EQ(math::round<long double>(1.5_ld), 2._ld);
        EXPECT_EQ(math::trunc<long double>(1.8_ld), 1._ld);

        EXPECT_EQ(math::ceil<float> (-1.1f), -1.f);
        EXPECT_EQ(math::floor<float>(-1.5f), -2.f);
        EXPECT_EQ(math::round<float>(-1.5f), -2.f);
        EXPECT_EQ(math::trunc<float>(-1.8f), -1.f);

        EXPECT_EQ(math::ceil<double> (-1.1), -1.);
        EXPECT_EQ(math::floor<double>(-1.5), -2.);
        EXPECT_EQ(math::round<double>(-1.5), -2.);
        EXPECT_EQ(math::trunc<double>(-1.8), -1.);

        EXPECT_EQ(math::ceil<long double> (-1.1_ld), -1._ld);
        EXPECT_EQ(math::floor<long double>(-1.5_ld), -2._ld);
        EXPECT_EQ(math::round<long double>(-1.5_ld), -2._ld);
        EXPECT_EQ(math::trunc<long double>(-1.8_ld), -1._ld);
    }

} // namespace
