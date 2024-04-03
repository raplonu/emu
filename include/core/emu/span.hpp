#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/type_name.hpp>

#include <span>
#include <fmt/core.h>
#include <fmt/format.h>

#include <boost/core/demangle.hpp>

namespace emu
{

    using std::span;

namespace detail
{

    template <typename T, auto Extent>
    constexpr auto type_extent = Extent == std::dynamic_extent
                                    ? std::dynamic_extent
                                    : Extent / sizeof(T);

} // namespace detail

    using std::as_bytes;
    using std::as_writable_bytes;

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(span<std::byte, Extent> sp) noexcept {
        auto data = reinterpret_cast<NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        constexpr auto extent = detail::type_extent<NewType, Extent>;

        return span<NewType, extent>{data, size};
    }

    template <typename NewType, std::size_t Extent>
    constexpr auto as_t(span<const std::byte, Extent> sp) noexcept {
        auto data = reinterpret_cast<const NewType *>(sp.data());
        auto size = sp.size_bytes() / sizeof(NewType);
        constexpr auto extent = detail::type_extent<NewType, Extent>;

        return span<const NewType, extent>{data, size};
    }

    template <typename T>
    auto as_auto_bytes(span<T> sp) {
        if constexpr (std::is_const_v<T>)
            return as_bytes(sp);
        else
            return as_writable_bytes(sp);
    }

namespace detail
{

    inline auto format_extent(fmt::format_context::iterator it, std::size_t extent) {
        return (extent == std::dynamic_extent)
            ? fmt::format_to(it, "dyn")
            : fmt::format_to(it, "{}", extent);
    }

    template<cpts::span Span>
    auto c_contigous(const Span&) {
        return true;
    }

    template<cpts::span Span>
    auto f_contigous(const Span&) {
        return true;
    }

} // namespace detail

namespace spe
{
    template <typename T, std::size_t Extent>
    struct info_t< span<T, Extent> >
    {
        constexpr auto format_type(fmt::format_context::iterator it) const {
            it = fmt::format_to(it, "span<{}, ", type_name<T>);
            it = detail::format_extent(it, Extent);
            return fmt::format_to(it, ">");
        }

        constexpr auto format_value(const span<T, Extent> &t, fmt::format_context::iterator it) const {
            return fmt::format_to(it, "@{}[{}]", fmt::ptr(t.data()), t.size());
        }
    };

} // namespace spe

} // namespace emu
