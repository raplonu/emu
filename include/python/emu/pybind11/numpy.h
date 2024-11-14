#pragma once

#include <emu/detail/dlpack_types.hpp>
#include <emu/pybind11.hpp>
#include <emu/cstring_view.hpp>
#include <emu/assert.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


namespace emu::pybind11::numpy
{

    inline uint8_t code_from_np_types(int numpy_type) {
        switch (numpy_type) {
            case py::detail::npy_api::constants::NPY_BOOL_:
                return kDLUInt;

            case py::detail::npy_api::constants::NPY_INT8_:
            case py::detail::npy_api::constants::NPY_INT16_:
            case py::detail::npy_api::constants::NPY_INT32_:
            case py::detail::npy_api::constants::NPY_INT64_:
                return kDLInt;

            case py::detail::npy_api::constants::NPY_UINT8_:
            case py::detail::npy_api::constants::NPY_UINT16_:
            case py::detail::npy_api::constants::NPY_UINT32_:
            case py::detail::npy_api::constants::NPY_UINT64_:
                return kDLUInt;

            case py::detail::npy_api::constants::NPY_FLOAT_:
            case py::detail::npy_api::constants::NPY_DOUBLE_:
            case py::detail::npy_api::constants::NPY_LONGDOUBLE_:
                return kDLFloat;

            case py::detail::npy_api::constants::NPY_CFLOAT_:
            case py::detail::npy_api::constants::NPY_CDOUBLE_:
            case py::detail::npy_api::constants::NPY_CLONGDOUBLE_:
                return kDLComplex;

            default:
                return kDLOpaqueHandle;
        }

    }

    inline int dlpack_type_to_numpy(const dlpack::data_type_ext_t& dtype) {
        if (dtype.lanes != 1) {
            throw std::invalid_argument("NumPy does not support vectorized types");
        }

        switch (dtype.code) {
            case kDLInt:
                switch (dtype.bits) {
                    case 8: return py::detail::npy_api::constants::NPY_INT8_;
                    case 16: return py::detail::npy_api::constants::NPY_INT16_;
                    case 32: return py::detail::npy_api::constants::NPY_INT32_;
                    case 64: return py::detail::npy_api::constants::NPY_INT64_;
                    default: throw std::invalid_argument("Unsupported DLInt bit width");
                }

            case kDLUInt:
                switch (dtype.bits) {
                    case 8: return py::detail::npy_api::constants::NPY_UINT8_;
                    case 16: return py::detail::npy_api::constants::NPY_UINT16_;
                    case 32: return py::detail::npy_api::constants::NPY_UINT32_;
                    case 64: return py::detail::npy_api::constants::NPY_UINT64_;
                    default: throw std::invalid_argument("Unsupported DLUInt bit width");
                }

            case kDLFloat:
                switch (dtype.bits) {
                    case 32: return py::detail::npy_api::constants::NPY_FLOAT_;
                    case 64: return py::detail::npy_api::constants::NPY_DOUBLE_;
                    case 128: return py::detail::npy_api::constants::NPY_LONGDOUBLE_;
                    default: throw std::invalid_argument("Unsupported DLFloat bit width");
                }

            case kDLComplex:
                switch (dtype.bits) {
                    case 32: return py::detail::npy_api::constants::NPY_CFLOAT_;
                    case 64: return py::detail::npy_api::constants::NPY_CDOUBLE_;
                    case 128: return py::detail::npy_api::constants::NPY_CLONGDOUBLE_;
                    default: throw std::invalid_argument("Unsupported DLComplex bit width");
                }

            case kDLOpaqueHandle:
                // kDLOpaqueHandle doesn't have a direct equivalent in NumPy. You can return a special value.
                // For example, return NPY_VOID or create your own custom enum value for opaque handles.
                return py::detail::npy_api::constants::NPY_VOID_;

            default:
                throw std::invalid_argument("Unsupported DLPack type code");
        }
    }

    /// Test if `numpy.dtype` old the same type as T.
    template<typename T>
    bool is(py::dtype dt) {
        return dt.is(py::dtype::of<T>());
    }

    /// Special case for `std::string` which is not accepted by `pybind11::dtype::of`.
    template<>
    inline bool is<std::string>(py::dtype dt) {
        return dt.is(py::dtype::from_args(py::type::of(py::str())));
    }

    py::dtype to_dtype(dlpack::data_type_ext_t dtype) {
        return py::dtype(dlpack_type_to_numpy(dtype));
    }

    py::dtype to_dtype(dlpack::data_type_t dtype) {
        return py::dtype(dlpack_type_to_numpy(dtype));
    }

    dlpack::data_type_ext_t from_dtype(py::dtype dtype) {
        return dlpack::data_type_ext_t{
            .code = code_from_np_types(dtype.num()),
            .bits = dtype.itemsize() * CHAR_BIT,
            .lanes = 1
        };
    }

} // namespace emu::pybind11::numpy
