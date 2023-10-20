#ifndef EMU_CAST_COMMON_H
#define EMU_CAST_COMMON_H

#include <emu/type_traits.h>
#include <emu/optional.h>
#include <emu/range.h>
#include <emu/pybind11.h>
#include <emu/pybind11/numpy.h>
#include <emu/span.h>
#include <emu/mdspan.h>
#ifdef EMU_CUDA
#include <emu/cuda.h>
#include <emu/cuda/span.h>
#include <emu/cuda/mdspan.h>
#endif

#include <boost/endian/conversion.hpp>

#include <pybind11/cast.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

namespace emu
{

namespace cast
{

    namespace py = ::pybind11;

    template<typename T>
    struct python_adaptor : std::false_type {};

    template<typename ElementType, typename Location>
    struct buffer_info_adaptor;

namespace detail
{

    struct extents_strides {
        std::vector<py::ssize_t> extents;
        std::vector<py::ssize_t> strides;
    };

    inline extents_strides extents_and_stride(py::handle cai, std::size_t word_size) {
        auto extents = cai["shape"].cast<std::vector<py::ssize_t>>();

        auto strides = value_or_invoke(
            pybind11::not_none(cai["strides"]).map(py::cast<std::vector<py::ssize_t>>),
            [&extents, word_size]{return strides_vector<py::ssize_t>(std::span{extents}, word_size); }
        );


        return {mv(extents), mv(strides)};
    }

} // namespace detail

    inline py::object to_array(
        const std::byte* ptr, py::dtype type, bool read_only,
        std::vector<py::ssize_t> extents, std::vector<py::ssize_t> strides,
        location::host /* location */
    ) {
        // In order to avoid copying data, we declare a dummy parent.
        // More info here: https://github.com/pybind/pybind11/issues/323#issuecomment-575717041
        py::str dummy_data_owner;
        auto res = py::array{type, mv(extents), mv(strides), ptr, dummy_data_owner};

        // Only way I found to force read only from const span.
        // More info here: https://github.com/pybind/pybind11/issues/481#issue-187301065
        if (read_only)
            py::detail::array_proxy(res.ptr())->flags &= ~py::detail::npy_api::NPY_ARRAY_WRITEABLE_;

        return res;
    }

    template<typename ElementType>
    struct buffer_info_adaptor<ElementType, location::host> {

        using value_type = std::remove_cv_t<ElementType>;

        static constexpr auto loc_descr() noexcept {
            using namespace py::detail;
            return _("host");
        }

        using buffer_type = py::array_t<ElementType>;

        buffer_type buffer_array;

        optional<py::buffer_info> get_buffer_info(py::handle handle, bool can_convert) {
            // Check if handle implement buffer protocol.
            if (py::buffer::check_(handle))
                try
                {
                    // Requesting may fail for severals reasons including constness incompatibility.
                    auto buffer_info = py::buffer(handle, /* is_borrowed = */ true).request(/* writable = */ not std::is_const_v<ElementType>);

                    // Not using `py::format_descriptor` anymore since format are not consistent with buffer_info...
                    // if (py::format_descriptor<value_type>::format() == buffer_info.format)
                    if (py::dtype::of<value_type>() == py::dtype::from_args(py::str(buffer_info.format)))
                        return buffer_info;
                }
                catch(const py::error_already_set&) { } // ignore the exception and return nullopt

            // if it did not work, try to construct a py::array_t from the handle
            if (can_convert and buffer_type::check_(handle)) {
                buffer_array = buffer_type::ensure(handle);
                return py::buffer(buffer_array).request(/* writable = */ not std::is_const_v<ElementType>);
            }

            return nullopt;
        }

        constexpr static location::host location(const py::buffer_info& bi) noexcept {
            return {}; // host is a monostate.
        }

        template <typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        static auto create_handle(const emu::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> & value) {

            using MS = emu::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

            std::vector<py::ssize_t> extents; extents.reserve(MS::rank());
            std::vector<py::ssize_t> strides; strides.reserve(MS::rank());

            for(std::size_t i = 0; i < MS::rank(); ++i) {
                extents.push_back(value.extent(i));
                strides.push_back(value.stride(i) * sizeof(ElementType));
            }

            return to_array(
                reinterpret_cast<const std::byte*>(value.data_handle()), py::dtype::of<ElementType>(),
                std::is_const_v<ElementType>, extents, strides, value.location()
            );
        }

    };

#ifdef EMU_CUDA

