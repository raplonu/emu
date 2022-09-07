#include <gtest/gtest.h>

#include <emu/mdspan.h>

#include <array>
#include <vector>

using namespace emu;

namespace stdex = std::experimental;
_MDSPAN_INLINE_VARIABLE constexpr auto dyn = stdex::dynamic_extent;

TEST(TestMdspanCtorDataCArray, test_mdspan_ctor_data_carray) {
  int data[1] = {42};
  emu::mdspan_t<int, stdex::extents<std::size_t, 1>> m(data);
  ASSERT_EQ(m.data(), data);
  ASSERT_EQ(m.rank(), 1);
  ASSERT_EQ(m.rank_dynamic(), 0);
  ASSERT_EQ(m.extent(0), 1);
  ASSERT_EQ(m.stride(0), 1);
  ASSERT_EQ(__MDSPAN_OP(m, 0), 42);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanCtorDataStdArray, test_mdspan_ctor_data_carray) {
  std::array<int, 1> d = {42};
  emu::mdspan_t<int, stdex::extents<std::size_t, 1>> m(d.data());
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 1);
  ASSERT_EQ(m.rank_dynamic(), 0);
  ASSERT_EQ(m.extent(0), 1);
  ASSERT_EQ(m.stride(0), 1);
  ASSERT_EQ(__MDSPAN_OP(m, 0), 42);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanCtorDataVector, test_mdspan_ctor_data_carray) {
  std::vector<int> d = {42};
  emu::mdspan_t<int, stdex::extents<std::size_t, 1>> m(d.data());
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 1);
  ASSERT_EQ(m.rank_dynamic(), 0);
  ASSERT_EQ(m.extent(0), 1);
  ASSERT_EQ(m.stride(0), 1);
  ASSERT_EQ(__MDSPAN_OP(m, 0), 42);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanCtorExtentsStdArrayConvertibleToSizeT, test_mdspan_ctor_extents_std_array_convertible_to_size_t) {
  std::array<int, 4> d{42, 17, 71, 24};
  std::array<int, 2> e{2, 2};
  emu::mdspan_t<int, stdex::dextents<std::size_t, 2>> m(d.data(), e);
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 2);
  ASSERT_EQ(m.rank_dynamic(), 2);
  ASSERT_EQ(m.extent(0), 2);
  ASSERT_EQ(m.extent(1), 2);
  ASSERT_EQ(m.stride(0), 2);
  ASSERT_EQ(m.stride(1), 1);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanListInitializationLayoutLeft, test_mdspan_list_initialization_layout_left) {
  std::array<int, 1> d{42};
  emu::mdspan_t<int, stdex::extents<std::size_t, dyn, dyn>, stdex::layout_left> m{d.data(), 16, 32};
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 2);
  ASSERT_EQ(m.rank_dynamic(), 2);
  ASSERT_EQ(m.extent(0), 16);
  ASSERT_EQ(m.extent(1), 32);
  ASSERT_EQ(m.stride(0), 1);
  ASSERT_EQ(m.stride(1), 16);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanListInitializationLayoutRight, test_mdspan_list_initialization_layout_right) {
  std::array<int, 1> d{42};
  emu::mdspan_t<int, stdex::extents<std::size_t, dyn, dyn>, stdex::layout_right> m{d.data(), 16, 32};
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 2);
  ASSERT_EQ(m.rank_dynamic(), 2);
  ASSERT_EQ(m.extent(0), 16);
  ASSERT_EQ(m.extent(1), 32);
  ASSERT_EQ(m.stride(0), 32);
  ASSERT_EQ(m.stride(1), 1);
  ASSERT_TRUE(m.is_contiguous());
}

TEST(TestMdspanListInitializationLayoutStride, test_mdspan_list_initialization_layout_stride) {
  std::array<int, 1> d{42};
  emu::mdspan_t<int, stdex::extents<std::size_t, dyn, dyn>, stdex::layout_stride> m{d.data(), {stdex::dextents<std::size_t, 2>{16, 32}, std::array<std::size_t, 2>{1, 128}}};
  ASSERT_EQ(m.data(), d.data());
  ASSERT_EQ(m.rank(), 2);
  ASSERT_EQ(m.rank_dynamic(), 2);
  ASSERT_EQ(m.extent(0), 16);
  ASSERT_EQ(m.extent(1), 32);
  ASSERT_EQ(m.stride(0), 1);
  ASSERT_EQ(m.stride(1), 128);
  ASSERT_FALSE(m.is_contiguous());
}

// #if defined(_MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION)
// TEST(TestMdspanCTADExtentsPack, test_mdspan_ctad_extents_pack) {
//   std::array<int, 1> d{42};
//   emu::mdspan_t m(d.data(), 64, 128);
//   ASSERT_EQ(m.data(), d.data());
//   ASSERT_EQ(m.rank(), 2);
//   ASSERT_EQ(m.rank_dynamic(), 2);
//   ASSERT_EQ(m.extent(0), 64);
//   ASSERT_EQ(m.extent(1), 128);
//   ASSERT_TRUE(m.is_contiguous());
// }

// TEST(TestMdspanCTADExtentsObject, test_mdspan_ctad_extents_object) {
//   std::array<int, 1> d{42};
//   emu::mdspan_t m{d.data(), stdex::extents{std::size_t, 64, 128}};
//   ASSERT_EQ(m.data(), d.data());
//   ASSERT_EQ(m.rank(), 2);
//   ASSERT_EQ(m.rank_dynamic(), 2);
//   ASSERT_EQ(m.extent(0), 64);
//   ASSERT_EQ(m.extent(1), 128);
//   ASSERT_TRUE(m.is_contiguous());
// }

