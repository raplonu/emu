#pragma once

// If has include gtest/gtest.h
#if not __has_include (<gtest/gtest.h>)
#error "Missing <gtest/gtest.h>, this header is meant to be use with Google Test for unit testing only."
#endif

#include <gtest/gtest.h>

#include <emu/cuda.hpp>

namespace emu::test
{

    struct CudaTestSuite : public ::testing::Test {
        void SetUp() override {
            if (not are_cuda_device_available()) {
                GTEST_SKIP() << "Skipping test because no CUDA devices are available.";
            }
        }

        static bool are_cuda_device_available() {
            return ::cuda::devices().size() > 0;
        }
    };

} // namespace emu::test

#define EMU_CUDA_TEST(test_suite_name, test_name)                             \
    GTEST_TEST_(test_suite_name, test_name,                                   \
                ::emu::test::CudaTestSuite,                                   \
                ::testing::internal::GetTypeId<::emu::test::CudaTestSuite>())

#if !(defined(EMU_DONT_DEFINE_TEST) && EMU_DONT_DEFINE_TEST)
#define CUDA_TEST(test_suite_name, test_name) EMU_CUDA_TEST(test_suite_name, test_name)
#endif
