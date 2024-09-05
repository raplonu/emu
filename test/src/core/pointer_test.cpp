#include <gtest/gtest.h>
#include <emu/pointer.hpp>

#include <memory>

namespace
{
    TEST(Pointer, StackPointer)
    {
        const int x = 42;

        auto device = emu::get_device_of_pointer(&x);

        EXPECT_TRUE(device.has_value());

        auto device_value = device.value();

        EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCPU);
        EXPECT_EQ(device_value.device_id, 0);
    }

    TEST(Pointer, HeapPointer)
    {
        auto ptr = std::make_unique<int>(42);

        auto device = emu::get_device_of_pointer(ptr.get());

        EXPECT_TRUE(device.has_value());

        auto device_value = device.value();

        EXPECT_EQ(device_value.device_type, emu::dlpack::device_type_t::kDLCPU);
        EXPECT_EQ(device_value.device_id, 0);

    }

} // namespace
