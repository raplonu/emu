// #include <gtest/gtest.h>

// #include <emu/iterator/coordinate.cuh>

// #include <cuda_runtime_api.h>

// constexpr EMU_HODE
// std::size_t operator "" _sz ( unsigned long long n ) noexcept { return n; }

// namespace
// {
//     TEST(XYPos, SquareCase)
//     {
//         /**
//          * Frame sub aperture order
//          *
//          *  +-----------+
//          *  | 0 0 | 1 1 |
//          *  | 0 0 | 1 1 |
//          *  +-----------+
//          *  | 2 2 | 3 3 |
//          *  | 2 2 | 3 3 |
//          *  +-----------+
//         **/

//         const std::size_t X[] = {0,2,0,2};
//         const std::size_t Y[] = {0,0,2,2};

//         auto xy_pos = emu::iterator::make_xy_pos_iterator(X, Y, 2, 2);

//         ASSERT_EQ(xy_pos[ 0], thrust::make_tuple(0_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 1], thrust::make_tuple(1_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 2], thrust::make_tuple(0_sz, 1_sz));
//         ASSERT_EQ(xy_pos[ 3], thrust::make_tuple(1_sz, 1_sz));

//         ASSERT_EQ(xy_pos[ 4], thrust::make_tuple(2_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 5], thrust::make_tuple(3_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 6], thrust::make_tuple(2_sz, 1_sz));
//         ASSERT_EQ(xy_pos[ 7], thrust::make_tuple(3_sz, 1_sz));

//         ASSERT_EQ(xy_pos[ 8], thrust::make_tuple(0_sz, 2_sz));
//         ASSERT_EQ(xy_pos[ 9], thrust::make_tuple(1_sz, 2_sz));
//         ASSERT_EQ(xy_pos[10], thrust::make_tuple(0_sz, 3_sz));
//         ASSERT_EQ(xy_pos[11], thrust::make_tuple(1_sz, 3_sz));

//         ASSERT_EQ(xy_pos[12], thrust::make_tuple(2_sz, 2_sz));
//         ASSERT_EQ(xy_pos[13], thrust::make_tuple(3_sz, 2_sz));
//         ASSERT_EQ(xy_pos[14], thrust::make_tuple(2_sz, 3_sz));
//         ASSERT_EQ(xy_pos[15], thrust::make_tuple(3_sz, 3_sz));
//     }

//     TEST(XYPos, RectangleCase)
//     {
//         /**
//          * Frame sub aperture order
//          *
//          *  +---------------+
//          *  | 0 0 0 | 1 1 1 |
//          *  | 0 0 0 | 1 1 1 |
//          *  +---------------+
//          *  | 2 2 2 | 3 3 3 |
//          *  | 2 2 2 | 3 3 3 |
//          *  +---------------+
//         **/

//         const std::size_t X[] = {0,3,0,3};
//         const std::size_t Y[] = {0,0,2,2};

//         auto xy_pos = emu::iterator::make_xy_pos_iterator(X, Y, 3, 2);

//         ASSERT_EQ(xy_pos[ 0], thrust::make_tuple(0_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 1], thrust::make_tuple(1_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 2], thrust::make_tuple(2_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 3], thrust::make_tuple(0_sz, 1_sz));
//         ASSERT_EQ(xy_pos[ 4], thrust::make_tuple(1_sz, 1_sz));
//         ASSERT_EQ(xy_pos[ 5], thrust::make_tuple(2_sz, 1_sz));

//         ASSERT_EQ(xy_pos[ 6], thrust::make_tuple(3_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 7], thrust::make_tuple(4_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 8], thrust::make_tuple(5_sz, 0_sz));
//         ASSERT_EQ(xy_pos[ 9], thrust::make_tuple(3_sz, 1_sz));
//         ASSERT_EQ(xy_pos[10], thrust::make_tuple(4_sz, 1_sz));
//         ASSERT_EQ(xy_pos[11], thrust::make_tuple(5_sz, 1_sz));

//         ASSERT_EQ(xy_pos[12], thrust::make_tuple(0_sz, 2_sz));
//         ASSERT_EQ(xy_pos[13], thrust::make_tuple(1_sz, 2_sz));
//         ASSERT_EQ(xy_pos[14], thrust::make_tuple(2_sz, 2_sz));
//         ASSERT_EQ(xy_pos[15], thrust::make_tuple(0_sz, 3_sz));
//         ASSERT_EQ(xy_pos[16], thrust::make_tuple(1_sz, 3_sz));
//         ASSERT_EQ(xy_pos[17], thrust::make_tuple(2_sz, 3_sz));

//         ASSERT_EQ(xy_pos[18], thrust::make_tuple(3_sz, 2_sz));
//         ASSERT_EQ(xy_pos[19], thrust::make_tuple(4_sz, 2_sz));
//         ASSERT_EQ(xy_pos[20], thrust::make_tuple(5_sz, 2_sz));
//         ASSERT_EQ(xy_pos[21], thrust::make_tuple(3_sz, 3_sz));
//         ASSERT_EQ(xy_pos[22], thrust::make_tuple(4_sz, 3_sz));
//         ASSERT_EQ(xy_pos[23], thrust::make_tuple(5_sz, 3_sz));
//     }
// }

