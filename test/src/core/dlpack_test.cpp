#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <emu/dlpack.hpp>

#include <test_utility/spy.hpp>

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

    TEST(DLPack, ConstSpan)
    {
        const std::array data{1, 2, 3, 4, 5, 6, 7, 8, 9};
        const std::span span{data};

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(span);

            auto tensor = st.tensor();

            EXPECT_TRUE(st.read_only());

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 9);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<int*>(tensor.data), data.data());
        }
        {
            auto span_result = emu::dlpack::import_from_scoped_tensor<std::span<const int>>(st);

            EXPECT_TRUE(span_result);

            const std::span<const int> span2 = span_result.value();

            EXPECT_EQ(span.data(), span2.data());
            EXPECT_EQ(span.size(), span2.size());
        }
        {
            auto span_result = emu::dlpack::import_from_scoped_tensor<std::span<int>>(st);

            EXPECT_FALSE(span_result);

            EXPECT_EQ(span_result.error(), emu::make_error_code(emu::errc::dlpack_read_only));
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

    TEST(DLPack, CopiedContainer)
    {
        emu::container data{1, 2, 3, 4, 5, 6, 7, 8, 9};

        const std::span s = data;
        const emu::capsule cap = data.capsule();

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(std::move(data));

            auto tensor = st.tensor();

            EXPECT_TRUE(st.is_copied());

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLInt);
            EXPECT_EQ(tensor.shape[0], 9);
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<int*>(tensor.data), data.data());
        }
        {
            auto data_result = emu::dlpack::import_from_scoped_tensor<emu::container<int>>(st);

            EXPECT_TRUE(data_result);

            emu::container<int> data2 = std::move(data_result).value(); // steal the capusle.

            EXPECT_EQ(data2.capsule().use_count(), 3) << "Capsule is only own by cap, st and data2";

            EXPECT_EQ(s.data(), data2.data());
            EXPECT_EQ(s.size(), data2.size());
            EXPECT_EQ(cap, data2.capsule());
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

    TEST(DLPack, StringView)
    {
        std::string_view sv = "hi mom!";

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(sv);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLOpaqueHandle);
            EXPECT_EQ(tensor.shape[0], sv.size());
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<char*>(tensor.data), sv.data());
        }
        {
            auto sv_result = emu::dlpack::import_from_scoped_tensor<std::string_view>(st);

            EXPECT_TRUE(sv_result);

            const std::string_view sv2 = sv_result.value();

            EXPECT_EQ(sv.data(), sv2.data());
            EXPECT_EQ(sv.size(), sv2.size());
        }
    }

    TEST(DLPack, String)
    {
        std::string s = "hi mom!";

        emu::dlpack::scoped_tensor st;

        {
            st = emu::dlpack::export_to_scoped_tensor(s);

            auto tensor = st.tensor();

            EXPECT_EQ(tensor.ndim, 1);
            EXPECT_EQ(tensor.dtype.code, emu::dlpack::data_type_code_t::kDLOpaqueHandle);
            EXPECT_EQ(tensor.shape[0], s.size());
            EXPECT_EQ(tensor.strides, nullptr);
            EXPECT_EQ(static_cast<char*>(tensor.data), s.data());
        }
        {
            auto sv_result = emu::dlpack::import_from_scoped_tensor<std::string_view>(st);

            EXPECT_TRUE(sv_result);

            const std::string_view sv2 = std::move(sv_result).value();

            EXPECT_EQ(s.data(), sv2.data());
            EXPECT_EQ(s.size(), sv2.size());
        }
        {
            auto s_result = emu::dlpack::import_from_scoped_tensor<std::string>(st);

            EXPECT_TRUE(s_result);

            const std::string s2 = std::move(s_result).value();

            EXPECT_EQ(s, s2);
            EXPECT_EQ(s.size(), s2.size());
        }
    }


    TEST(DLPack, VectorLifeTime)
    {
        using namespace emu_test;

        spy_flag flag;

        emu::dlpack::scoped_tensor st;

        {
            // Create the vector here.
            // only emplace_back allow to not copy the Spy object.
            std::vector<Spy> v; v.emplace_back(&flag);

            // Move the vector into the scoped_tensor.
            st = emu::dlpack::export_to_scoped_tensor(std::move(v));

            EXPECT_TRUE(st.is_copied());

            EXPECT_FALSE(flag.copied);
            EXPECT_FALSE(flag.moved);
            EXPECT_FALSE(flag.destroyed);
        }

        // vector is now out of scope but data is still living in scoped_tensor.

        // Nothing has been moved nor copied.
        EXPECT_FALSE(flag.copied);
        EXPECT_FALSE(flag.moved);
        EXPECT_FALSE(flag.destroyed);

        {
            // If we move the tensor into the import function it will move each element
            // into a new vector.
            // Element are still moved. vector does not allow to borrow a range.
            auto v_result = emu::dlpack::import_from_scoped_tensor<std::vector<Spy>>(std::move(st));

            EXPECT_FALSE(flag.copied);
            EXPECT_TRUE(flag.moved) << "At this point, elements have only been moved";
            EXPECT_FALSE(flag.destroyed);

            EXPECT_TRUE(v_result);

            // Unwrap the vector from the result.
            auto v2 = EMU_UNWRAP_RES_OR_THROW(std::move(v_result));

            EXPECT_EQ(v2.size(), 1);
        }

        // vector 2 is now out of scope. The buffer is destroyed.

        EXPECT_FALSE(flag.copied);
        EXPECT_TRUE(flag.moved);
        EXPECT_TRUE(flag.destroyed);

    }

} // namespace
