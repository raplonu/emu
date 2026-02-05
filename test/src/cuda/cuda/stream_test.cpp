#include <gtest/gtest.h>

#include <emu/cuda/stream.hpp>

using namespace emu::cuda;

namespace
{

    TEST(CudaStream, StreamHandleCreation)
    {
        auto device = current_device();

        stream_handle stream(device);

        EXPECT_TRUE(stream.get());
        EXPECT_TRUE(stream.is_done());

        auto status = cudaStreamQuery(stream.get());
        EXPECT_EQ(status, cudaSuccess);

    }

    TEST(CudaStream, StreamHandleFromStreamRef)
    {
        auto device = current_device();

        const stream stream_owner(device);

        {
            const stream_handle stream(stream_owner, /* take_ownership = */ false);

            EXPECT_TRUE(stream.get());
            EXPECT_TRUE(stream.is_done());

            auto status = cudaStreamQuery(stream_owner.get());
            EXPECT_EQ(status, cudaSuccess);
        }

        auto status = cudaStreamQuery(stream_owner.get());
        EXPECT_EQ(status, cudaSuccess);
    }

} // namespace
