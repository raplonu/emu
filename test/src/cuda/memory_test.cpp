#include <gtest/gtest.h>
#include <emu/cuda/memory.hpp>
#include <emu/cuda/device.hpp>

TEST(CudaMemoryTest, DeviceMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::memory::device::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}

TEST(CudaMemoryTest, HostMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::memory::host::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}

TEST(CudaMemoryTest, ManagedMemory)
{
    auto device = emu::cuda::device::current();
    auto ptr = emu::cuda::memory::managed::make_unique<int[]>(device, 10);
    ASSERT_NE(ptr, nullptr);
}
