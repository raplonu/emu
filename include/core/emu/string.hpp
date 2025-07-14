/**
 * @file
 * @brief Defines string manipulation utilities.
 *
 * This file provides functions for string-to-number conversion and for splitting
 * strings into ranges of substrings based on a delimiter.
 */
#pragma once

#include <emu/error.hpp>

#include <charconv>
#include <string_view>
#include <ranges>

namespace emu
{

    /**
     * @brief Converts a sequence of characters to a numeric value.
     * @tparam T The type of the value to convert to.
     * @param sv The string view containing the characters to convert.
     * @param value The variable to store the converted value in.
     * @param base The number base to use for the conversion (e.g., 10 for decimal).
     * @return A result containing a string_view of the remaining characters on
     *         success, or an error if the conversion fails.
     */
    template<typename T>
    auto from_chars(std::string_view sv, T& value, int base = 10) -> result<std::string_view> {
        const auto* last = sv.data() + sv.size();
        auto [p, ec] = std::from_chars(sv.data(), last, value, base);
        EMU_TRUE_OR_RETURN_UN_EC(ec == std::errc(), ec);
        return std::string_view{p, last};
    }

    /**
     * @brief Splits a string into a range of substrings based on a delimiter.
     * @param words The string_view to split.
     * @param delim The delimiter to split the string by.
     * @return A range of string_views representing the substrings. Empty
     *         substrings will be included in the result.
     */
    constexpr inline auto split_string(std::string_view words, std::string_view delim = " ") {
        return std::views::split(words, delim) | std::views::transform([](auto sub_str) {
                // sub_str is not categorized as contiguous range but it is.
                // We have to use the deref hack to construct the string_view.
                return std::string_view(&*sub_str.begin(), std::ranges::distance(sub_str));
            });
    }

    /**
     * @brief Splits a string into a range of substrings based on a delimiter,
     *        excluding any empty substrings.
     * @param words The string_view to split.
     * @param delim The delimiter to split the string by.
     * @return A range of string_views representing the non-empty substrings.
     */
    constexpr inline auto split_string_no_empty(std::string_view words, std::string_view delim = " ") {
        return split_string(words, delim) | std::views::filter([](auto sv) { return not sv.empty(); });
    }

} // namespace emu
