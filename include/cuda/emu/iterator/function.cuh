#ifndef EMU_ITERATOR_FUNCTION_H
#define EMU_ITERATOR_FUNCTION_H

#include <emu/macro.cuh>
#include <emu/utility.h>
#include <emu/functional/apply.h>

#include <thrust/tuple.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/zip_iterator.h>

namespace emu
{

namespace iterator
{

    /**
     * Helper function that return an iterator that iterate on multiple given iterators
     * and return the result of the given function with the source of the iterators.
     *
     * Procedure<B (Ts...)> F, Iterator<Ts>... Its
     */
    template<typename... Its, typename F>
    constexpr EMU_HODE
    auto make_transform_iterator(F fn, Its... its) noexcept {
        return thrust::make_transform_iterator(
            thrust::make_zip_iterator(thrust::make_tuple(its...)),
            functional::make_apply(fwd<F>(fn))
        );
    }

} // namespace iterator


} // namespace emu

#endif //EMU_ITERATOR_FUNCTION_H