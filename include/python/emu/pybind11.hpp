#pragma once

#include <emu/cstring_view.hpp>
#include <emu/type_traits.hpp>
#include <emu/optional.hpp>

#include <pybind11/pybind11.h>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <pybind11/pytypes.h>

namespace emu::pybind11
{

    namespace py = ::pybind11;

    template<typename T>
    bool can_cast(py::handle handle) {
        return py::detail::make_caster<T>().load(handle, true);
    }

    inline optional<py::detail::str_attr_accessor> try_attr(py::handle handle, cstring_view name) {
        if (py::hasattr(handle, name.c_str()))
            return handle.attr(name.c_str());

        return nullopt;
    }

    template<typename T>
    optional<T> try_cast(py::handle handle) {
        // Implementation directly comes from
        // https://github.com/pybind/pybind11/blob/master/include/pybind11/cast.h
        using namespace py::detail;

        static_assert(!cast_is_temporary_value_reference<T>::value,
            "Unable to cast type to reference: value is local to type caster");

        make_caster<T> conv;

        if(conv.load(handle, true))
            return cast_op<T>(conv);

        return nullopt;
    }

    inline optional<py::handle> not_none(py::handle handle) {
        if (handle.is_none())
            return nullopt;

        return handle;
    }

    inline void check_error() {
        if (PyErr_Occurred()) {
            throw ::pybind11::error_already_set();
        }
    }

namespace cpts
{

    template<typename T>
    concept derived_from_handle = std::derived_from<T, py::handle>;

    template<typename T>
    concept accessor = emu::cpts::specialization_of<T, py::detail::accessor>;

} // namespace cpts

} // namespace emu::pybind11
