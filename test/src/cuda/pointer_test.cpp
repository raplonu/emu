#include <gtest/gtest.h>
#include <emu/pointer.hpp>
#include <emu/cuda.hpp>

#include <memory>

namespace
{

    TEST(Pointer, HostHeapPointer)
    {
        auto ptr = std::make_unique<int>(42);

        auto device = emu::get_device_of_pointer(ptr.get());

        EXPECT_TRUE(device.has_value());

        auto device_value = device.value();

        EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCPU);
        EXPECT_EQ(device_value.device_id, 0);

    }

    TEST(Pointer, DeviceHeapPointer)
    {
        auto ptr = emu::cuda::memory::device::make_unique<int[]>(emu::cuda::device::get(0), 1);

        auto device = emu::get_device_of_pointer(ptr.get());

        EXPECT_TRUE(device.has_value());

        auto device_value = device.value();

        EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDA);
        EXPECT_EQ(device_value.device_id, 0);
    }

    TEST(Pointer, DeviceHeapPointerMultiDevice)
    {
        if (emu::cuda::devices::count() > 1)
        {
            auto ptr = emu::cuda::memory::device::make_unique<int[]>(emu::cuda::device::get(1), 1);

            auto device = emu::get_device_of_pointer(ptr.get());

            EXPECT_TRUE(device.has_value());

            auto device_value = device.value();

            EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDA);
            EXPECT_EQ(device_value.device_id, 1);
        } else
            GTEST_SKIP() << "Skipping test as there is only one CUDA device available";
    }

    TEST(Pointer, DevicePointerDescriptor)
    {
        auto ptr = emu::cuda::memory::device::make_unique<int[]>(emu::cuda::device::get(0), 1);
        auto desc_opt = emu::pointer_descritor_of(reinterpret_cast<const emu::byte*>(ptr.get()));
        ASSERT_TRUE(desc_opt.has_value());
        auto desc = desc_opt.value();
        EXPECT_EQ(desc.location, "[anonymous]");

        // Check that the pointer is within the returned region
        auto ptr_addr = reinterpret_cast<uintptr_t>(ptr.get());
        auto region_start = reinterpret_cast<uintptr_t>(desc.base_region.data());
        auto region_end = region_start + desc.base_region.size();
        EXPECT_GE(ptr_addr, region_start);
        EXPECT_LT(ptr_addr, region_end);
    }

} // namespace
