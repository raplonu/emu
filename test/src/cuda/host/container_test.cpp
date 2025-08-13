#include <gtest/gtest.h>
#include <emu/cuda/host/container.hpp>
#include <emu/cuda.hpp>

namespace
{
    TEST(Container, AllocateSimple)
    {
        const auto con = emu::cuda::host::make_container<int>(5);
        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }

} // namespace
