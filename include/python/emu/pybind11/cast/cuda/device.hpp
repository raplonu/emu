#pragma once

#include <emu/cuda.hpp>

#include <pybind11/cast.h>


PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::device_t >
    {
        PYBIND11_TYPE_CASTER(::emu::cuda::device_t, const_name("cuda::device"));

        bool load(handle src, bool convert) {
            namespace py = pybind11;
            auto cuda = py::module_::import("cupy").attr("cuda");

            if (py::isinstance(src, cuda.attr("Device"))) {
                value = ::emu::cuda::device::get(src.attr("id").cast<::emu::cuda::device::id_t>());
                return !PyErr_Occurred();
            }
            return false;
        }

        static pybind11::handle cast(const ::emu::cuda::device_t& value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            namespace py = pybind11;
            auto cuda = py::module_::import("cupy").attr("cuda");

            return cuda.attr("Device")(value.id()).inc_ref();
        }
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
