#pragma once

#include <tl/expected.hpp>

#include <fmt/core.h>

namespace emu
{
    /**
     * @brief The vocabulary type which contains an expected value of type T, or an error E.
     *
     * An optional object is an object that contains the storage for another object and managesthe
     * lifetime of this contained object, if any. The contained object may be initialized after
     * the optional object has been initialized, and may be destroyed before the optional object
     * has been destroyed. The initialization state of the contained object is tracked by the optional object.
     *
     *
     * Please refer to https://tl.tartanllama.xyz/en/latest/api/expected.html#tl-optional for more detail.
     *
     * @note Do not replace this type by std::optional. tl::optional
     * provide a non standard functional interface that is not standard yet.
     */
    using tl::expected;

    /**
     * @brief Create an unexpected from e, deducing the return type.
     **/
    using tl::make_unexpected;

    /**
     * @brief A tag to tell tl::expected to construct the unexpected value.
     **/
    using tl::unexpect;

    /**
     * @brief A tag to tell optional to construct its value in-place.
     **/
    using tl::in_place;

    /**
     * @brief Thrown when checked value accesses fail.
     **/
    using tl::bad_expected_access;

    /**
     * @brief Internal type used as a wrapper to store the unexpected value.
     **/
    using tl::unexpected;

}

template<typename T, typename E>
struct fmt::formatter<tl::expected<T, E>>
{
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tl::expected<T, E>& exp, FormatContext& ctx) {
        if (exp)
            return fmt::format_to(ctx.out(), "expected({})", exp.value());
        else
            return fmt::format_to(ctx.out(), "unexpected({})", exp.error());
    }
};
