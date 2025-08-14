#pragma once

#include <emu/cuda.hpp>

#include <pybind11/cast.h>
#include <optional>  // Added for optional storage of stream_t


PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::stream_t >
    {
        // Use std::optional to handle stream_t which doesn't have a default constructor
        // stream_t has deleted copy constructor but supports move semantics
        std::optional<::emu::cuda::stream_t> value;

        bool load(handle src, bool convert) {
            auto cuda = module_::import("cupy").attr("cuda");

            // Check if the Python object is a CuPy Stream or ExternalStream
            if (isinstance(src, cuda.attr("Stream"))
             or isinstance(src, cuda.attr("ExternalStream")))
            {
                // Wrap the CuPy stream pointer into an EMU stream_t object
                // We don't take ownership since CuPy manages the stream lifetime
                value = emu::cuda::stream::wrap(
                    reinterpret_cast<::emu::cuda::stream::handle_t>(src.attr("ptr").cast<std::uintptr_t>()),
                    /* take_ownership = */ false
                );
                return !PyErr_Occurred();
            }
            return false;
        }

        static handle cast(const ::emu::cuda::stream_t& value, return_value_policy /* policy */, handle) {
            auto cuda = module_::import("cupy").attr("cuda");

            // Convert EMU stream_t to CuPy ExternalStream using the stream handle and device ID
            return cuda.attr("ExternalStream")(
                reinterpret_cast<std::uintptr_t>(value.handle()),
                value.device_id()
            ).inc_ref();
        }

        static constexpr auto name = const_name("cuda::stream");

        // Conversion operators for pybind11's automatic type casting
        // Returns pointer to the stream if available, nullptr otherwise
        operator ::emu::cuda::stream_t*() { return value ? &*value : nullptr; }
        // Returns reference to the stream, throws cast_error if not available
        operator ::emu::cuda::stream_t&() { if (!value) throw cast_error(); return *value; }

        template <typename T> using cast_op_type = detail::cast_op_type<T>;
    };
} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
