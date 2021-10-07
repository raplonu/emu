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

    template<typename ElementType, typename Location, std::size_t Extent>
    struct python_adaptor<span::detail::span_t<ElementType, Location, Extent>> : std::true_type
    {
        using cpp_type = span::detail::span_t<ElementType, Location, Extent>;
        static_assert(Extent == span::dynamic_extent, "pybind11 required only default constructible types. Fixed size span are not.");

        using bia = buffer_info_adaptor<ElementType, Location>;

        static constexpr auto descr() {
            using namespace ::pybind11::detail;
            return _("span<") + make_caster<ElementType>::name + _("[*], ") + bia::loc_descr() + _(">");
        }

        static optional_t<cpp_type> convert(::pybind11::handle handle) {

            // Try cast handle into buffer.
            return bia::get_buffer_info(handle).and_then([](auto buffer_info) -> optional_t<cpp_type> {
                // Must be only one dimension.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == 1);

                // Must be contigious.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[0] == sizeof(ElementType));

                return optional_t<cpp_type>{in_place,
                    reinterpret_cast<ElementType*>(buffer_info.ptr),
                    buffer_info.size,
                    bia::location(buffer_info)
                };
            });
        }

        static auto convert(const cpp_type& value) {
            return bia::create_handle(mdspan::create(value));
        }

    };

} // namespace emu

} // namespace cast

namespace pybind11
{

namespace detail
{

    template<typename ElementType, typename Location, std::size_t Extent>
    struct type_caster< emu::span::detail::span_t<ElementType, Location, Extent> >
    {
        using adaptor  = emu::cast::python_adaptor<emu::span::detail::span_t<ElementType, Location, Extent>>;
        using cpp_type = typename adaptor::cpp_type;

        PYBIND11_TYPE_CASTER(cpp_type, adaptor::descr());

        bool load(handle src, bool) {
            return adaptor::convert(src)
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