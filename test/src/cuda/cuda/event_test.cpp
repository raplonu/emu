#include <gtest/gtest.h>

#include <emu/cuda/event.hpp>

using namespace emu::cuda;

namespace
{

    TEST(CudaEvent, EventHandleCreation)
    {
        auto device = current_device();

        event_handle event(device);

        EXPECT_TRUE(event.get());
        EXPECT_TRUE(event.is_done());

        auto status = cudaEventQuery(event.get());
        EXPECT_EQ(status, cudaSuccess);

    }

    TEST(CudaEvent, EventHandleFromEventRef)
    {
        auto device = current_device();

        const event event_owner(device);

        {
            const event_handle event(event_owner, /* take_ownership = */ false);

            EXPECT_TRUE(event.get());
            EXPECT_TRUE(event.is_done());

            auto status = cudaEventQuery(event_owner.get());
            EXPECT_EQ(status, cudaSuccess);
        }

        auto status = cudaEventQuery(event_owner.get());
        EXPECT_EQ(status, cudaSuccess);
    }

} // namespace
