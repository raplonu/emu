cmake_minimum_required(VERSION 3.18)

message(STATUS "EmuWrap - Configuring")

project(_emu VERSION 0.2.0 LANGUAGES CXX)

find_package(pybind11 REQUIRED)
find_package(emu REQUIRED)

if (PYBIND11_PYTHON_VERSION)
    message(STATUS "Python version: ${PYBIND11_PYTHON_VERSION}")
endif()

pybind11_add_module(_emu MODULE
    src/emu_wrap.cpp
)

target_compile_features(_emu PUBLIC cxx_std_17)

target_include_directories(_emu PUBLIC include)

target_link_libraries(_emu PRIVATE emu::emu)

install(TARGETS _emu LIBRARY DESTINATION .)
