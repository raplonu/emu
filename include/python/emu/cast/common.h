#ifndef EMU_CAST_COMMON_H
#define EMU_CAST_COMMON_H

#include <emu/type_traits.h>
#include <emu/optional.h>
#include <emu/span.h>
#include <emu/mdspan.h>
#if EMU_CUDA
#include <emu/cuda.h>
#include <emu/cuda/span.h>
#include <emu/cuda/mdspan.h>
#endif

#include <boost/endian/conversion.hpp>

#include <pybind11/cast.h>
#include <pybind11/numpy.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

namespace cast
{

    namespace py = pybind11;

    template<typename T>
    struct python_adaptor : std::false_type {};

    template<typename ElementType, typename Location>
    struct buffer_info_adaptor;

namespace detail
{

    inline optional_t<py::handle> not_none(py::handle handle) {
        if (handle.is_none())
            return nullopt;
        else
            return handle;
    }

    template<typename T>
    T cast(py::handle handle) {
        return handle.cast<T>();
    }

    struct shape_strides {
        std::vector<std::size_t> shape;
        std::vector<std::size_t> strides;
    };

    shape_strides shape_and_stride(py::handle cai, std::size_t word_size);

} // namespace detail

    template<typename ElementType>
    struct buffer_info_adaptor<ElementType, location::host_t> {

        using value_type = RemoveCV<ElementType>;

        static constexpr auto loc_descr() noexcept {
            using namespace pybind11::detail;
            return _("host");
        }

        static optional_t<py::buffer_info> get_buffer_info(py::handle handle) {
            // Check if handle implement buffer protocol.
            if (py::buffer::check_(handle))
                try
                {
                    // Requesting may fail for severals reasons including constness incompatibility.
                    auto buffer_info = py::buffer(handle, /* is_borrowed = */ true).request(/* writable = */ not IsConst<ElementType>);
                    if (py::format_descriptor<value_type>::format() == buffer_info.format)
                        return buffer_info;
                }
                catch(const py::error_already_set&) {} // ignore the exception and return nullopt

            return nullopt;
        }

        constexpr static location::host_t location(const py::buffer_info& bi) noexcept {
            return {}; // host_t is a monostate.
        }

        template <typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        static auto create_handle(const emu::mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy> & value) {

            using mdspan = emu::mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

            std::vector<std::size_t> extents; extents.reserve(mdspan::rank());
            std::vector<std::size_t> strides; strides.reserve(mdspan::rank());
            for(std::size_t i = 0; i < mdspan::rank(); ++i) {
                extents.push_back(value.extent(i));
                strides.push_back(value.stride(i) * sizeof(ElementType));
            }

            pybind11::str dummyDataOwner;
            auto res = py::array_t<ElementType>{mv(extents), mv(strides), value.data(), dummyDataOwner};

            if (IsConst<ElementType>)
                py::detail::array_proxy(res.ptr())->flags &= ~py::detail::npy_api::NPY_ARRAY_WRITEABLE_;

            return res;
        }

    };

#if EMU_CUDA

namespace detail
{

    pybind11::module_& cupy_module();

    pybind11::module_& cuda_module();

} // namespace detail

    template<typename ElementType>
    struct buffer_info_adaptor<ElementType, location::cuda_t> {

        using value_type = RemoveCV<ElementType>;

        static constexpr auto loc_descr() {
            using namespace pybind11::detail;
            return _("cuda");
        }

        static optional_t<py::buffer_info> get_buffer_info(py::handle handle) {

            if (py::hasattr(handle, "__cuda_array_interface__"))
            {
                auto cai = handle.attr("__cuda_array_interface__");

                auto s_and_s = detail::shape_and_stride(cai, sizeof(value_type));

                auto fmt_src = cai["typestr"].cast<std::string>();
                auto dt_dst = py::dtype::of<value_type>();

                auto byteorder = boost::endian::order::native == boost::endian::order::big ? '>' : '<';

                if(fmt_src == fmt::format("{}{}{}", byteorder, dt_dst.kind(), dt_dst.itemsize()))
                {
                    return optional_t<py::buffer_info>{in_place,
                        reinterpret_cast<value_type*>(cai["data"].cast<py::tuple>()[0].cast<std::uintptr_t>()),
                        mv(s_and_s.shape),
                        mv(s_and_s.strides),
                        not IsConst<ElementType> // There is no way to check mutability from cuda array...
                    };
                }
            }

            return nullopt;
        }

        static location::cuda_t location(const py::buffer_info& bi) {
            ::cuda::memory::pointer_t<void> ptr{bi.ptr};

            // Only way to consistently retreive device location.
            return {::cuda::device::get(ptr.attributes().device)};
        }

        template <typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        static py::object create_handle(const emu::cuda::mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy> & value) {

            using mdspan = emu::cuda::mdspan_t<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

            py::list extents;//(mdspan::rank());// extents.reserve(mdspan::rank());
            py::list strides;//(mdspan::rank());// strides.reserve(mdspan::rank());
            auto memory_range = 1;
            for(std::size_t i = 0; i < mdspan::rank(); ++i) {
                memory_range += (value.extent(i) - 1) * value.stride(i);
                extents.append(value.extent(i));
                strides.append(value.stride(i) * sizeof(ElementType));
            }

            using namespace pybind11::literals; // to bring in the `_a` literal

            auto ptr = reinterpret_cast<std::uintptr_t>(value.data());

            auto & cuda = detail::cuda_module();

            // fmt::print("memory_range = {}\n", memory_range);

            auto memory     = cuda.attr("UnownedMemory")(ptr, memory_range * sizeof(ElementType), 0, value.location().device.id());
            auto memory_ptr = cuda.attr("MemoryPointer")(memory, 0);

            // py::tuple py_extents = py::cast(extents);
            // py::tuple py_strides = py::cast(strides);

            // fmt::print("NIK\n");

            // fmt::print("py_extents = {}\n", extents[0].cast<std::size_t>());
            // fmt::print("py_strides = {}\n", strides[0].cast<std::size_t>());

            return detail::cupy_module().attr("ndarray")(
                "memptr"_a=memory_ptr,
                "dtype"_a=pybind11::format_descriptor<ElementType>::format(),
                "shape"_a=extents,
                "strides"_a=strides
            );

            // fmt::print("HERE IS OK\n");

            // return res.inc_ref();
        }

    };

#endif

} // namespace cast

} // namespace emu

#endif //EMU_CAST_COMMON_H