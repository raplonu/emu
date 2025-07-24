#pragma once

//Note: from https://github.com/nvidia-riva/riva-asrlib-decoder/blob/test-nemo-model/include/riva/asrlib/decoder/pybind11_dlpack_caster.h

#include <emu/optional.hpp>
#include <emu/pybind11.hpp>

#include <dlpack/dlpack.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace emu::pybind11
{

    inline optional<DLManagedTensor> managed_tensor_from_handle(py::handle src) {
        py::capsule capsule;

        if (py::isinstance<py::capsule>(src)) {
            capsule = py::reinterpret_borrow<py::capsule>(src);
        } else if (py::hasattr(src, "__dlpack__")) {
        // note that, if the user tries to pass in an object with
        // a __dlpack__ attribute instead of a capsule, they have
        // no ability to pass the "stream" argument to __dlpack__

        // this can cause a performance reduction, but not a
        // correctness error, since the default null stream will
        // be used for synchronization if no stream is passed

        // I think I'm stealing this. The result of CallMethod
        // should already have a reference count of 1
        capsule = py::reinterpret_steal<py::capsule>(
            PyObject_CallMethod(src.ptr(), "__dlpack__", nullptr));
        } else {
            return nullopt;
        }

        // is this the same as PyCapsule_IsValid?
        if (strcmp(capsule.name(), "dltensor") != 0) {
            return nullopt;
        }

        // Mark the dltensor capsule as "consumed"
        EMU_INVOKE_AT_SCOPE_EXIT([&] { capsule.set_name("used_dltensor")} );

        return capsule.get_pointer<DLManagedTensor>();
    }

    inline py::handle handle_from_managed_tensor(DLManagedTensor* src, py::return_value_policy policy, py::handle parent) {
        if (src) {
            // why call release here?
            // need to get the capsule a name!
            py::capsule capsule(src, reinterpret_cast<void (*)(void*)>(src->deleter));
            capsule.set_name("dltensor");
            return capsule.release();
        } else {
            return py::none().inc_ref();
        }
    }

} // namespace emu::pybind11
