#pragma once

#include <emu/utility.hpp>
#include <emu/pybind11/dltensor.hpp>
// #include <emu/type_traits.hpp>
// #include <emu/optional.hpp>
// #include <emu/mdspan.hpp>
// // #include <emu/cuda/mdspan.hpp>
// #include <emu/pybind11.h>
// #include <emu/pybind11/numpy.h>

// #include <pybind11/cast.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
// #include <pybind11/stl.h>

#include <vector>

namespace emu
{

namespace cast
{




    namespace py = ::pybind11;

    template<typename LocationPolicy>
        requires std::same_as<LocationPolicy, no_location_policy> or std::same_as<LocationPolicy, host::location_policy>
    struct location_adaptor
    {

        template<typename Mapping>
        static inline py::object create_array(
            const byte* ptr, py::dtype type, bool read_only,
            const Mapping& mapping, py::object data_owner
        ) {
            auto& extents = mapping.extents();

            std::vector<py::ssize_t> extents; extents.reserve(extents.rank());
            // No need to test if mapping is contiguous since py::array will create the strides anyway.
            std::vector<py::ssize_t> strides; strides.reserve(extents.rank());

            for(size_t i = 0; i < MS::rank(); ++i) {
                extents.push_back(extents.extent(i));
                strides.push_back(mapping.stride(i) * sizeof(T));
            }

            // In order to avoid copying data, we declare a dummy parent.
            // More info here: https://github.com/pybind/pybind11/issues/323#issuecomment-575717041
            // py::str dummy_data_owner;
            auto res = py::array{type, move(extents), move(strides), ptr, data_owner};

            // Only way I found to force read only from const span.
            // More info here: https://github.com/pybind/pybind11/issues/481#issue-187301065
            if (read_only)
                py::detail::array_proxy(res.ptr())->flags &= ~py::detail::npy_api::NPY_ARRAY_WRITEABLE_;

            return res;
        }

        template<typename ElementType, typename Mapping>
        static inline optional<Mapping> mapping_from(py::handle handle)
        {
            if (py::buffer::check_(handle))
                try
                {
                    // Requesting may fail for severals reasons including constness incompatibility.
                    auto buffer_info = py::buffer(handle, /* is_borrowed = */ true).request(/* writable = */ is_const<ElementType>);

                    // Not using `py::format_descriptor` anymore since format are not consistent with buffer_info...
                    // if (py::format_descriptor<value_type>::format() == buffer_info.format)
                    if (py::dtype::of<rm_const<ElementType>>() == py::dtype::from_args(py::str(buffer_info.format))) {

                        return buffer_info;
                    }
                }
                catch(py::error_already_set&) { } // ignore the exception and return nullopt

            return nullopt;
        }

    };

#ifdef EMU_CUDA

    template<>
    struct location_adaptor<cuda::device_location_policy>
    {

        template<typename Mapping>
        static inline py::object create_array(
            const std::byte* ptr, py::dtype type, bool read_only,
            const Mapping& mapping, py::object data_owner
        ) {
            using namespace py::literals; // to bring in the `_a` literal

            auto& extents = mapping.extents();

            py::list extents(extents.rank());
            py::list strides(extents.rank());

            for(size_t i = 0; i < MS::rank(); ++i) {
                extents[i] = extents.extent(i);
                strides[i] = mapping.stride(i) * sizeof(T);
            }


            auto i_ptr = reinterpret_cast<std::uintptr_t>(ptr);

            auto device_id = cuda::memory::pointer::wrap(ptr).device().id();

            auto cupy = py::module_::import("cupy");
            auto cuda = cupy.attr("cuda");

            auto memory     = cuda.attr("UnownedMemory")(i_ptr, mapping.required_span_size() * type.itemsize(), 0, device_id);
            auto memory_ptr = cuda.attr("MemoryPointer")(memory, 0);

            // There is no way for now to return read only array.
            return cupy.attr("ndarray")(
                "memptr"_a=memory_ptr,
                "dtype"_a=type,
                "shape"_a=extents,
                "strides"_a=strides
            );
        }

        template<typename ElementType, typename Mapping>
        static inline optional<Mapping> mapping_from(py::handle handle)
        {
            //TODO: use handle.__cuda__array_interface__

            return nullopt;
        }

    };

#endif // EMU_CUDA

    template<typename T, typename LocationPolicy>
    struct buffer_info_adaptor
    {
        using element_type = T;
        using value_type = rm_cv<T>;

        using array_type = py::array_t<element_type>;

        static constexpr auto loc_descr() {
            return py::detail::_("???");
        }

        static optional<py::buffer_info> get_buffer_info(py::handle handle) {
            // Check if handle implement buffer protocol.
            if (py::buffer::check_(handle))
                try {
                    // Requesting may fail for severals reasons including constness incompatibility.
                    auto buffer_info = py::buffer(handle, /* is_borrowed = */ true).request(/* writable = */ not is_const<element_type>);

                    // Not using `py::format_descriptor` anymore since format are not consistent with buffer_info...
                    // if (py::format_descriptor<value_type>::format() == buffer_info.format)
                    if (py::dtype::of<value_type>() == py::dtype::from_args(py::str(buffer_info.format))) {

                        return buffer_info;
                    }
                }
                catch(py::error_already_set&) { } // ignore the exception and return nullopt

            return nullopt;
        }

        /**
         * @brief Check if handle is an instance of numpy array. Cast if possible.
         */
        static optional<array_type> cast_array(py::handle handle) {
            if ( py::isinstance<array_type>(handle) ) {
                fmt::print("handle is array_type !\n");
                return array_type::ensure(handle);
            } else
                return nullopt;
        }

        /**
         * @brief Create a array object
         *
         * @param handle
         * @return optional<array_type>
         */
        static optional<array_type> create_array(py::handle handle) {
            // Check if handle implement buffer protocol and copy if type mismatch.
            auto array_maybe_copy = array_type::ensure(handle);

            if (not array_maybe_copy)
                return nullopt;

            // If the array is not writable, we need to create a writable copy.
            if (not std::is_const_v<T> and not array_maybe_copy.writeable())
                // Create a new array with the new buffer and copied data.
                return array_type(array_maybe_copy.request());
            else
                return array_maybe_copy;
        }


        template <typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        static auto create_handle(const emu::mdspan<T, Extents, LayoutPolicy, AccessorPolicy> & value, py::object data_owner) {

            using MS = emu::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>;

            // Sadge to create vectors...
            std::vector<py::ssize_t> extents; extents.reserve(MS::rank());
            std::vector<py::ssize_t> strides; strides.reserve(MS::rank());

            for(size_t i = 0; i < MS::rank(); ++i) {
                extents.push_back(value.extends().extent(i));
                strides.push_back(value.stride(i) * sizeof(T));
            }

            return array_factory<LocationPolicy>::create_array(
                reinterpret_cast<const std::byte*>(value.data_handle()), py::dtype::of<T>(),
                std::is_const_v<T>, extents, strides, data_owner
            );
        }

    };

} // namespace cast

} // namespace emu
