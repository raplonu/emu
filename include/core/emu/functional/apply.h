#ifndef EMU_FUNCTIONAL_APPLY_H
#define EMU_FUNCTIONAL_APPLY_H

#include <emu/macro.h>
#include <emu/utility.h>
#include <emu/functional/invoke.h>
#include <emu/tuple.h>
namespace emu
{

namespace functional
{
namespace detail
{

        template <class F, class Tuple, std::size_t... I>
        EMU_HODE constexpr decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
        {
            using std::get;
            return invoke(EMU_FWD(f), get<I>(EMU_FWD(t))...);
        }

}  // namespace detail

    // Behave exactly as the std::apply of C++17 with device support.
    // Implementation: https://en.cppreference.com/w/cpp/utility/apply.
    // TODO : Delete when C++17 and cuda std provide it.
    template <class F, class Tuple>
    EMU_HODE constexpr
    decltype(auto) apply(F&& f, Tuple&& t)
        EMU_NOEXCEPT_EXPR(
            detail::apply_impl(EMU_FWD(f), EMU_FWD(t), std::make_index_sequence<tuple::Size<Tuple>::value>{})
        )
    {
        return detail::apply_impl(
            EMU_FWD(f), EMU_FWD(t),
            std::make_index_sequence<tuple::Size<Tuple>::value>{});
    }

    /**
     * Function object that apply a function with a given tuple.
     *
     * Procedure<B (Ts...)> F, Tuple<Ts...> T
     */
    template<typename F> // , typename... Ts>
    struct apply_t {
        F fn;

        template<typename Tuple>
        constexpr EMU_HODE
        auto operator()(Tuple && t)
            EMU_NOEXCEPT_EXPR(apply(fn, t))
        {
            return apply(fn, EMU_FWD(t));
        }


        template<typename Tuple>
        constexpr EMU_HODE
        auto operator()(Tuple && t) const
            EMU_NOEXCEPT_EXPR(apply(fn, t))
        {
            return apply(fn, EMU_FWD(t));
        }
    };

    /**
     * Helper function to generate apply_t function object.
     */
    template<typename F>
    constexpr EMU_HODE
    auto make_apply(F && fn) noexcept {
        return apply_t<F>{ EMU_FWD(fn) };
    }
} // namespace functional


} // namespace emu

#endif //EMU_FUNCTIONAL_APPLY_H