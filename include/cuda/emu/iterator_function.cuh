#ifndef EMU_UTILS_CUH
#define EMU_UTILS_CUH

#include <emu/macro.cuh>
#include <emu/utility.h>
#include <emu/type_traits.h>


#include <tuple>

#include <thrust/tuple.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/zip_iterator.h>

namespace emu
{
    /**
     * Function object that apply a function with a given tuple.
     * 
     * Procedure<B (Ts...)> F, Tuple<Ts...> T
     */    
    template<typename F, typename... Ts>
    struct apply_t {
        F fn;

        using result_type = decltype(apply(fn, std::declval<thrust::tuple<Ts...>>()));

        template<typename Tuple>
        constexpr EMU_HODE result_type
        operator()(Tuple && t)
            EMU_NOEXCEPT_EXPR(apply(fn, t))
        {
            return apply(fn, fwd<Tuple>(t));
        }


        template<typename Tuple>
        constexpr EMU_HODE result_type
        operator()(Tuple && t) const
            EMU_NOEXCEPT_EXPR(apply(fn, t))
        {
            return apply(fn, fwd<Tuple>(t));
        }
    };

    /**
     * Helper function to generate apply_t function object.
     */
    template<typename... Ts, typename F>
    EMU_HODE  auto make_apply(F && fn) {
        return apply_t<F, Ts...>{ fwd<F>(fn) };
    }

    /**
     * Helper function that return an iterator that iterate on multiple given iterators
     * and return the result of the given function with the source of the iterators.
     * 
     * Procedure<B (Ts...)> F, Iterator<Ts>... Its
     */
    template<typename... Its, typename F>
    EMU_HODE auto make_transform_iterator(F fn, Its... its) {
        return thrust::make_transform_iterator(
            thrust::make_zip_iterator(thrust::make_tuple(its...)),
            make_apply< IteratorValue<Its>... >(fwd<F>(fn))
        );
    }
}

#endif //EMU_UTILS_CUH