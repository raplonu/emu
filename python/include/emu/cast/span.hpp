#pragma once

#include <emu/cast/common.hpp>

namespace emu
{

namespace cast
{



} // namespace cast

} // namespace emu

namespace pybind11
{

namespace detail
{
    template<emu::cpts::span Span>
    struct type_caster< Span >
    {
        // using adaptor  = emu::cast::python_adaptor<emu::detail::span<ElementType, Location, Extent>>;
        using cpp_type = Span;

        static_assert(cpp_type::extent == std::dynamic_extent, "pybind11 required only default constructible types. Fixed size span are not.");

        using element_type  = typename cpp_type::element_type;
        using bia = emu::cast::buffer_info_adaptor<element_type>;

        static constexpr auto descr() {
            using namespace ::pybind11::detail;
            return _("span<") + make_caster<element_type>::name + _("[*]>");
        }

        PYBIND11_TYPE_CASTER(cpp_type, descr());

        bool load(handle src, bool convert) {
            return bia::get_buffer_info(src).and_then([](auto buffer_info) -> emu::optional<cpp_type> {
                // Must be only one dimension.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == 1);

                // Must be contigious.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.strides[0] == sizeof(element_type));

                return emu::optional<cpp_type>{emu::in_place,
                    reinterpret_cast<element_type*>(buffer_info.ptr),
                    buffer_info.size
                };
            }).map([&](auto && new_value){
                value = emu::mv(new_value);
                return !PyErr_Occurred();
            })
            .value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            // In order to avoid copying data, we declare a dummy parent.
            // More info here: https://github.com/pybind/pybind11/issues/323#issuecomment-575717041
            ::pybind11::str dummy_data_owner;
            return bia::create_handle(emu::as_mdspan(value), dummy_data_owner).inc_ref();
        }
    };

} // namespace detail

} // namespace pybind11