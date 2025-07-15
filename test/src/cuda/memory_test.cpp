#include <gtest/gtest.h>
#include <emu/cuda/memory.hpp>
#include <emu/cuda/device.hpp>
#include <emu/cuda/stream.hpp>
#include <vector>
#include <numeric>
#include <algorithm>

TEST(CudaMemoryTest, DeviceMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::device::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}

TEST(CudaMemoryTest, HostMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::host::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}

TEST(CudaMemoryTest, ManagedMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::managed::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}

TEST(CudaMemoryTest, Copy)
{
    auto device = emu::cuda::device::current();
    auto stream = emu::cuda::stream::create(device);

    constexpr std::size_t n = 1024;

    auto host_src = emu::cuda::host::make_unique<int[]>(n);
    std::iota(host_src.get(), host_src.get() + n, 0);

    auto device_mem = emu::cuda::device::make_unique<int[]>(n);
    emu::cuda::copy_n(host_src.get(), n, device_mem.get(), stream);

    auto host_dst = emu::cuda::host::make_unique<int[]>(n);
    emu::cuda::copy_n(device_mem.get(), n, host_dst.get(), stream);

    stream.wait();

    for(std::size_t i = 0; i < n; ++i)
    {
        ASSERT_EQ(host_src[i], host_dst[i]);
    }
}

TEST(CudaMemoryTest, SyncCopy)
{
    auto device = emu::cuda::device::current();

    constexpr std::size_t n = 1024;

    auto host_src = emu::cuda::host::make_unique<int[]>(n);
    std::iota(host_src.get(), host_src.get() + n, 0);

    auto device_mem = emu::cuda::device::make_unique<int[]>(n);
    emu::cuda::copy_n(host_src.get(), n, device_mem.get());

    auto host_dst = emu::cuda::host::make_unique<int[]>(n);
    emu::cuda::copy_n(device_mem.get(), n, host_dst.get());

    for(std::size_t i = 0; i < n; ++i)
    {
        ASSERT_EQ(host_src[i], host_dst[i]);
    }
}
