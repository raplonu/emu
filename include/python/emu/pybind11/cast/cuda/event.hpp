#pragma once

#include <emu/cuda/event.hpp>
#include <emu/optional.hpp>
#include <emu/macro.hpp>

#include <emu/pybind11/format.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/cast.h>

#include <bit>

namespace emu::pybind11::detail
{

    inline optional<emu::cuda::event_ref> event_ref_from_handle(::pybind11::handle src) {
            if (src.is_none()) {
                // If the source is None, we return an empty event_ref
                return ::emu::cuda::event_ref(cudaEvent_t{nullptr});
            }

            auto type = ::pybind11::type::handle_of(src);
            auto module_path = fmt::to_string(type.attr("__module__"));
            if (module_path == "cupy.cuda.stream")
            {
                auto qualified_name = fmt::to_string(type.attr("__qualname__"));

                if (qualified_name == "Event"/*  || qualified_name == "ExternalEvent" */){ //Note: ExternalEvent does not exist in cupy.
                    // If the source is a cupy.cuda.stream, we can wrap it directly
                    auto event_id = std::bit_cast<cudaEvent_t>(src.attr("ptr").cast<uintptr_t>());
                    return ::emu::cuda::event_ref(event_id);
                }
            }
            else if (module_path == "cuda.bindings.runtime")
            {
                auto qualified_name = fmt::to_string(type.attr("__qualname__"));

                if (qualified_name == "cudaEvent_t") {
                    // If the source is a cudaEvent_t, we can wrap it directly
                    auto event_id = std::bit_cast<cudaEvent_t>(src.cast<uintptr_t>());
                    return ::emu::cuda::event_ref(event_id);
                }
            }

            return nullopt;
    }

    inline ::pybind11::handle event_ref_to_handle(::emu::cuda::event_ref value) {
        auto runtime = ::pybind11::module_::import("cuda.bindings.runtime");
        return runtime.attr("cudaEvent_t")(std::bit_cast<uintptr_t>(value.get())).inc_ref();
    }

} // namespace emu::pybind11::detail

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)

namespace detail
{

    template<>
    struct type_caster< ::emu::cuda::event_ref >
    {
        constexpr type_caster() noexcept
            : value(::emu::cuda::event_id(nullptr))
        {}

        PYBIND11_TYPE_CASTER(::emu::cuda::event_ref, const_name("cuda::event_ref"));

        bool load(handle src, bool /* convert */) {
            value = EMU_UNWRAP_OR_RETURN_FALSE(emu::pybind11::detail::event_ref_from_handle(src));
            return true;
        }

        static handle cast(::emu::cuda::event_ref value, return_value_policy /* policy */, pybind11::handle) {
            return emu::pybind11::detail::event_ref_to_handle(value);
        }
    };

    template<>
    struct type_caster< ::emu::cuda::event_handle >
    {
        PYBIND11_TYPE_CASTER(::emu::cuda::event_handle, const_name("cuda::event_handle"));

        bool load(handle src, bool /* convert */) {
            auto event_ref = EMU_UNWRAP_OR_RETURN_FALSE(emu::pybind11::detail::event_ref_from_handle(src));
            value = ::emu::cuda::event_handle::from_native_handle(event_ref.get(), /* take_ownership = */ false);

            return true;
        }

        static handle cast(const ::emu::cuda::event_handle& value, return_value_policy /* policy */, pybind11::handle) {
            return emu::pybind11::detail::event_ref_to_handle(value);
        }
    };

} // namespace detail

PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
