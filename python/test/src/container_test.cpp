#include <gtest/gtest.h>

#include <emu/cast/container.hpp>

#include <pybind11/embed.h>

namespace py = pybind11;
using namespace py::literals;

template<typename... Args>
auto invoke_with(std::string code, Args&&... args) {
    auto locals = py::dict(EMU_FWD(args)...);
    py::exec(code, py::globals(), locals);
    return locals;
}
