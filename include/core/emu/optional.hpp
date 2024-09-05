#pragma once

#include <emu/macro.hpp>
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

    /**
     * @brief Returns the opt content if exists or will emplace it from the additional arguments provided.
     *
     * @param opt - An optional value (e.g. std::optional, tl::optional).
     * @param args - variadic list of argument that will be forwarded to the constructor.
     */
    template<typename Opt, typename... Args>
        requires( cpts::optional< decay<Opt> > )
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
    template<typename Opt, typename... Args>
        requires( cpts::optional< decay<Opt> > )
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
    template<typename Opt, typename Fn, typename... Args>
        requires( cpts::optional< decay<Opt> > )
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
    template<typename Opt, typename Fn, typename... Args>
        requires( cpts::optional< decay<Opt> > )
    constexpr auto value_or_invoke_emplace(Opt && opt, Fn && fn, Args &&... args)
        EMU_NOEXCEPT_EXPR( (opt) ? EMU_FWD(opt).value() : EMU_FWD(fn)(EMU_FWD(args)...) )
    {
        return (opt) ? EMU_FWD(opt).value() : EMU_FWD(opt).emplace(EMU_FWD(fn)(EMU_FWD(args)...));
    }

    template<typename Fn, typename... Opts>
        requires( (cpts::opt_like< decay<Opts> > && ...) )
    auto invoke_when_all(Fn&& fn, Opts&&... opts) -> tl::optional<decltype(fn(EMU_FWD(opts).value()...))> {
        if ((opts && ...))
            return fn(EMU_FWD(opts).value()...);
        else
            return nullopt;
    }

namespace spe
{
    template<typename T>
    struct map< std::optional<T> > {

        template<typename Fr, typename Fn>
        constexpr auto operator()(Fr&& opt, Fn&& fn)
            -> std::optional<decltype(std::invoke(EMU_FWD(fn), EMU_FWD(opt).value()))> const
        {
            if (opt)
                return std::invoke(EMU_FWD(fn), EMU_FWD(opt).value());
            else
                return std::nullopt;
        }
    };

    template<typename T>
    struct map< tl::optional<T> > {

        template<typename Fr, typename Fn>
        constexpr auto operator()(Fr&& opt, Fn&& fn) const {
            return EMU_FWD(opt).map(EMU_FWD(fn));
        }
    };

    template<typename T>
    struct unwrap_error<std::optional<T>> {
        constexpr std::nullopt_t operator()(const std::optional<T>&) const {
            return std::nullopt;
        }
    };


    template<typename T>
    struct unwrap_error<tl::optional<T>> {
        constexpr tl::nullopt_t operator()(const tl::optional<T>&) const {
            return tl::nullopt;
        }
    };

} // namespace spe


    template<typename T>
    tl::optional<T> as_opt(T* ptr) noexcept {
        if (ptr)
            return *ptr;
        else
            return tl::nullopt;
    }

    template<typename T>
    tl::optional<std::reference_wrapper<T>> as_opt_ref(T* ptr) noexcept {
        if (ptr)
            return std::ref(*ptr);
        else
            return tl::nullopt;
    }

} // namespace emu

template<typename T, typename Char>
struct fmt::formatter<tl::optional<T>, Char> : fmt::formatter<T, Char>
{
    using base = fmt::formatter<T>;

    template<typename FormatContext>
    auto format(const tl::optional<T>& opt, FormatContext& ctx) {
        if (opt) {
            // First print the prefix and update the context to the end of it.
            ctx.advance_to(format_to(ctx.out(), "optional("));
            // Format the hold value using its own formatter.
            base::format(opt.value(), ctx);
            // Finally print the suffix.
            return format_to(ctx.out(), ")");
        } else
            return format_to(ctx.out(), "none");
    }
};
