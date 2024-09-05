#include <gtest/gtest.h>

#include <emu/macro.cuh>
#include <emu/math.hpp>
#include <emu/cuda.hpp>
#include <emu/type_name.hpp>
#include <emu/ostream.hpp>
#include <thrust/device_vector.h>

using namespace emu;

long double operator "" _ld(long double ld) { return ld; }

template<typename To, typename From>
EMU_GLOB void ker(To * to, From * from) {
    to[0] = math::ceil <To>(from[0]);
    to[1] = math::floor<To>(from[1]);
    to[2] = math::round<To>(from[2]);
    to[3] = math::trunc<To>(from[3]);
}

template<typename To, typename From>
void generic_round_test(From from_0, To to_0, From from_1, To to_1, From from_2, To to_2, From from_3, To to_3) {
    thrust::device_vector<From> from_v(4);
    thrust::device_vector<To>   to_v  (4);

    from_v[0] = from_0;
    from_v[1] = from_1;
    from_v[2] = from_2;
    from_v[3] = from_3;

    ker<<<1,1>>>(to_v.data().get(), from_v.data().get());
    ::cuda::device::current::get().synchronize();

    EXPECT_EQ(to_v[0], to_0) << "from_0: " << emu::numeric_name<From> << " to_0: " << emu::numeric_name<To>;
    EXPECT_EQ(to_v[1], to_1) << "from_1: " << emu::numeric_name<From> << " to_1: " << emu::numeric_name<To>;
    EXPECT_EQ(to_v[2], to_2) << "from_2: " << emu::numeric_name<From> << " to_2: " << emu::numeric_name<To>;
    EXPECT_EQ(to_v[3], to_3) << "from_3: " << emu::numeric_name<From> << " to_3: " << emu::numeric_name<To>;
}

namespace
{
    TEST(math_test, device_math_as_round_same_types) {
        generic_round_test(
            1.1f, 2.f,
            1.5f, 1.f,
            1.5f, 2.f,
            1.8f, 1.f
        );

        generic_round_test(
            1.1, 2.,
            1.5, 1.,
            1.5, 2.,
            1.8, 1.
        );

    }

    TEST(math_test, device_math_as_round_different_types) {
        // float to int
        generic_round_test(
            1.1f, 2,
            1.5f, 1,
            1.5f, 2,
            1.8f, 1
        );

        // double to int
        generic_round_test(
            1.1, 2,
            1.5, 1,
            1.5, 2,
            1.8, 1
        );

        // int to double
        generic_round_test(
            2, 2.,
            1, 1.,
            2, 2.,
            1, 1.
        );

    }

}
