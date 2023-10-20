// #include <gtest/gtest.h>

// #include <cuda_runtime_api.h>
// #include <fmt/core.h>

// #include <thrust/device_vector.h>
// #include <thrust/functional.h>

// #include <emu/iterator/function.cuh>

// template<typename T> void f()
// {
//     fmt::print("{}\n", __PRETTY_FUNCTION__);
// }

// namespace
// {

//     TEST(BinaryTransform, Simple)
//     {
//         using emu::iterator::make_transform_iterator;
//         std::array<int, 3> arr1{1, 2, 3}, arr2{2, 3, 4};

//         auto it = make_transform_iterator(thrust::plus<int>{}, arr1.begin(), arr2.begin());

//         ASSERT_EQ(it[0], 3);
//         ASSERT_EQ(it[1], 5);
//         ASSERT_EQ(it[2], 7);
//     }

//     TEST(BinaryTransform, SimpleCopy)
//     {
//         using emu::iterator::make_transform_iterator;
//         std::vector<int> arr1{1, 2, 3}, arr2{2, 3, 4}, res(3);
//         // thrust::device_vector<int> d_arr1(arr1), d_arr2(arr2);

//         auto it = make_transform_iterator(thrust::plus<int>{}, arr1.begin(), arr2.begin());

//         thrust::copy(it, it + 3, res.begin());

//         ASSERT_EQ(res[0], 3);
//         ASSERT_EQ(res[1], 5);
//         ASSERT_EQ(res[2], 7);
//     }

//     TEST(BinaryTransform, Cuda)
//     {
//         using emu::iterator::make_transform_iterator;
//         thrust::device_vector<int> arr1(3), arr2(3);
//         arr1[0] = 1; arr1[1] = 2; arr1[2] = 3;
//         arr2[0] = 2; arr2[1] = 3; arr2[2] = 4;

//         auto it = make_transform_iterator(thrust::plus<int>{}, arr1.begin(), arr2.begin());

//         ASSERT_EQ(it[0], 3);
//         ASSERT_EQ(it[1], 5);
//         ASSERT_EQ(it[2], 7);
//     }

//     TEST(BinaryTransform, CudaCopy)
//     {
//         using emu::iterator::make_transform_iterator;
//         thrust::device_vector<int> arr1(3), arr2(3), res(3);
//         arr1[0] = 1; arr1[1] = 2; arr1[2] = 3;
//         arr2[0] = 2; arr2[1] = 3; arr2[2] = 4;

//         auto it = make_transform_iterator(thrust::plus<int>{}, arr1.begin(), arr2.begin());

//         thrust::copy(it, it + 3, res.begin());

//         ASSERT_EQ(res[0], 3);
//         ASSERT_EQ(res[1], 5);
//         ASSERT_EQ(res[2], 7);
//     }
// }

