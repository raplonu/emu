#pragma once

#include <emu/cuda.hpp>

#include <pybind11/cast.h>


PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::device_t >
    {
        ::emu::cuda::device_t value{::emu::cuda::device::id_t{0}};

        bool load(handle src, bool convert) {
            auto cuda = module_::import("cupy").attr("cuda");

            if (isinstance(src, cuda.attr("Device"))) {
                value = ::emu::cuda::device::get(src.attr("id").cast<::emu::cuda::device::id_t>());
                return !PyErr_Occurred();
            }
            return false;
        }

        static handle cast(const ::emu::cuda::device_t& value, return_value_policy /* policy */, handle) {
            auto cuda = module_::import("cupy").attr("cuda");

            return cuda.attr("Device")(value.id()).inc_ref();
        }

        static constexpr auto name = const_name("cuda::device");

        // Conversion operators for pybind11's automatic type casting
        operator ::emu::cuda::device_t*() { return &value; }
        operator ::emu::cuda::device_t&() { return value; }

        template <typename T> using cast_op_type = detail::cast_op_type<T>;
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