    inline py::object to_array(
        const std::byte* ptr, py::dtype type, bool read_only,
        std::vector<py::ssize_t> extents, std::vector<py::ssize_t> strides,
        const cuda::location::device & location)
    {
        using namespace py::literals; // to bring in the `_a` literal

        auto i_ptr = reinterpret_cast<std::uintptr_t>(ptr);

        auto memory_range = 1;
        for(std::size_t i = 0; i < extents.size(); ++i)
            memory_range += (extents[i] - 1) * strides[i];

        auto cupy = py::module_(py::module_::import("cupy"));
        auto cuda = py::module_(cupy.attr("cuda"));

        auto memory     = cuda.attr("UnownedMemory")(i_ptr, memory_range * type.itemsize(), 0, location.id);
        auto memory_ptr = cuda.attr("MemoryPointer")(memory, 0);

        // There is no way for now to return read only array.
        return cupy.attr("ndarray")(
            "memptr"_a=memory_ptr,
            "dtype"_a=type,
            "shape"_a=py::cast(extents),
            "strides"_a=py::cast(strides)
        );
    }

    template<typename ElementType>
    struct buffer_info_adaptor<ElementType, cuda::location::device> {

        using value_type = std::remove_cv_t<ElementType>;

        static constexpr auto loc_descr() {
            using namespace py::detail;
            return _("cuda");
        }

        optional<py::buffer_info> get_buffer_info(py::handle handle) {

            if (py::hasattr(handle, "__cuda_array_interface__"))
            {
                auto cai = handle.attr("__cuda_array_interface__");

                auto e_and_s = detail::extents_and_stride(cai, sizeof(value_type));

                // auto fmt_src = py::dtype::from_args(cai["typestr"]); //.cast<std::string>();
                // auto dt_dst = py::dtype::of<value_type>();
                // auto byteorder = boost::endian::order::native == boost::endian::order::big ? '>' : '<';
                // if(fmt_src == fmt::format("{}{}{}", byteorder, dt_dst.kind(), dt_dst.itemsize()))

                if( emu::pybind11::numpy::is<value_type>(py::dtype::from_args(cai["typestr"])) )
                {
                    return optional<py::buffer_info>{in_place,
                        reinterpret_cast<value_type*>(cai["data"].cast<py::tuple>()[0].cast<std::uintptr_t>()),
                        mv(e_and_s.extents),
                        mv(e_and_s.strides),
                        not std::is_const_v<ElementType> // There is no way to check mutability from cuda array...
                    };
                }
            }

            return nullopt;
        }

        static cuda::location::device location(const py::buffer_info& bi) {
            ::cuda::memory::pointer_t<void> ptr{bi.ptr};

            // Only way to consistently retreive device location.
            return {ptr.device().id()};
        }

        template <typename Extents, typename LayoutPolicy, typename AccessorPolicy>
        static py::object create_handle(const emu::cuda::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy> & value) {

            using MS = emu::cuda::mdspan<ElementType, Extents, LayoutPolicy, AccessorPolicy>;

            std::vector<py::ssize_t> extents; extents.reserve(MS::rank());
            std::vector<py::ssize_t> strides; strides.reserve(MS::rank());

            for(std::size_t i = 0; i < MS::rank(); ++i) {
                extents.push_back(value.extent(i));
                strides.push_back(value.stride(i) * sizeof(ElementType));
            }

            return to_array(
                reinterpret_cast<const std::byte*>(value.data()), py::dtype::of<ElementType>(),
                std::is_const_v<ElementType>, extents, strides, value.location()
            );
        }

    };

#endif

    template<typename Location>
    py::object to_array(
        const std::byte* ptr, py::dtype type, bool read_only,
        std::vector<py::ssize_t> extents, Location && location
    ) {

        auto strides = strides_vector<py::ssize_t>(std::span{extents}, type.itemsize());

        return to_array(ptr, type, read_only, mv(extents), mv(strides), EMU_FWD(location));
    }

} // namespace cast

} // namespace emu

#endif //EMU_CAST_COMMON_H