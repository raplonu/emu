#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <emu/dlpack.hpp>

namespace
{
    TEST(DLPack, Scalar)
    {
        const float f = 3.14;
        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(f);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 0);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLFloat);
            EXPECT_EQ(tensor.shape, nullptr);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<float*>(tensor.data), &f);

        }

        {
            auto f2_result = emu::dlpack::import_from_scoped_tensor<float>(st);

            EXPECT_TRUE(f2_result);

            const float& f2 = f2_result.value();

            EXPECT_EQ(&f, &f2);
        }

    }

    TEST(DLPack, Span)
    {
        std::array data{1, 2, 3, 4, 5, 6, 7, 8, 9};
        const std::span span{data};

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(span);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 9);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<int*>(tensor.data), data.data());
        }
        {
            auto span_result = emu::dlpack::import_from_scoped_tensor<std::span<int>>(st);

            EXPECT_TRUE(span_result);

            const std::span<int> span2 = span_result.value();

            EXPECT_EQ(span.data(), span2.data());
            EXPECT_EQ(span.size(), span2.size());
        }
    }

    TEST(DLPack, Container)
    {
        emu::container data{1, 2, 3, 4, 5, 6, 7, 8, 9};

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(data);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 9);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<int*>(tensor.data), data.data());
        }
        {
            auto data_result = emu::dlpack::import_from_scoped_tensor<emu::container<int>>(st);

            EXPECT_TRUE(data_result);

            emu::container<int> data2 = data_result.value();

            EXPECT_EQ(data.data(), data2.data());
            EXPECT_EQ(data.size(), data2.size());
            EXPECT_EQ(data.capsule(), data2.capsule());
        }
    }

    TEST(DLPack, MDSpan2D)
    {
        std::array data{1, 2, 3, 4, 5, 6, 7, 8, 9};
        const emu::mdspan mdspan{data.data(), 3, 3};

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(mdspan);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 2);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 3);
            EXPECT_EQ(tensor.shape[1], 3);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<int*>(tensor.data), mdspan.data_handle());
        }
        {
            auto data_result = emu::dlpack::import_from_scoped_tensor<emu::mdspan_2d<int>>(st);

            EXPECT_TRUE(data_result);

            const emu::mdspan mdspan2 = data_result.value();

            EXPECT_EQ(mdspan.data_handle(), mdspan2.data_handle());
            EXPECT_EQ(mdspan.size(), mdspan2.size());
            EXPECT_EQ(mdspan.mapping(), mdspan2.mapping());
        }
    }

    TEST(DLPack, MDSpan2DStrided)
    {
        std::array data{1, 2, 3, 4, 5, 6, 7, 8, 9};
        // Drop every second element in the second dimension
        const emu::mdspan_2d_s<int> mdspan{data.data(), {emu::d2{3, 2}, std::array{3, 2}}};

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(mdspan);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 2);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 3);
            EXPECT_EQ(tensor.shape[1], 2);
            ASSERT_THAT(tensor.strides, ::testing::NotNull());
            EXPECT_EQ(tensor.strides[0], 3);
            EXPECT_EQ(tensor.strides[1], 2);
            EXPECT_EQ(static_cast<int*>(tensor.data), mdspan.data_handle());
        }
        {
            auto data_result = emu::dlpack::import_from_scoped_tensor<emu::mdspan_2d_s<int>>(st);

            EXPECT_TRUE(data_result);

            const emu::mdspan mdspan2 = data_result.value();

            EXPECT_EQ(mdspan.data_handle(), mdspan2.data_handle());
            EXPECT_EQ(mdspan.size(), mdspan2.size());
            EXPECT_EQ(mdspan.mapping(), mdspan2.mapping());
        }
    }

} // namespace
