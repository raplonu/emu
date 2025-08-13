#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_span.hpp>
#include <emu/cuda/managed/mdspan.hpp>
#include <emu/mdalgo.hpp>


namespace emu
{
namespace cuda::managed
{

    template <typename ElementType, size_t Extent>
    struct span : emu::detail::basic_span<ElementType, Extent, cuda::managed_location_policy, span<ElementType, Extent> >
    {
        using base = emu::detail::basic_span<ElementType, Extent, cuda::managed_location_policy, span >;

        using base::base;

        template<typename OT, size_t OExtent>
        constexpr auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return span<OT, OExtent>(sp.data(), sp.size());
        }
    };

    //Note: some of these deduction guides point to invalid constructor.
    // It is the role of the constructor to filter out invalid cases.
    EMU_DEFINE_SPAN_DEDUCTION_GUIDES

} // namespace cuda::managed

    template<typename T, std::size_t Extent>
    struct spe::md_converter<cuda::managed::span<T, Extent>>
    {
        static constexpr auto convert(cuda::managed::span<T, Extent> s) noexcept {
            return cuda::managed::mdspan<T, extents<size_t, Extent>, layout_right, default_accessor<T> >{s.data(), s.size()};
        }
    };

} // namespace emu
