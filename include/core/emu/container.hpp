#pragma once

#include <emu/detail/basic_container.hpp>
#include <emu/mdcontainer.hpp>
#include <emu/location_policy.hpp>
#include <emu/mdalgo.hpp>

namespace emu
{

    template <typename ElementType, size_t Extent>
    struct container : emu::detail::basic_container<ElementType, Extent, no_location_policy, container<ElementType, Extent>>
    {
        using base = emu::detail::basic_container<ElementType, Extent, no_location_policy, container>;

        using value_type = typename base::value_type;

        using base::base;

        //Note: Due to a bug with gcc 11, we need to define this constructor here.
        constexpr container(std::initializer_list<value_type> il)
            : base(std::vector(il))
        {}

        template<typename OT, size_t OExtent>
        auto from_span(std::span<OT, OExtent> sp) const {
            return container<OT, OExtent>(sp, static_cast<const capsule&>(*this));
        }
    };

    EMU_DEFINE_CONTAINER_DEDUCTION_GUIDES

    template<typename T>
    container<T> make_container(size_t size) {
        auto u_ptr = std::make_unique<T[]>(size);
        return container<T>(u_ptr.get(), size, move(u_ptr));
    }

    template<typename T, size_t Extent>
    struct spe::md_converter<container<T, Extent>>
    {
        static constexpr auto convert(const container<T, Extent>& s) noexcept {
            return mdcontainer<T, extents<size_t, Extent>, layout_right, default_accessor<T> >(s.data(), s.capsule(), exts_flag, s.size());
        }
    };

} // namespace emu
