#pragma once

#include <emu/pybind11.hpp>

#include <pybind11/pybind11.h>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <string_view>

namespace fmt
{

template <emu::pybind11::cpts::derived_from_handle T, typename Char>
struct formatter<T, Char> : fmt::formatter<const Char *, Char> {
    using base = fmt::formatter<const Char *, Char>;

    std::string_view format_spec = "{}";

    constexpr auto parse(format_parse_context& ctx) {
        auto it = ctx.begin(), end = ctx.end();

        // If spec is empty, return directly.
        if (it == end or *it == '}') return it;

        format_spec = std::string_view(it - 2, (end - it + 2));

        // Advance iterator to the end.
        for(;it != end and *it != '}'; it++);

        return it;
    }

    template <typename FormatContext>
    auto format(const T& obj, FormatContext& ctx) {
        return base::format(nanobind::str(format_spec.data(), format_spec.size()).format(obj).c_str(), ctx);
    }
};

    // Specialization for nanobind python based type to never be treated as a range by fmt.
    template <emu::nanobind::cpts::derived_from_handle T, typename Char>
    struct is_range<T, Char>: std::false_type{};

} // namespace fmt
