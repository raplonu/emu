// #include <gtest/gtest.h>

// #include <emu/macro.cuh>
// #include <emu/math.h>
// #include <emu/cuda.hpp>
// #include <thrust/device_vector.h>

// using namespace emu;

// long double operator "" _ld(long double ld) { return ld; }

// template<typename To, typename From>
// EMU_GLOB void ker(To * to, From * from) {
//     to[0] = math::as<To>.ceil (from[0]);
//     to[1] = math::as<To>.floor(from[1]);
//     to[2] = math::as<To>.round(from[2]);
//     to[3] = math::as<To>.trunc(from[3]);
// }

// template<typename To, typename From>
// void generic_round_test(From from_0, To to_0, From from_1, To to_1, From from_2, To to_2, From from_3, To to_3) {
//     thrust::device_vector<From> from_v(4);
//     thrust::device_vector<To>   to_v  (4);

//     from_v[0] = from_0;
//     from_v[1] = from_1;
//     from_v[2] = from_2;
//     from_v[3] = from_3;

//     ker<<<1,1>>>(to_v.data().get(), from_v.data().get());
//     ::cuda::device::current::get().synchronize();

//     EXPECT_EQ(to_v[0], to_0);
//     EXPECT_EQ(to_v[1], to_1);
//     EXPECT_EQ(to_v[2], to_2);
//     EXPECT_EQ(to_v[3], to_3);
// }

// namespace
// {
//     TEST(math_test, device_math_as_round_same_types) {
//         generic_round_test(
//             1.1f, 1.1f,
//             1.5f, 1.5f,
//             1.5f, 1.5f,
//             1.8f, 1.8f
//         );

//         generic_round_test(
//             1.1, 1.1,
//             1.5, 1.5,
//             1.5, 1.5,
//             1.8, 1.8
//         );

//     }

//     TEST(math_test, device_math_as_round_different_types) {
//         // float to int
//         generic_round_test(
//             1.1f, 2,
//             1.5f, 1,
//             1.5f, 2,
//             1.8f, 1
//         );

//         // double to int
//         generic_round_test(
//             1.1, 2,
//             1.5, 1,
//             1.5, 2,
//             1.8, 1
//         );

//         // int to double
//         generic_round_test(
//             2, 2.,
//             1, 1.,
//             2, 2.,
//             1, 1.
//         );

//     }

// }
