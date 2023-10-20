// #include <gtest/gtest.h>

// #include <emu/math.h>

// using namespace emu;

// long double operator "" _ld(long double ld) { return ld; }

// namespace
// {
//     TEST(math_test, host_math_round) {
//         EXPECT_EQ(math::std::ceil (1.1f), 2.0f);
//         EXPECT_EQ(math::std::floor(1.5f), 1.0f);
//         EXPECT_EQ(math::std::round(1.5f), 2.0f);
//         EXPECT_EQ(math::std::trunc(1.8f), 1.0f);

//         EXPECT_EQ(math::std::ceil (1.1), 2.0);
//         EXPECT_EQ(math::std::floor(1.5), 1.0);
//         EXPECT_EQ(math::std::round(1.5), 2.0);
//         EXPECT_EQ(math::std::trunc(1.8), 1.0);

//         EXPECT_EQ(math::std::ceil (1.1_ld), 2.0_ld);
//         EXPECT_EQ(math::std::floor(1.5_ld), 1.0_ld);
//         EXPECT_EQ(math::std::round(1.5_ld), 2.0_ld);
//         EXPECT_EQ(math::std::trunc(1.8_ld), 1.0_ld);

//         EXPECT_EQ(math::std::ceil (-1.1f), -1.0f);
//         EXPECT_EQ(math::std::floor(-1.5f), -2.0f);
//         EXPECT_EQ(math::std::round(-1.5f), -2.0f);
//         EXPECT_EQ(math::std::trunc(-1.8f), -1.0f);

//         EXPECT_EQ(math::std::ceil (-1.1), -1.0);
//         EXPECT_EQ(math::std::floor(-1.5), -2.0);
//         EXPECT_EQ(math::std::round(-1.5), -2.0);
//         EXPECT_EQ(math::std::trunc(-1.8), -1.0);

//         EXPECT_EQ(math::std::ceil (-1.1_ld), -1.0_ld);
//         EXPECT_EQ(math::std::floor(-1.5_ld), -2.0_ld);
//         EXPECT_EQ(math::std::round(-1.5_ld), -2.0_ld);
//         EXPECT_EQ(math::std::trunc(-1.8_ld), -1.0_ld);
//     }

//     TEST(math_test, host_math_as_round) {
//         EXPECT_EQ(math::as<float>.ceil (1.1f), 1.1f);
//         EXPECT_EQ(math::as<float>.floor(1.5f), 1.5f);
//         EXPECT_EQ(math::as<float>.round(1.5f), 1.5f);
//         EXPECT_EQ(math::as<float>.trunc(1.8f), 1.8f);

//         EXPECT_EQ(math::as<double>.ceil (1.1), 1.1);
//         EXPECT_EQ(math::as<double>.floor(1.5), 1.5);
//         EXPECT_EQ(math::as<double>.round(1.5), 1.5);
//         EXPECT_EQ(math::as<double>.trunc(1.8), 1.8);

//         EXPECT_EQ(math::as<long double>.ceil (1.1_ld), 1.1_ld);
//         EXPECT_EQ(math::as<long double>.floor(1.5_ld), 1.5_ld);
//         EXPECT_EQ(math::as<long double>.round(1.5_ld), 1.5_ld);
//         EXPECT_EQ(math::as<long double>.trunc(1.8_ld), 1.8_ld);

//         EXPECT_EQ(math::as<float>.ceil (-1.1f), -1.1f);
//         EXPECT_EQ(math::as<float>.floor(-1.5f), -1.5f);
//         EXPECT_EQ(math::as<float>.round(-1.5f), -1.5f);
//         EXPECT_EQ(math::as<float>.trunc(-1.8f), -1.8f);

//         EXPECT_EQ(math::as<double>.ceil (-1.1), -1.1);
//         EXPECT_EQ(math::as<double>.floor(-1.5), -1.5);
//         EXPECT_EQ(math::as<double>.round(-1.5), -1.5);
//         EXPECT_EQ(math::as<double>.trunc(-1.8), -1.8);

//         EXPECT_EQ(math::as<long double>.ceil (-1.1_ld), -1.1_ld);
//         EXPECT_EQ(math::as<long double>.floor(-1.5_ld), -1.5_ld);
//         EXPECT_EQ(math::as<long double>.round(-1.5_ld), -1.5_ld);
//         EXPECT_EQ(math::as<long double>.trunc(-1.8_ld), -1.8_ld);
//     }

// }
