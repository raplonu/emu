#pragma once

#include <cuda/api/device.hpp>
#include <cuda/api/types.hpp>
#include <emu/cuda.hpp>

#include <pybind11/cast.h>


PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::cuda::stream_t >
    {
        PYBIND11_TYPE_CASTER(::cuda::stream_t, const_name("cuda::stream"));

        bool load(handle src, bool convert) {
            auto cuda = py::module_::import("cupy").attr("cuda");

            if (isinstance(src, cuda.attr("Stream"))
             or isinstance(src, cuda.attr("ExternalStream")))
            {
                value = emu::cuda::stream::wrap(
                    src.attr("ptr").cast<::cuda::stream::handle_t>(),
                    ::cuda::device::get(src.attr("device_id").cast<::cuda::device::id_t>()),
                    /* take_ownership = */ false
                );
                return PyErr_Occurred();
            }

        }

        static pybind11::handle cast(::cuda::stream_t value, pybind11::return_value_policy /* policy */, pybind11::handle) {
            auto cuda = py::module_::import("cupy").attr("cuda");

            return cuda.attr("ExternalStream")(value.handle(), value.device_id()).inc_ref();
        }
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
