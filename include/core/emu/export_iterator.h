#ifndef EMU_EXPORT_ITERATOR_H
#define EMU_EXPORT_ITERATOR_H

#include <emu/macro.h>

#include <thrust/iterator/iterator_adaptor.h>

namespace emu
{

    template<typename IteratorIn, typename  IteratorOut>
    struct export_iterator : thrust::iterator_adaptor<
        export_iterator<IteratorIn, IteratorOut>,
        IteratorIn>
    {
        using super_t = thrust::iterator_adaptor<
            export_iterator<IteratorIn, IteratorOut>,
            IteratorIn
        >;
        using super_t::base_reference;

        // IteratorIn is handled by thrust::iterator_adaptor.
        // It is not the case of IteratorOut.
        IteratorOut out;

        EMU_HODE
        export_iterator(IteratorIn in, IteratorOut out) :
            super_t(in), out(out)
        {}

        // befriend thrust::iterator_core_access to allow it access to the private interface below
        friend class thrust::iterator_core_access;

        EMU_HODE
        typename super_t::reference dereference()
        {
            *out = *m_iterator;
            return *m_iterator;
        }

        EMU_HODE
        constexpr void advance(typename super_t::difference_type n)
            EMU_NOEXCEPT_EXPR(base_reference() += n, out += n)
        {
            base_reference() += n;
            out += n;
        }
    };

    template<typename IteratorIn, typename IteratorOut>
    constexpr export_iterator<IteratorIn, IteratorOut> make_export_iterator(IteratorIn in, IteratorOut out)
        EMU_NOEXCEPT_EXPR(export_iterator<IteratorIn, IteratorOut>{in, out})
    {
        return export_iterator<IteratorIn, IteratorOut>{in, out};
    }
}

#endif //EMU_EXPORT_ITERATOR_H