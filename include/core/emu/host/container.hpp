#pragma once

#include <emu/type_traits.hpp>
#include <emu/detail/basic_container.hpp>
#include <emu/host/mdcontainer.hpp>
#include <emu/host/location_policy.hpp>
#include <emu/mdalgo.hpp>

namespace emu
{
namespace host
{

    template <typename ElementType, size_t Extent>
    struct container : emu::detail::basic_container<ElementType, Extent, host::location_policy, container<ElementType, Extent> >
    {
        using base = emu::detail::basic_container<ElementType, Extent, host::location_policy, container>;

        using value_type = typename base::value_type;

        using base::base;

        //Note: Do to a bug with gcc 11, we need to define this constructor here.
        constexpr container(std::initializer_list<value_type> il)
            : base(std::vector(il))
        {}

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const noexcept {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T> make_container(size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, std::move(u_ptr));
    }

} // namespace host

    template<typename T, std::size_t Extent>
    struct spe::md_converter<host::container<T, Extent>>
    {
        static constexpr auto convert(const host::container<T, Extent> &s) noexcept {
            return host::mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >(s.data(), s.capsule(), exts_flag, s.size());
        }
    };

} // namespace emu
