#include <gtest/gtest.h>

#include <emu/detail/dlpack_types.hpp>

#include <fmt/format.h>

using namespace emu::dlpack;

namespace
{

    TEST(DLpackTypes, Convertion)
    {
        {
            device_type_t device_type = kDLCPU;

            DLDeviceType dl_device_type = kDLCPU;

            EXPECT_EQ(dl_device_type, device_type);
        }
    }

    TEST(DLpackTypes, Format)
    {

        {
            device_type_t device_type = kDLCPU;

            EXPECT_EQ(fmt::to_string(device_type), "Cpu");
        }
        {
            data_type_code_t data_type_code = kDLFloat;

            EXPECT_EQ(fmt::to_string(data_type_code), "float");
        }
        {
            data_type_t data_type = {
                .code = kDLFloat,
                .bits = 32,
                .lanes = 1
            };

            EXPECT_EQ(fmt::to_string(data_type), "dtype('float32')");
        }
        {
            data_type_ext_t data_type_ext = {
                .code = kDLFloat,
                .bits = 32,
                .lanes = 1
            };

            EXPECT_EQ(fmt::to_string(data_type_ext), "dtype('float32')");
        }
        {
            data_type_t data_type = {
                .code = kDLFloat,
                .bits = 32,
                .lanes = 2
            };

            EXPECT_EQ(fmt::to_string(data_type), "dtype('float32[2]')");
        }
        {
            data_type_ext_t data_type_ext = {
                .code = kDLFloat,
                .bits = 32,
                .lanes = 2
            };

            EXPECT_EQ(fmt::to_string(data_type_ext), "dtype('float32[2]')");
        }




    }

} // namespace
