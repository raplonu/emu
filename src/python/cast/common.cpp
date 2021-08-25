#include <emu/cast/mdspan.h>
#include <emu/range.h>

namespace emu
{

namespace cast
{

namespace detail
{

    std::vector<std::size_t> as_vector(py::tuple t) {
        std::vector<std::size_t> res; res.reserve(t.size());
        for (auto & v : t) {
            res.push_back(v.cast<std::size_t>());
        }
        return res;
    }

    shape_strides shape_and_stride(py::handle cai, std::size_t word_size) {
        auto shape = cai["shape"].cast<py::tuple>();

        auto strides = optional::value_or_invoke(
            not_none(cai["strides"]).map(cast<py::tuple>).map(as_vector),
            [&]{

                std::vector<std::size_t> res(shape.size());
                std::size_t last = word_size;

                for(int i = shape.size() - 1; i >= 0; --i)
                    res[i] = std::exchange(last, shape[i].cast<std::size_t>() * last);

                return res;
            }
        );

        return {mv(as_vector(shape)), mv(strides)};
    }

} // namespace detail

#if EMU_CUDA

namespace detail
{

    pybind11::module_& cupy_module() {
        static pybind11::module_ cupy(pybind11::module_::import("cupy"));
        return cupy;
    }

    pybind11::module_& cuda_module() {
        static pybind11::module_ cuda(cupy_module().attr("cuda"));
        return cuda;
    }

} // namespace detail

#endif // EMU_CUDA

} // namespace cast

} // namespace emu
