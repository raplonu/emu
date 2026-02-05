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

    TEST(Pointer, StackPointerDescriptor)
    {
        const int x = 42;
        auto desc_opt = emu::pointer_descritor_of(reinterpret_cast<const emu::byte*>(&x));
        ASSERT_TRUE(desc_opt.has_value());
        const auto& desc = desc_opt.value();
        EXPECT_EQ(desc.location, "[stack]");

        // Check that the pointer is within the returned region
        auto ptr_addr = reinterpret_cast<uintptr_t>(&x);
        auto region_start = reinterpret_cast<uintptr_t>(desc.base_region.data());
        auto region_end = region_start + desc.base_region.size();
        EXPECT_GE(ptr_addr, region_start);
        EXPECT_LT(ptr_addr, region_end);
    }

} // namespace
