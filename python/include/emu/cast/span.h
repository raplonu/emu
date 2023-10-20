#ifndef EMU_CAST_SPAN_H
#define EMU_CAST_SPAN_H

#include <emu/cast/common.h>
#include <emu/type_traits.h>
#include <emu/optional.h>
#include <emu/cast/mdspan.h>
#include <emu/span.h>
#include <emu/mdspan.h>
#ifdef EMU_CUDA
#include <emu/cuda/span.h>
#endif

#include <pybind11/numpy.h>

namespace emu
{

namespace cast
{

    // template<typename ElementType, typename Location, std::size_t Extent>
    template<cpts::span Span>
    struct python_adaptor<Span> : std::true_type
    {
        // using cpp_type = emu::detail::span<ElementType, Location, Extent>;
        using cpp_type = Span;

        using element_type  = typename cpp_type::element_type;
        using location_type = location_or_default_type<cpp_type>;
        static constexpr auto Extent = cpp_type::extent;

        static_assert(Extent == std::dynamic_extent, "pybind11 required only default constructible types. Fixed size span are not.");

        using bia = buffer_info_adaptor<element_type, location_type>;

        bia bia_;

        static constexpr auto descr() {
            using namespace ::pybind11::detail;
            return _("span<") + make_caster<element_type>::name + _("[*], ") + bia::loc_descr() + _(">");
        }

        optional<cpp_type> convert(::pybind11::handle handle, bool can_convert) {

            // Try cast handle into buffer.
            return bia_.get_buffer_info(handle, can_convert).and_then([](auto buffer_info) -> optional<cpp_type> {
                // Must be only one dimension.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == 1);

                // Must be contigious.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[0] == sizeof(element_type));

                if constexpr (cpts::has_location<cpp_type>) {
                    return optional<cpp_type>{in_place,
                        reinterpret_cast<element_type*>(buffer_info.ptr),
                        buffer_info.size,
                        bia::location(buffer_info)
                    };
                } else {
                    return optional<cpp_type>{in_place,
                        reinterpret_cast<element_type*>(buffer_info.ptr),
                        buffer_info.size
                    };
                }
            });
        }

        static auto convert(const cpp_type& value) {
            return bia::create_handle(emu::detail::mdspan<element_type, location_type, extents<std::size_t, Extent>>(value.data(), get_location(value), value.size()));
        }

    };

} // namespace emu

} // namespace cast

namespace pybind11
{

namespace detail
{

    // template<typename ElementType, typename Location, std::size_t Extent>
    template<emu::cpts::span Span>
    struct type_caster< Span >
    {
        // using adaptor  = emu::cast::python_adaptor<emu::detail::span<ElementType, Location, Extent>>;
        using adaptor  = emu::cast::python_adaptor<Span>;
        using cpp_type = typename adaptor::cpp_type;

        adaptor adaptor_;

        PYBIND11_TYPE_CASTER(cpp_type, adaptor::descr());

        bool load(handle src, bool convert) {
            return adaptor_.convert(src, convert)
                .map([&](auto && new_value){
                    value = emu::mv(new_value);
                    return !PyErr_Occurred();
                })
                .value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            return adaptor::convert(value).inc_ref();
        }
    };

} // namespace detail

} // namespace pybind11

#endif //EMU_CAST_SPAN_H