#include <gtest/gtest.h>
#include <emu/cuda/managed/container.hpp>
#include <emu/cuda.hpp>

namespace
{
    TEST(ManagedContainer, AllocateSimple)
    {
        const auto con = emu::cuda::managed::make_container<int>(emu::cuda::device_t{0}, 5);

        EXPECT_NE(con.data(), nullptr);
        EXPECT_EQ(con.size(), 5);
    }

} // namespace
