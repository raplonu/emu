#pragma once

#include <emu/type_name.hpp>
#include <emu/concepts.hpp>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

namespace detail
{

    template<typename T>
    struct unkown_type_printer {};

} // namespace detail

    template<typename T>
    constexpr decltype(auto) try_print(const T& t) {
        if constexpr (cpts::formattable<T>)
            return t;
        else
            return detail::unkown_type_printer<T>{};
    }

    template<typename T, typename D>
    constexpr decltype(auto) try_print(const T& t, const D& default_value) {
        if constexpr (cpts::formattable<T>)
            return t;
        else
            return default_value;
    }

namespace spe
{
    template <typename T>
    struct default_info_t
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            return fmt::format_to(it, "{}", type_name<T>);
        }

        constexpr auto format_value(const T &t, fmt::format_context::iterator it) const {
            if constexpr (emu::cpts::formattable<T>) {
                return fmt::format_to(it, "{} @ {}", t, fmt::ptr(std::addressof(t)));
            }
        }
    };

    template <typename T>
    struct info_t : default_info_t<T>
    {};

} // namespace spe

namespace detail
{

    template <typename T>
    struct info_holder_t
    {
        const T &t;
        spe::info_t<T> info;

        template <typename... Args>
        info_holder_t(const T &t, Args&&... args)
            : t(t)
            , info{std::forward<Args>(args)...}
        {}

        constexpr auto format_type(fmt::format_context::iterator it) const {
            return info.format_type(it);
        }

        constexpr auto format_value(fmt::format_context::iterator it) const {
            return info.format_value(t, it);
        }
    };

} // namespace detail

    template <typename T, typename... Args>
    auto info(const T &t, Args&&... args) {
        return detail::info_holder_t<T>{t, std::forward<Args>(args)...};
    }

} // namespace emu

template <typename T>
struct fmt::formatter<emu::detail::unkown_type_printer<T>> {

    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator {
        return ctx.begin();
    }

    constexpr auto format(const emu::detail::unkown_type_printer<T> &p, format_context &ctx) const -> format_context::iterator {
        return fmt::format_to(ctx.out(), "unkown type {}", emu::type_name<T>);
    }
};


template <typename T>
struct fmt::formatter<emu::detail::info_holder_t<T>> {

    constexpr auto parse(format_parse_context &ctx) -> format_parse_context::iterator {
        return ctx.begin();
    }

    constexpr auto format(const emu::detail::info_holder_t<T> &p, format_context &ctx) const -> format_context::iterator {
        auto it = p.format_type(ctx.out());
        it = fmt::format_to(it, "\t");
        return p.format_value(it);
    }
};
