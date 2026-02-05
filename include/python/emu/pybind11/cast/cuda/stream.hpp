#pragma once

#include <emu/cuda/stream.hpp>
#include <emu/optional.hpp>
#include <emu/macro.hpp>

#include <emu/pybind11/format.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/cast.h>

#include <bit>

namespace emu::pybind11::detail
{

    inline optional<emu::cuda::stream_ref> stream_ref_from_handle(::pybind11::handle src) {
            if (src.is_none()) {
                // If the source is None, we return an empty stream_ref
                return ::emu::cuda::stream_ref(cudaStream_t{nullptr});
            }

            auto type = ::pybind11::type::handle_of(src);
            auto module_path = fmt::to_string(type.attr("__module__"));
            if (module_path == "cupy.cuda.stream")
            {
                auto qualified_name = fmt::to_string(type.attr("__qualname__"));

                if (qualified_name == "Stream" || qualified_name == "ExternalStream"){
                    // If the source is a cupy.cuda.stream, we can wrap it directly
                    auto stream_id = std::bit_cast<cudaStream_t>(src.attr("ptr").cast<uintptr_t>());
                    return ::emu::cuda::stream_ref(stream_id);
                }
            }
            else if (module_path == "cuda.bindings.runtime")
            {
                auto qualified_name = fmt::to_string(type.attr("__qualname__"));

                if (qualified_name == "cudaStream_t") {
                    // If the source is a cudaStream_t, we can wrap it directly
                    auto stream_id = std::bit_cast<cudaStream_t>(src.cast<uintptr_t>());
                    return ::emu::cuda::stream_ref(stream_id);
                }
            }

            return nullopt;
    }

    inline ::pybind11::handle stream_ref_to_handle(::emu::cuda::stream_ref value) {
        auto runtime = ::pybind11::module_::import("cuda.bindings.runtime");
        return runtime.attr("cudaStream_t")(std::bit_cast<uintptr_t>(value.get())).inc_ref();
    }

} // namespace emu::pybind11::detail

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::stream_ref >
    {
        constexpr type_caster() noexcept
            : value(::emu::cuda::stream_id(nullptr))
        {}

        PYBIND11_TYPE_CASTER(::emu::cuda::stream_ref, const_name("cuda::stream_ref"));

        bool load(handle src, bool /* convert */) {
            value = EMU_UNWRAP_OR_RETURN_FALSE(emu::pybind11::detail::stream_ref_from_handle(src));
            return true;
        }

        static handle cast(::emu::cuda::stream_ref value, return_value_policy /* policy */, pybind11::handle) {
            return emu::pybind11::detail::stream_ref_to_handle(value);
        }
    };

    template<>
    struct type_caster< ::emu::cuda::stream_handle >
    {
        PYBIND11_TYPE_CASTER(::emu::cuda::stream_handle, const_name("cuda::stream_handle"));

        bool load(handle src, bool /* convert */) {
            auto stream_ref = EMU_UNWRAP_OR_RETURN_FALSE(emu::pybind11::detail::stream_ref_from_handle(src));
            value = ::emu::cuda::stream_handle::from_native_handle(stream_ref.get(), /* take_ownership = */ false);

            return true;
        }

        static handle cast(const ::emu::cuda::stream_handle& value, return_value_policy /* policy */, pybind11::handle) {
            return emu::pybind11::detail::stream_ref_to_handle(value);
        }
    };

} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
