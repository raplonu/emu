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
        cuda::device::get(0).make_current();

        {
            auto ptr = cuda::memory::device::make_unique_span<int>(1);

            auto device = emu::get_device_of_pointer(ptr.data());

            EXPECT_TRUE(device.has_value());

            auto device_value = device.value();

            EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDA);
            EXPECT_EQ(device_value.device_id, 0);
        }
    }

    TEST(Pointer, DeviceHeapPointerMultiDevice)
    {

        if (auto devices = cuda::devices(); devices.size() > 1)
        {
            devices.at(1).make_current();

            auto ptr = cuda::memory::device::make_unique_span<int>(1);

            auto device = emu::get_device_of_pointer(ptr.data());

            EXPECT_TRUE(device.has_value());

            auto device_value = device.value();

            EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDA);
            EXPECT_EQ(device_value.device_id, 1);
        } else
            GTEST_SKIP() << "Skipping test as there is only one CUDA device available";
    }

    TEST(Pointer, ManagedHeapPointer)
    {
        cuda::device::get(0).make_current();

        {
            auto ptr = cuda::memory::managed::make_unique_span<int>(1);

            auto device = emu::get_device_of_pointer(ptr.data());

            EXPECT_TRUE(device.has_value());

            auto device_value = device.value();

            EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDAManaged);
            EXPECT_EQ(device_value.device_id, 0);
        }

        // if (auto devices = cuda::devices(); devices.size() > 1)
        // {
        //     devices.at(1).make_current();

        //     auto ptr = cuda::memory::managed::make_unique_span<int>(1);

        //     auto device = emu::get_device_of_pointer(ptr.data());

        //     EXPECT_TRUE(device.has_value());

        //     auto device_value = device.value();

        //     EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCUDAManaged);
        //     EXPECT_EQ(device_value.device_id, 1);
        // } else
        //     GTEST_SKIP() << "Skipping test as there is only one CUDA device available";
    }


} // namespace
