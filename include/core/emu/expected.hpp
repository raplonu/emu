#pragma once

#include <emu/assert.hpp>
#include <emu/concepts.hpp>
#include <emu/functor.hpp>

#include <tl/expected.hpp>

#include <fmt/format.h>

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


    namespace spe
    {
        template<typename T, typename E>
        struct map< expected<T, E> > {

            template<typename Fr, typename Fn>
            constexpr auto operator()(Fr&& opt, Fn&& fn) const {
                return EMU_FWD(opt).map(EMU_FWD(fn));
            }
        };

        template<typename T, typename E>
        struct unwrap_error<expected<T, E>> {
            constexpr unexpected<E> operator()(const expected<T, E>& e) const {
                return unexpected(e.error());
            }
        };


    } // namespace spe



    template<typename Expected>
        requires cpts::expected<decay<Expected>>
    decltype(auto) fwd_error(Expected&& exp) {
        return unexpected(EMU_FWD(exp).error());
    }

    /**
     * @brief Return the first error from expected values. They must share the same error type.
     *
     * @tparam Ts
     * @tparam E
     * @param exp
     * @return E the first error found if any, otherwise the default value of E.
     */
    template<typename... Ts, typename E>
    E get_first_error(const tl::expected<Ts, E>&... exps) {
        E error;
        // If `not exps` test pass (aka has error), then assign error and stop the evaluation
        // using the `or` short-circuit operator.
        ((not exps and (error = exps.error(), true)) or ...);
        return error;
    }


    // template<typename Fn, typename... Ts, typename E>
    // auto invoke_when_all(Fn&& fn, tl::expected<Ts, E>&&... expecteds) -> decltype(fn(expecteds.value()...)) {
    //     if ((expecteds && ...))
    //         return fn(expecteds.value()...);
    //     else
    //         return get_first_error(expecteds...);
    // }

} // namespace emu

template<typename T, typename E, typename Char>
struct fmt::formatter<tl::expected<T, E>, Char> : fmt::formatter<T, Char>
{
    using base = fmt::formatter<T, Char>;

    template<typename FormatContext>
    auto format(const tl::expected<T, E>& exp, FormatContext& ctx) {
        if (exp) {
            // First print the prefix and update the context to the end of it.
            ctx.advance_to(fmt::format_to(ctx.out(), "expected("));
            // Format the hold value using its own formatter.
            base::format(exp.value(), ctx);
            // Finally print the suffix.
            return fmt::format_to(ctx.out(), ")");
        } else
            //Note: The error has no `parse` method. It cannot be personalized.
            return fmt::format_to(ctx.out(), "unexpected({})", exp.error());
    }
};
