#pragma once

#include <emu/concepts.hpp>
#include <emu/location_policy.hpp>
#include <emu/host/location_policy.hpp>
#include <emu/pybind11/cast/detail/layout_adaptor.hpp>

#ifdef EMU_CUDA
#include <emu/cuda/device/location_policy.hpp>
#endif // EMU_CUDA

namespace emu::cast::detail
{
    namespace py = ::pybind11;

    template<cpts::mdspan Mdspan>
        requires std::same_as<location_type_of<Mdspan>, no_location_policy> or std::same_as<location_type_of<Mdspan>, host::location_policy>
    struct mdspan_caster
    {
        using cpp_type = Mdspan;
        using element_type = typename cpp_type::element_type;
        using value_type = typename cpp_type::value_type;
        using extents_type = typename cpp_type::extents_type;
        using layout_type = typename cpp_type::layout_type;
        using mapping_type = typename layout_type::template mapping<extents_type>;

        using location_type = location_type_of<cpp_type>;

        using layout_adaptor = detail::layout_adaptor<element_type, extents_type, layout_type>;

        static py::dtype dtype() noexcept
        {
            return py::dtype::of<value_type>();
        }

        static bool check(py::handle handle) noexcept {
            return py::buffer::check_(handle);
        }

        static inline optional<py::buffer_info> buffer_info_from(py::handle handle)
        {
            if (check(handle))
                try
                {
                    // Requesting may fail for severals reasons including constness incompatibility.
                    auto buffer_info = py::reinterpret_borrow<py::buffer>(handle).request(/* writable = */ not is_const<element_type>);

                    // Not using `py::format_descriptor` anymore since format are not consistent with buffer_info...
                    // if (py::format_descriptor<value_type>::format() == buffer_info.format)
                    if (dtype().is(py::dtype::from_args(py::str(buffer_info.format)))) {

                        return buffer_info;
                    }
                }
                catch(py::error_already_set&) { } // ignore the exception and return nullopt

            return nullopt;
        }

        static optional<cpp_type> from_python(py::handle handle, bool convert)
        {
            return buffer_info_from(handle).and_then([](auto buffer_info) -> optional<cpp_type> {
                // Must have the same rank.
                EMU_TRUE_OR_RETURN_NULLOPT(buffer_info.ndim == cpp_type::rank());

                for(auto i = 0; i < cpp_type::rank(); ++i)
                    // Must be Extent if span is not dynamic.
                    EMU_TRUE_OR_RETURN_NULLOPT(cpp_type::static_extent(i) == std::dynamic_extent or static_cast<size_t>(buffer_info.shape[i]) == cpp_type::static_extent(i));

                return layout_adaptor::mapping_from(buffer_info).map([ptr = buffer_info.ptr] (auto mapping) {
                    return cpp_type(reinterpret_cast<element_type*>(ptr), mapping);
                });
            });
        }

        static py::object to_python(const cpp_type& value, py::handle parent)
        {
            auto& mapping = value.mapping();
            auto& extents = mapping.extents();

            std::vector<py::ssize_t> shape; shape.reserve(extents.rank());
            // No need to test if mapping is contiguous since py::array will create the strides anyway.
            std::vector<py::ssize_t> strides; strides.reserve(extents.rank());

            for(size_t i = 0; i < cpp_type::rank(); ++i) {
                shape.push_back(extents.extent(i));
                strides.push_back(mapping.stride(i) * sizeof(value_type));
            }

            // In order to avoid copying data, we declare a dummy parent.
            // More info here: https://github.com/pybind/pybind11/issues/323#issuecomment-575717041
            // py::str dummy_data_owner;
            auto res = py::array{dtype(), move(shape), move(strides), value.data_handle(), parent};

            // Only way I found to force read only from const span.
            // More info here: https://github.com/pybind/pybind11/issues/481#issue-187301065
            if (is_const<element_type>)
                py::detail::array_proxy(res.ptr())->flags &= ~py::detail::npy_api::NPY_ARRAY_WRITEABLE_;

            return res;
        }

    };

#ifdef EMU_CUDA

    // template<cpts::mdspan Mdspan>
    //     requires std::same_as<location_type_of<Mdspan>, cuda::device_location_policy>
    // struct mdspan_caster
    // {

    // }

#endif // EMU_CUDA

} // namespace emu::cast::detail
