#include <gtest/gtest.h>

#include <emu/tensor_traits.hpp>

#include <span>
#include <vector>
#include <array>

using namespace emu;

namespace
{

    // These tests are not relevent anymore since capsule accept type with value semantics like shared_ptr.
    TEST(TensorTraits, ConceptTest)
    {
        static_assert(not cpts::tensor<int>);
        static_assert(not cpts::tensor<int*>);
        static_assert(not cpts::tensor<int&>);

        static_assert(cpts::tensor<      std::span<      int>  >);
        static_assert(cpts::tensor<      std::span<const int>  >);
        static_assert(cpts::tensor<const std::span<      int>  >);
        static_assert(cpts::tensor<const std::span<const int>  >);
        static_assert(cpts::tensor<const std::span<      int>& >);
        static_assert(cpts::tensor<const std::span<const int>& >);
        static_assert(cpts::tensor<      std::span<      int>&&>);
        static_assert(cpts::tensor<      std::span<const int>&&>);

        static_assert(cpts::tensor<      emu::mdspan_2d<      int>  >);
        static_assert(cpts::tensor<      emu::mdspan_2d<const int>  >);
        static_assert(cpts::tensor<const emu::mdspan_2d<      int>  >);
        static_assert(cpts::tensor<const emu::mdspan_2d<const int>  >);
        static_assert(cpts::tensor<const emu::mdspan_2d<      int>& >);
        static_assert(cpts::tensor<const emu::mdspan_2d<const int>& >);
        static_assert(cpts::tensor<      emu::mdspan_2d<      int>&&>);
        static_assert(cpts::tensor<      emu::mdspan_2d<const int>&&>);

        static_assert(cpts::tensor<      std::vector<int>  >);
        static_assert(cpts::tensor<const std::vector<int>  >);
        static_assert(cpts::tensor<const std::vector<int>& >);
        static_assert(cpts::tensor<      std::vector<int>&&>);

        static_assert(cpts::tensor<      std::array<int, 3>  >);
        static_assert(cpts::tensor<const std::array<int, 3>  >);
        static_assert(cpts::tensor<const std::array<int, 3>& >);
        static_assert(cpts::tensor<      std::array<int, 3>&&>);

    }

    TEST(TensorTraits, RankTest)
    {
        static_assert(emu::tensor_traits<std::span<int>>::rank == 1);
        static_assert(emu::tensor_traits<std::span<const int>>::rank == 1);
        static_assert(emu::tensor_traits<const std::span<int>>::rank == 1);
        static_assert(emu::tensor_traits<const std::span<const int>>::rank == 1);
        static_assert(emu::tensor_traits<const std::span<int>&>::rank == 1);
        static_assert(emu::tensor_traits<const std::span<const int>&>::rank == 1);
        static_assert(emu::tensor_traits<std::span<int>&&>::rank == 1);
        static_assert(emu::tensor_traits<std::span<const int>&&>::rank == 1);

        static_assert(emu::tensor_traits<emu::mdspan_2d<int>>::rank == 2);
        static_assert(emu::tensor_traits<emu::mdspan_2d<const int>>::rank == 2);
        static_assert(emu::tensor_traits<const emu::mdspan_2d<int>>::rank == 2);
        static_assert(emu::tensor_traits<const emu::mdspan_2d<const int>>::rank == 2);
        static_assert(emu::tensor_traits<const emu::mdspan_2d<int>&>::rank == 2);
        static_assert(emu::tensor_traits<const emu::mdspan_2d<const int>&>::rank == 2);
        static_assert(emu::tensor_traits<emu::mdspan_2d<int>&&>::rank == 2);
        static_assert(emu::tensor_traits<emu::mdspan_2d<const int>&&>::rank == 2);

        static_assert(emu::tensor_traits<std::vector<int>>::rank == 1);
        static_assert(emu::tensor_traits<const std::vector<int>>::rank == 1);
        static_assert(emu::tensor_traits<const std::vector<int>&>::rank == 1);
        static_assert(emu::tensor_traits<std::vector<int>&&>::rank == 1);

        static_assert(emu::tensor_traits<std::array<int, 3>>::rank == 1);
        static_assert(emu::tensor_traits<const std::array<int, 3>>::rank == 1);
        static_assert(emu::tensor_traits<const std::array<int, 3>&>::rank == 1);
        static_assert(emu::tensor_traits<std::array<int, 3>&&>::rank == 1);

    }

    TEST(TensorTraits, DataHandleTypeTest)
    {
        static_assert(std::same_as<typename emu::tensor_traits<std::span<int>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<std::span<const int>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::span<int>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::span<const int>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::span<int>&>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::span<const int>&>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<std::span<int>&&>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<std::span<const int>&&>::data_handle_type, const int*>);

        static_assert(std::same_as<typename emu::tensor_traits<emu::mdspan_2d<int>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<emu::mdspan_2d<const int>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const emu::mdspan_2d<int>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const emu::mdspan_2d<const int>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const emu::mdspan_2d<int>&>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const emu::mdspan_2d<const int>&>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<emu::mdspan_2d<int>&&>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<emu::mdspan_2d<const int>&&>::data_handle_type, const int*>);

        static_assert(std::same_as<typename emu::tensor_traits<std::vector<int>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::vector<int>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::vector<int>&>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<std::vector<int>&&>::data_handle_type, int*>);

        static_assert(std::same_as<typename emu::tensor_traits<std::array<int, 3>>::data_handle_type, int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::array<int, 3>>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<const std::array<int, 3>&>::data_handle_type, const int*>);
        static_assert(std::same_as<typename emu::tensor_traits<std::array<int, 3>&&>::data_handle_type, int*>);

    }


} // namespace
