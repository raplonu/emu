#pragma once
#include <gtest/gtest.h>

#include <emu/concepts.hpp>
#include <emu/type_traits.hpp>

#include <ranges>
#include <span>
#include <memory>
#include <algorithm>
#include <numeric>


namespace emu_test
{

    struct shared_vector
    {
        std::shared_ptr<int[]> data;
        std::size_t size;

        shared_vector(std::initializer_list<int> il)
            : data(new int[il.size()]), size(il.size())
        {
            std::copy(il.begin(), il.end(), data.get());
        }

        shared_vector(const shared_vector& other) = default;
        shared_vector(shared_vector&& other) = default;

        shared_vector& operator=(const shared_vector& other) = default;
        shared_vector& operator=(shared_vector&& other) = default;

        ~shared_vector() = default;

        [[nodiscard]] int* begin() const { return data.get(); }
        [[nodiscard]] int* end() const { return data.get() + size; }
    };


} // namespace emu_test

template <>
constexpr bool emu::spe::enable_relocatable_owning_range<emu_test::shared_vector> = true;

namespace emu_test
{

    struct shared_vector_view : shared_vector, std::ranges::view_base
    {
        using shared_vector::shared_vector;
    };

    template<emu::cpts::view View>
    auto data_of(const View& view)
    {
        if constexpr (std::ranges::contiguous_range<View>)
            return view.data();
        else
            return view.data_handle();
    }

    template < emu::cpts::span Span >
    void expect_rank(const Span &s, std::size_t expected_rank) {
        EXPECT_EQ(1, expected_rank);
    }

    template < emu::cpts::mdspan MDSpan >
    void expect_rank(const MDSpan &md, std::size_t expected_rank) {
        EXPECT_EQ(md.rank(), expected_rank);
    }

    template < emu::cpts::span Span >
    void expect_address(const Span &s, typename Span::const_pointer expected_address) {
        EXPECT_EQ(s.data(), expected_address);
    }

    template < emu::cpts::mdspan MDSpan >
    void expect_address(const MDSpan &md, typename MDSpan::element_type const * expected_address) {
        EXPECT_EQ(md.data_handle(), expected_address);
    }

    template < emu::cpts::span Span >
    void expect_extents(const Span &s, std::span<const size_t> expected_extents) {
        expect_rank(s, expected_extents.size());
        EXPECT_EQ(s.size(), expected_extents[0]);
    }

    template < emu::cpts::mdspan MD>
    void expect_extents(const MD &md, std::span<const size_t> expected_extents) {
        expect_rank(md, expected_extents.size());
        for (std::size_t i = 0; i < expected_extents.size(); i++) {
            EXPECT_EQ(md.extent(i), expected_extents[i]);
        }
    }

    template < emu::cpts::view V, typename Ptr >
    void check_view(const V& v, const Ptr* expected_address, std::span<const size_t> expected_extents) {
        expect_address(v, expected_address);
        expect_extents(v, expected_extents);
    }

    template < emu::cpts::capsule_owner CO >
    void expect_use_count(const CO &co, std::size_t expected_use_count) {
        EXPECT_EQ(co.use_count(), expected_use_count);
    }

    template < emu::cpts::capsule_owner CO, typename Ptr>
    void check_container(const CO& co, const Ptr* expected_address, std::span<const size_t> expected_extents, std::size_t expected_use_count) {
        check_view(co, expected_address, expected_extents);

        expect_use_count(co, expected_use_count);
    }

namespace md_helper
{
    constexpr size_t buffer_size = 2 * 3 * 4;

    template <typename T>
    std::vector<T> get_vector() {
        std::vector<T> v(buffer_size);
        std::iota(v.begin(), v.end(), 0);
        return v;
    }

    template <typename T>
    std::array<T, buffer_size> get_array() {
        std::array<T, buffer_size> arr;
        std::iota(arr.begin(), arr.end(), 0);
        return arr;
    }

    template<typename T>
    void fill_c_array(T* arr) {
        for (size_t i = 0; i < buffer_size; i++) {
            arr[i] = i;
        }
    }

    template<size_t Rank>
    std::span<const size_t, Rank> get_extents() {
        if constexpr (Rank == 1) {
            static constexpr std::array<size_t, 1> extents{buffer_size};
            return extents;
        } else if constexpr (Rank == 2) {
            static constexpr std::array<size_t, 2> extents{2, 3 * 4};
            return extents;
        } else if constexpr (Rank == 3) {
            static constexpr std::array<size_t, 3> extents{2, 3, 4};
            return extents;
        } else {
            static_assert(emu::dependent_false_v<Rank>, "Rank not supported");
        }
    }

} // namespace md_helper

} // namespace emu_test
