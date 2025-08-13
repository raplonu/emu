#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_container.hpp>
#include <emu/cuda/host/mdcontainer.hpp>
#include <emu/cuda.hpp>
#include <emu/mdalgo.hpp>

namespace emu
{
namespace cuda::host
{

    template <typename ElementType, size_t Extent>
    struct container : emu::detail::basic_container<ElementType, Extent, cuda::host_location_policy, container<ElementType, Extent>>
    {
        using base = emu::detail::basic_container<ElementType, Extent, cuda::host_location_policy, container>;

        using base::base;

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T, std::dynamic_extent> make_container(size_t size) {
        auto u_ptr = host::make_unique<T[]>(size);
        return container<T, std::dynamic_extent>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace cuda::host

    template<typename T, std::size_t Extent>
    struct spe::md_converter<cuda::host::container<T, Extent>>
    {
        static constexpr auto convert(const cuda::host::container<T, Extent> &s) noexcept {
            return cuda::host::mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >(s.data(), s.capsule(), emu::exts_flag, s.size());
        }
    };


} // namespace emu
