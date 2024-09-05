#pragma once

#include <emu/utility.hpp>

#include <algorithm>
#include <utility>
#include <tuple>
#include <vector>
#include <array>
#include <variant>

namespace emu
{

namespace spe
{

    template<typename From>
    struct map {
        template<typename Fr, typename Fn>
        constexpr auto operator()(Fr&& from, Fn&& fn) const {
            return std::invoke(EMU_FWD(fn), EMU_FWD(from));
        }
    };

} // namespace spe

namespace detail {

    // just used to move the first template type of spe::as From to the method.
    struct map {
        template<typename From, typename Fn> // <- From is declared here.
        constexpr auto operator()(From && from, Fn&& fn) const {
            return spe::map<std::remove_cvref_t<From>>{}(EMU_FWD(from), EMU_FWD(fn));
        }
    };

} // namespace detail

    constexpr auto map = detail::map{};

namespace detail {

    template<typename To>
    struct as {
        template<typename From>
        constexpr auto operator()(From&& from) const {
            return emu::map(
                EMU_FWD(from),
                []<typename T>(T&& t) { return static_cast<To>(EMU_FWD(t)); }
            );
        }
    };

} // namespace detail

    template<typename T>
    EMU_HODE_CONSTEXPR detail::as<T> as;

namespace spe
{

    template<typename T>
    struct unwrap_error {
        constexpr bool operator()(const T&) const {
            return false;
        }
    };

} // namespace spe

namespace detail
{

    struct unwrap_error {
        template<typename T>
        constexpr auto operator()(const T& t) const {
            return spe::unwrap_error<T>{}(t);
        }
    };

} // namespace detail

    EMU_HODE_CONSTEXPR detail::unwrap_error unwrap_error;

} // namespace emu

// Specializations for std::vector
template<typename T, typename Allocator>
struct emu::spe::map< std::vector<T, Allocator> > {

    using source_vector = std::vector<T, Allocator>;
    using allocator_traits = std::allocator_traits<Allocator>;

    template<typename Fr, typename Fn>
    constexpr auto operator()(Fr&& vec, Fn&& fn) const {
        using destination_type = decltype(emu::map(vec[0], fn));
        using destination_allocator = typename allocator_traits::template rebind_alloc<destination_type>;

        destination_allocator alloc = allocator_traits::select_on_container_copy_construction(vec.get_allocator());
        std::vector<destination_type, destination_allocator> res(alloc); res.reserve(vec.size());

        std::ranges::transform(EMU_FWD(vec), std::back_inserter(res), fn);

        return res;
    }
};

template<typename T, std::size_t N>
struct emu::spe::map< std::array<T, N>> {

    template<typename Fr, typename Fn, std::size_t... I>
    constexpr auto apply(Fr&& vec, Fn&& fn, std::index_sequence<I...>) const {
        return std::array{ std::invoke(fn, vec[I])... };
    }

    template<typename Fr, typename Fn>
    constexpr auto operator()(Fr&& vec, Fn&& fn) const {
        return apply(EMU_FWD(vec), EMU_FWD(fn), std::make_index_sequence<N>{});
    }
};

template<typename... Ts>
struct emu::spe::map<std::tuple<Ts...>> {

    template<typename Tuple, typename Fn, std::size_t... I>
    constexpr auto apply(Tuple&& tuple, Fn&& fn, std::index_sequence<I...>) const {
        return std::tuple{ std::invoke(fn, std::get<I>(tuple))... };
    }

    template<typename Tuple, typename Fn>
    constexpr auto operator()(Tuple&& tuple, Fn&& fn) const {
        return apply(EMU_FWD(tuple), EMU_FWD(fn), std::make_index_sequence<sizeof...(Ts)>{});
    }
};

template<typename T1, typename T2>
struct emu::spe::map<std::pair<T1, T2>> {

    template<typename Pair, typename Fn>
    constexpr auto operator()(Pair&& pair, Fn&& fn) const {
        return std::pair{ std::invoke(fn, pair.first), std::invoke(fn, pair.second) };
    }
};

template<typename... Ts>
struct emu::spe::map<std::variant<Ts...>> {

    template<typename Variant, typename Fn>
    constexpr auto operator()(Variant&& var, Fn&& fn) const {

        using new_variant = std::variant<decltype(std::invoke(fn, std::declval<Ts>()))...>;

        return std::visit([&fn](auto&& v) { return new_variant{ std::invoke(EMU_FWD(fn), EMU_FWD(v)) }; }, EMU_FWD(var));
    }
};
