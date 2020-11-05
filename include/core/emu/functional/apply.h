#ifndef EMU_FUNCTIONAL_APPLY_H
#define EMU_FUNCTIONAL_APPLY_H

namespace emu
{

namespace functional
{
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
            return apply(fn, fwd<Tuple>(t));
        }


        template<typename Tuple>
        constexpr EMU_HODE
        auto operator()(Tuple && t) const
            EMU_NOEXCEPT_EXPR(apply(fn, t))
        {
            return apply(fn, fwd<Tuple>(t));
        }
    };

    /**
     * Helper function to generate apply_t function object.
     */
    template<typename F>
    constexpr EMU_HODE
    auto make_apply(F && fn) noexcept {
        return apply_t<F>{ fwd<F>(fn) };
    }
} // namespace functional


} // namespace emu

#endif //EMU_FUNCTIONAL_APPLY_H