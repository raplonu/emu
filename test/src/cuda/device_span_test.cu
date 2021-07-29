#include <gtest/gtest.h>

#include <emu/cuda/span.h>

#include <thrust/device_vector.h>

namespace
{
    TEST(DeviceSpan, EmptySpan)
    {
        emu::cuda::span_t<int> s;

        EXPECT_EQ(s.data(), nullptr);
        EXPECT_EQ(s.size(), 0);
        EXPECT_EQ(s.location().device, emu::cuda::device::current::get());
    }

    TEST(DeviceSpan, ThrustSpanAsRawPtr)
    {
        thrust::device_vector<int> v(5);

        emu::cuda::span_t<int> s(v.data().get(), v.size(), emu::location::cuda_t{});

        EXPECT_EQ(s.data(), v.data().get());
        EXPECT_EQ(s.size(), 5);
        EXPECT_EQ(s.location().device, emu::cuda::device::current::get());
    }

}

