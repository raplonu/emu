#pragma once

#include <emu/macro.cuh>
#include <emu/utility.hpp>

#include <thrust/tuple.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/zip_iterator.h>

#include <functional>

namespace emu::iterator
{

namespace detail
{

    template<typename F>
    struct apply {
        F fn;

        template<typename... Ts>
        constexpr EMU_HODE
        auto operator()(Ts&&... ts) const noexcept {
            return std::apply(fn, EMU_FWD(ts)...);
        }
    };

} // namespace detail

    /**
     * Helper function that return an iterator that iterate on multiple given iterators
     * and return the result of the given function with the source of the iterators.
     *
     * Procedure<B (Ts...)> F, Iterator<Ts>... Its
     */
    template<typename... Its, typename F>
    constexpr EMU_HODE
    auto make_transform_iterator(F fn, Its... its) noexcept {
        return ::thrust::make_transform_iterator(
            ::thrust::make_zip_iterator(::thrust::make_tuple(its...)),
            detail::apply<F>(fn)
        );
    }

} // namespace emu::iterator