// TEST(TestMdspanCTADExtentsStdArray, test_mdspan_ctad_extents_std_array) {
//   std::array<int, 1> d{42};
//   emu::mdspan_t m{d.data(), std::array{64, 128}};
//   ASSERT_EQ(m.data(), d.data());
//   ASSERT_EQ(m.rank(), 2);
//   ASSERT_EQ(m.rank_dynamic(), 2);
//   ASSERT_EQ(m.extent(0), 64);
//   ASSERT_EQ(m.extent(1), 128);
//   ASSERT_TRUE(m.is_contiguous());
// }

// TEST(TestMdspanCTADLayoutLeft, test_mdspan_ctad_layout_left) {
//   std::array<int, 1> d{42};

//   emu::mdspan_t m0{d.data(), stdex::layout_left::mapping{stdex::extents{std::size_t, 16, 32}}};
//   ASSERT_EQ(m0.data(), d.data());
//   ASSERT_EQ(m0.rank(), 2);
//   ASSERT_EQ(m0.rank_dynamic(), 2);
//   ASSERT_EQ(m0.extent(0), 16);
//   ASSERT_EQ(m0.extent(1), 32);
//   ASSERT_EQ(m0.stride(0), 1);
//   ASSERT_EQ(m0.stride(1), 16);
//   ASSERT_TRUE(m0.is_contiguous());

// // TODO: Perhaps one day I'll get this to work.
// /*
//   emu::mdspan_t m1{d.data(), stdex::layout_left::mapping{{16, 32}}};
//   ASSERT_EQ(m1.data(), d.data());
//   ASSERT_EQ(m1.rank(), 2);
//   ASSERT_EQ(m1.rank_dynamic(), 2);
//   ASSERT_EQ(m1.extent(0), 16);
//   ASSERT_EQ(m1.extent(1), 32);
//   ASSERT_EQ(m1.stride(0), 1);
//   ASSERT_EQ(m1.stride(1), 16);
//   ASSERT_TRUE(m1.is_contiguous());
// */
// }

// TEST(TestMdspanCTADLayoutRight, test_mdspan_ctad_layout_right) {
//   std::array<int, 1> d{42};

//   emu::mdspan_t m0{d.data(), stdex::layout_right::mapping{stdex::extents{std::size_t, 16, 32}}};
//   ASSERT_EQ(m0.data(), d.data());
//   ASSERT_EQ(m0.rank(), 2);
//   ASSERT_EQ(m0.rank_dynamic(), 2);
//   ASSERT_EQ(m0.extent(0), 16);
//   ASSERT_EQ(m0.extent(1), 32);
//   ASSERT_EQ(m0.stride(0), 32);
//   ASSERT_EQ(m0.stride(1), 1);
//   ASSERT_TRUE(m0.is_contiguous());

// // TODO: Perhaps one day I'll get this to work.
// /*
//   emu::mdspan_t m1{d.data(), stdex::layout_right::mapping{{16, 32}}};
//   ASSERT_EQ(m1.data(), d.data());
//   ASSERT_EQ(m1.rank(), 2);
//   ASSERT_EQ(m1.rank_dynamic(), 2);
//   ASSERT_EQ(m1.extent(0), 16);
//   ASSERT_EQ(m1.extent(1), 32);
//   ASSERT_EQ(m1.stride(0), 32);
//   ASSERT_EQ(m1.stride(1), 1);
//   ASSERT_TRUE(m1.is_contiguous());
// */
// }

// TEST(TestMdspanCTADLayoutStride, test_mdspan_ctad_layout_stride) {
//   std::array<int, 1> d{42};

//   emu::mdspan_t m0{d.data(), stdex::layout_stride::mapping{stdex::extents{std::size_t, 16, 32}, std::array{1, 128}}};
//   ASSERT_EQ(m0.data(), d.data());
//   ASSERT_EQ(m0.rank(), 2);
//   ASSERT_EQ(m0.rank_dynamic(), 2);
//   ASSERT_EQ(m0.extent(0), 16);
//   ASSERT_EQ(m0.extent(1), 32);
//   ASSERT_EQ(m0.stride(0), 1);
//   ASSERT_EQ(m0.stride(1), 128);
//   ASSERT_FALSE(m0.is_contiguous());

//   /*
//   emu::mdspan_t m1{d.data(), stdex::layout_stride::mapping{stdex::extents{std::size_t, 16, 32}, stdex::extents{std::size_t, 1, 128}}};
//   ASSERT_EQ(m1.data(), d.data());
//   ASSERT_EQ(m1.rank(), 2);
//   ASSERT_EQ(m1.rank_dynamic(), 2);
//   ASSERT_EQ(m1.extent(0), 16);
//   ASSERT_EQ(m1.extent(1), 32);
//   ASSERT_EQ(m1.stride(0), 1);
//   ASSERT_EQ(m1.stride(1), 128);
//   ASSERT_FALSE(m1.is_contiguous());
//   */

// // TODO: Perhaps one day I'll get this to work.
// /*
//   emu::mdspan_t m2{d.data(), stdex::layout_stride::mapping{{16, 32}, {1, 128}}};
//   ASSERT_EQ(m2.data(), d.data());
//   ASSERT_EQ(m2.rank(), 2);
//   ASSERT_EQ(m2.rank_dynamic(), 2);
//   ASSERT_EQ(m2.extent(0), 16);
//   ASSERT_EQ(m2.extent(1), 32);
//   ASSERT_EQ(m2.stride(0), 1);
//   ASSERT_EQ(m2.stride(1), 128);
//   ASSERT_FALSE(m2.is_contiguous());
// */
// }
// #endif
