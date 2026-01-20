#pragma once

#include <emu/tensor_traits.hpp>
#include <emu/pybind11/cast/detail/tensor_caster.hpp>

#include <emu/span.hpp>

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<emu::cpts::span Span>
    struct type_caster< Span >
    {

        using cpp_type = Span;

        // span location
        using accessor_type = emu::tensor_traits<cpp_type>::accessor_type;

        // mdspan equivalent of the span (mdspan_1d)
        using mdspan_type = emu::mdspan_type_t<cpp_type>;

        using base_caster = emu::cast::detail::tensor_caster_for<accessor_type>::template md_caster<mdspan_type>;

        using element_type  = typename cpp_type::element_type;

        static_assert(cpp_type::extent == std::dynamic_extent, "pybind11 required only default constructible types. Fixed size span are not.");

        static constexpr auto descr() {
            return _("span<") + make_caster<element_type>::name + _("[*]>");
        }

        PYBIND11_TYPE_CASTER(cpp_type, descr());

        bool load(handle src, bool convert) {
            return base_caster::from_python(src, convert).map([&](auto new_value){
                value = cpp_type(new_value.data_handle(), new_value.size());
                return true;
            }).value_or(false);
        }

        static pybind11::handle cast(cpp_type value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            // In order to avoid copying data, we declare a dummy parent.
            // More info here: https://github.com/pybind/pybind11/issues/323#issuecomment-575717041
            // return ::pybind11::none();
            ::pybind11::str dummy_data_owner;
            return base_caster::to_python(emu::as_tensor(value), dummy_data_owner).inc_ref();
        }
    };

} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
