#pragma once

#include <cstddef>
#include <emu/cstring_view.hpp>

#include <array>
#include <string_view>

#warning "This header is experimental and may be removed in the future. Do not rely on it."

namespace emu
{


    template <size_t N>
    struct const_string {
        constexpr const_string(const char (&_str)[N])
            : value_(std::to_array(_str))
        {}

        constexpr const_string(const std::array<char, N>& arr)
            : value_(arr)
        {}

        constexpr operator std::string_view() const noexcept {
            return std::string_view(value_.data(), N - 1);
        }

        constexpr operator cstring_view() const noexcept {
            return cstring_view(null_terminated, value_.data(), N);
        }

        const std::array<char, N> value_;
    };

    // operator+
    template <size_t N, size_t M>
    constexpr auto operator+(const const_string<N>& lhs, const const_string<M>& rhs) {
        std::array<char, N + M - 1> arr{};
        for (size_t i = 0; i < N - 1; ++i) {
            arr[i] = lhs.value_[i];
        }
        for (size_t i = 0; i < M; ++i) {
            arr[N - 1 + i] = rhs.value_[i];
        }
        return const_string<N + M - 1>(arr);
    }

    template <size_t N, size_t M>
    constexpr auto operator+(const const_string<N>& lhs, const char (&rhs)[M]) {
        return lhs + const_string<M>(rhs);
    }

    template <size_t N, size_t M>
    constexpr auto operator+(const char (&lhs)[N], const const_string<M>& rhs) {
        return const_string<N>(lhs) + rhs;
    }


} // namespace emu
