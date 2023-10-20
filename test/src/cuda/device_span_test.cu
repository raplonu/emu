#include <gtest/gtest.h>

#include <emu/cuda/span.h>

#include <thrust/device_vector.h>

namespace
{
    TEST(DeviceSpan, EmptySpan)
    {
        emu::cuda::span<int> s;

        EXPECT_EQ(s.data(), nullptr);
        EXPECT_EQ(s.size(), 0);
        EXPECT_EQ(s.location().id, ::cuda::device::current::get().id());
    }

    TEST(DeviceSpan, ThrustSpanAsRawPtr)
    {
        thrust::device_vector<int> v(5);

        emu::cuda::span<int> s(v.data().get(), v.size(), emu::cuda::location::device{});

        EXPECT_EQ(s.data(), v.data().get());
        EXPECT_EQ(s.size(), 5);
        EXPECT_EQ(s.location().id, ::cuda::device::current::get().id());
    }

}

