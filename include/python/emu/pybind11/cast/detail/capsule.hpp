#pragma once

#include <cstring>
#include <emu/capsule.hpp>
#include <emu/optional.hpp>
#include <emu/pybind11.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>

namespace emu::pybind11::detail
{

    constexpr auto emu_capsule_name = "emu_capsule";

    inline emu::capsule array_base_to_capsule(py::handle base)
    {
        if (py::isinstance<py::capsule>(base))
        {
            auto py_cap = base.cast<py::capsule>();

            if (py_cap and std::string_view(emu_capsule_name) == py_cap.name())
            {
                // Note: should not comsume the capsule. Capsule being base of a numpy array is different
                // from one time use capsule returned by `__dlpack__` method.
                // py_cap.set_name("emu_capsule_comsumed");
                auto* ptr = py_cap.get_pointer<emu::capsule::interface>();
                if (ptr)
                    return emu::capsule(ptr);
            }

        }
        // Asume that if base and is not none exist, it holds some kind of capsule that needs to be kept alive.
        return emu::capsule(std::move(base));
    }

    inline emu::capsule handle_to_capsule(py::handle handle)
    {
        // Try access the base attribute of the handle and convert it to a capsule.
        // Otherwise, fallback to keeping the whole handle alive.
        return value_or_invoke(
            // Try to get the base attribute of the handle.
            try_attr(handle, "base")
                // If none, skip to the fallback.
                .and_then(not_none)
                // Try to convert the base attribute to a capsule.
                .map(array_base_to_capsule)
        , [&]{
            // fallback: Last resort, assume that the whole handle needs to be kept alive.
            return emu::capsule(std::move(handle));
        });
    }

    inline py::handle capsule_to_handle(emu::capsule capsule)
    {
        if (capsule) {
            auto* ptr = capsule.holder;
            capsule::manual_hold(ptr);
            return py::capsule(ptr, emu_capsule_name, [](PyObject* ptr){
                auto* cap = reinterpret_cast<emu::capsule::interface*>(ptr);
                capsule::manual_release(cap);
            }).release();
        }
        return py::none();
    }

} // namespace emu::pybind11::detail
