#pragma once

#include <emu/concepts.hpp>
#include <emu/location_policy.hpp>
#include <emu/host/location_policy.hpp>
#include <emu/pybind11/cast/detail/layout_adaptor.hpp>

#ifdef EMU_CUDA
#include <emu/cuda/device/location_policy.hpp>
#include <emu/cuda.hpp>
#endif // EMU_CUDA

namespace emu::cast::detail
{
    namespace py = ::pybind11;


    template<typename LocationPolicy>
    struct mdspan_caster_for;

    template<cpts::mdspan Mdspan>
        // requires std::same_as<location_type_of<Mdspan>, no_location_policy>
        //       or std::same_as<location_type_of<Mdspan>, host::location_policy>
    struct host_mdspan_caster
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

                for(size_t i = 0; i < cpp_type::rank(); ++i)
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


    template<>
    struct mdspan_caster_for<no_location_policy>
    {
        template<typename Mdspan>
        using md_caster = host_mdspan_caster<Mdspan>;

    };


    template<>
    struct mdspan_caster_for<host::location_policy>
    {
        template<typename Mdspan>
        using md_caster = host_mdspan_caster<Mdspan>;

    };


#ifdef EMU_CUDA

    // Have to explicitly reference device::mdspan because dump compiler.
    // template<typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        // requires std::same_as<location_type_of<Mdspan>, cuda::device_location_policy>
    template<cuda::device::cpts::mdspan MdSpan>
    struct cuda_device_mdspan_caster
    {

        using cpp_type = MdSpan;
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
            return py::hasattr(handle, "__cuda_array_interface__");
        }

        static optional<cpp_type> from_python(py::handle handle, bool convert)
        {
            return pybind11::try_attr(handle, "__cuda_array_interface__")
                .and_then(pybind11::try_cast<py::dict>)
                .and_then([&](py::dict cai) -> optional<cpp_type>
            {
                auto shape = cai["shape"].cast<py::list>();

                EMU_TRUE_OR_RETURN_NULLOPT(shape.size() == cpp_type::rank());

                for(size_t i = 0; i < cpp_type::rank(); ++i)
                    // Must be Extent if span is not dynamic.
                    EMU_TRUE_OR_RETURN_NULLOPT(cpp_type::static_extent(i) == std::dynamic_extent or shape[i].cast<size_t>() == cpp_type::static_extent(i));


                // py::list strides;
                // if (auto s = try_attr(cai, "strides"); s)
                //     strides = s->cast<py::list>();

                // auto data = cai.attr("data");

                auto ptr = cai["data"].cast<py::tuple>()[0].cast<uintptr_t>();

                return layout_adaptor::mapping_from(cai).map([ptr] (auto mapping) {
                    return cpp_type(reinterpret_cast<element_type*>(ptr), mapping);
                });

            });
        }

        static py::object to_python(const cpp_type& value, py::handle parent)
        {
            auto& mapping = value.mapping();
            auto& m_extents = mapping.extents();

            using namespace py::literals; // to bring in the `_a` literal

            py::list extents(m_extents.rank());
            py::list strides(m_extents.rank());

            for(size_t i = 0; i < cpp_type::rank(); ++i) {
                extents[i] = m_extents.extent(i);
                strides[i] = mapping.stride(i) * sizeof(value_type);
            }

            auto ptr = value.data_handle();

            auto i_ptr = reinterpret_cast<std::uintptr_t>(ptr);

            auto device_id = ::cuda::memory::pointer::wrap(ptr).device().id();

            auto cupy = py::module_::import("cupy");
            auto cuda = cupy.attr("cuda");

            auto memory = cuda.attr("UnownedMemory")(
                /* ptr = */ i_ptr, /* size = */ mapping.required_span_size() * sizeof(value_type),
                /* owner = */ parent, /* device_id = */ device_id
            );

            //TODO: get the start of the provided device pointer and then specify the right offset.
            auto memory_ptr = cuda.attr("MemoryPointer")(memory, /* offset = */ 0);

            // There is no way for now to return read only array.
            return cupy.attr("ndarray")(
                "memptr"_a=memory_ptr,
                "dtype"_a=py::dtype::of<value_type>(),
                "shape"_a=extents,
                "strides"_a=strides
            );
        }

    };

    template<>
    struct mdspan_caster_for<cuda::device_location_policy>
    {
        template<typename Mdspan>
        using md_caster = cuda_device_mdspan_caster<Mdspan>;

    };

#endif // EMU_CUDA

} // namespace emu::cast::detail
