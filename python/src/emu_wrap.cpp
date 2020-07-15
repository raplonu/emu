#include <pybind11/pybind11.h>

#include <emu/fun.h>

int add(int i, int j) {
    return i + j + 12;
}

PYBIND11_MODULE(emuwrap, m) {
    m.doc() = "pybind11 emuwarp plugin";

    m.def("add", &add, "A function which adds two numbers");
    m.def("fun", &emu::fun, "The emu best function.");
}