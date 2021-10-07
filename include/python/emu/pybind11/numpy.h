#ifndef EMU_PYTHON_PYBIND11_NUMPY_H
#define EMU_PYTHON_PYBIND11_NUMPY_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace emu
{

namespace pybind11
{

namespace numpy
{

    /// Test if `numpy.dtype` old the same type as T.
    template<typename T>
    bool is(::pybind11::dtype dt) {
        return dt.is(::pybind11::dtype::of<T>());
    }

    /// Special case for `std::string` which is not accepted by `pybind11::dtype::of`.
    template<>
    inline bool is<std::string>(::pybind11::dtype dt) {
        return dt.is(::pybind11::dtype::from_args(::pybind11::type::of(::pybind11::str())));
    }

} // namespace numpy

} // namespace pybind11

} // namespace emu

#endif //EMU_PYTHON_PYBIND11_NUMPY_H