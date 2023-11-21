#pragma once

#include <emu/macro.h>
#include <emu/concepts.hpp>
#include <emu/utility.hpp>
#include <emu/functor.hpp>

#include <tl/optional.hpp>

#include <fmt/format.h>
#include <fmt/std.h>

#include <optional>

namespace emu
{
    //TODO: replace by std::optional when C++23 is available.
    using tl::optional;

    using tl::monostate;

    using tl::nullopt_t;
    using tl::nullopt;

    using tl::in_place_t;
    using tl::in_place;

namespace cpts
{

    template<typename Opt>
    concept optional = equivalent<Opt, tl::optional<typename std::decay_t<Opt>::value_type>>
                    or equivalent<Opt, std::optional<typename std::decay_t<Opt>::value_type>>;

} // namespace cpts

    /**
     * @brief Returns the opt content if exists or will emplace it from the additional arguments provided.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param args - variadic list of argument that will be forwarded to the constructor.
     */
    template<cpts::optional Opt, typename... Args>
    constexpr auto value_or_emplace(Opt && opt, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(args)...) )
    {
        return (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(args)...);
    }

    /**
     * @brief Returns the opt content if exists or will create it from the additional arguments provided.
     *
     * This function is an alternative to Opt::value_or (e.g. std::optional::value_or, tl::optional::value_or)
     * when the user doesn't want to create a default_value if not used.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param args - variadic list of argument that will be forwarded to the constructor.
     */
    template<cpts::optional Opt, typename... Args>
    constexpr auto value_or_create(Opt && opt, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : ValueType<Opt>(EMU_FWD(args)...) )
    {
        return (opt) ? EMU_FWD(opt).value() : ValueType<Opt>(EMU_FWD(args)...);
    }

    /**
     * @brief Returns the opt content if exists or will get it from the invocation of the provided callable.
     *
     * This function is an alternative to Opt::value_or (e.g. std::optional::value_or, tl::optional::value_or)
     * when the user doesn't want to invoke the function if not used.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param fn - A callable value that will return (e.g. function, function object).
     * @param args - variadic list of argument that will be forwarded to the function.
     */
    template<cpts::optional Opt, typename Fn, typename... Args>
    constexpr auto value_or_invoke(Opt && opt, Fn && fn, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...) )
    {
        return (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...);
    }

    /**
     * @brief Returns the opt content if exists or will emplace it from the invocation of the provided callable.
     *
     * This function is an alternative to value_or_emplace
     * when the user doesn't want to invoke the function if not used.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param fn - A callable value that will return (e.g. function, function object).
     * @param args - variadic list of argument that will be forwarded to the function.
     */
    template<cpts::optional Opt, typename Fn, typename... Args>
    constexpr auto value_or_invoke_emplace(Opt && opt, Fn && fn, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...) )
    {
        return (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(fn)(EMU_FWD(args)...));
    }

    template<typename T>
    struct spe::map< std::optional<T> > {

        template<typename Fr, typename Fn>
        constexpr auto operator()(Fr&& opt, Fn&& fn) -> std::optional<decltype(std::invoke(EMU_FWD(fn), EMU_FWD(opt).value()))> const {
            if (opt)
                return std::invoke(EMU_FWD(fn), EMU_FWD(opt).value());
            else
                return std::nullopt;
        }
    };

    template<typename T>
    struct spe::map< tl::optional<T> > {

        template<typename Fr, typename Fn>
        constexpr auto operator()(Fr&& opt, Fn&& fn) const {
            return EMU_FWD(opt).map(EMU_FWD(fn));
        }
    };

} // namespace emu

template<typename T>
struct fmt::formatter<tl::optional<T>>
{
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const tl::optional<T>& opt, FormatContext& ctx) {
        if (opt)
            return format_to(ctx.out(), "optional({})", opt.value());
        else
            return format_to(ctx.out(), "none");
    }
};