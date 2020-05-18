#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <cuda_runtime_api.h>


__global__ void ker()
{
}

namespace
{
    TEST(ArrayTest, EmptyBArray)
    {
        ker<<<1,1>>>();
        cudaDeviceSynchronize();
        EXPECT_TRUE(1);
    }
}

