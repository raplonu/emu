#pragma once

#include <emu/fwd.hpp>
#include <emu/concepts.hpp>
#include <emu/type_name.hpp>
#include <emu/detail/basic_mdspan.hpp>
#include <emu/mdalgo.hpp>
namespace emu
{

    using stdex::mdspan;

    EMU_DEFINE_MDSPAN_ALIAS

namespace spe
{

//     template <typename ElementType, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
//     struct md_converter<mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>> {
//         static constexpr auto as_md(const mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>& md) noexcept {
//             return md;
//         }
//     };

    template <std::ranges::contiguous_range T>
    struct md_converter<T> {

        template <typename TT>
        static constexpr auto convert(TT&& t) noexcept {
            return mdspan(std::ranges::data(t), std::ranges::size(t));
        }
    };

} // namespace spe

} // namespace emu
