#include <gtest/gtest.h>

#include <emu/blas.hpp>
#include <emu/blas/cblas.hpp>

#include <emu/mdcontainer.hpp>

#include <vector>
#include <algorithm>

using namespace emu::linealg;

namespace
{

    // These tests are not relevent anymore since capsule accept type with value semantics like shared_ptr.
    TEST(CBlas, Simple)
    {
        cblas::context ctx;

        std::vector x = {1.f, 2.f, 3.f, 4.f};
        std::vector y = {0.f, 0.f, 0.f, 0.f};

        auto A = emu::make_mdcontainer<float, emu::layout_left>(4, 4);
        std::fill_n(A.data_handle(), A.size(), 0.f);
        A(0, 0) = 1;
        A(1, 1) = 2;
        A(2, 2) = 3;
        A(3, 3) = 4;

        blas::gemv(ctx,
                   transpose_mode::none,
                   1.0f,
                   A,
                   x,
                   0.0f,
                   y);

        EXPECT_EQ(y[0], 1);
        EXPECT_EQ(y[1], 4);
        EXPECT_EQ(y[2], 9);
        EXPECT_EQ(y[3], 16);

    }

} // namespace
