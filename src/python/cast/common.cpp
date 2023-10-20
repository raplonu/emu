#include <emu/cast/mdspan.h>
#include <emu/range.h>
#include <emu/cast/common.h>

namespace emu
{

namespace cast
{

namespace detail
{


    extents_strides extents_and_stride(py::handle cai, std::size_t word_size) {
        // auto shape = as_vector<py::ssize_t>(cai["shape"].cast<py::tuple>());
        auto extents = cai["shape"].cast<std::vector<py::ssize_t>>();

        auto strides = value_or_invoke(
            pybind11::not_none(cai["strides"]).map(py::cast<std::vector<py::ssize_t>>),
            [&extents, word_size]{return strides_vector<py::ssize_t>(std::span{extents}, word_size); }
        );


        return {mv(extents), mv(strides)};
    }

} // namespace detail

    py::object to_array(
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

#ifdef EMU_CUDA

namespace detail
{

    py::module_ cupy_module() {
        py::module_ cupy(py::module_::import("cupy"));
        return cupy;
    }

    py::module_ cuda_module() {
        py::module_ cuda(cupy_module().attr("cuda"));
        return cuda;
    }

} // namespace detail

    py::object to_array(
        const std::byte* ptr, py::dtype type, bool read_only,
        std::vector<py::ssize_t> extents, std::vector<py::ssize_t> strides,
        const cuda::location::device & location)
    {
        using namespace py::literals; // to bring in the `_a` literal

        auto i_ptr = reinterpret_cast<std::uintptr_t>(ptr);

        auto memory_range = 1;
        for(std::size_t i = 0; i < extents.size(); ++i)
            memory_range += (extents[i] - 1) * strides[i];

        auto cuda = detail::cuda_module();

        auto memory     = cuda.attr("UnownedMemory")(i_ptr, memory_range * type.itemsize(), 0, location.id);
        auto memory_ptr = cuda.attr("MemoryPointer")(memory, 0);

        // There is no way for now to return read only array.
        return detail::cupy_module().attr("ndarray")(
            "memptr"_a=memory_ptr,
            "dtype"_a=type,
            "shape"_a=py::cast(extents),
            "strides"_a=py::cast(strides)
        );
    }

#endif // EMU_CUDA

} // namespace cast

} // namespace emu
