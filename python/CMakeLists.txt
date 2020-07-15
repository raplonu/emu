find_package(pybind11 REQUIRED)

if (PYBIND11_PYTHON_VERSION)
    message(STATUS "Python version: ${PYBIND11_PYTHON_VERSION}")
endif()

pybind11_add_module(emuwrap
    src/emu_wrap.cpp)

target_link_libraries(emuwrap PRIVATE emucore)

if (emu_build_cuda)
    target_link_libraries(emuwrap PRIVATE emucuda)
endif()