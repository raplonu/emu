#pragma once

#include <emu/cuda.hpp>

#include <pybind11/cast.h>


PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::stream_t >
    {
        PYBIND11_TYPE_CASTER(::emu::cuda::stream_t, const_name("cuda::stream"));

        bool load(handle src, bool convert) {
            auto cuda = py::module_::import("cupy").attr("cuda");

            if (isinstance(src, cuda.attr("Stream"))
             or isinstance(src, cuda.attr("ExternalStream")))
            {
                value = emu::cuda::stream::wrap(
                    src.attr("ptr").cast<::emu::cuda::stream::id_t>(),
                    ::emu::cuda::device_t(src.attr("device_id").cast<::emu::cuda::device::id_t>()),
                    /* take_ownership = */ false
                );
                return PyErr_Occurred();
            }
            return false;
        }

        static pybind11::handle cast(const ::emu::cuda::stream_t& value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            auto cuda = py::module_::import("cupy").attr("cuda");

            return cuda.attr("ExternalStream")(value.id(), value.device_id()).inc_ref();
        }
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
