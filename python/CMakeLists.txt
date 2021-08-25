cmake_minimum_required(VERSION 3.18)

message(STATUS "EmuWrap - Configuring")

project(emuwrap VERSION 0.1 LANGUAGES CXX)

find_package(pybind11 REQUIRED)
find_package(emu REQUIRED)

if (PYBIND11_PYTHON_VERSION)
    message(STATUS "Python version: ${PYBIND11_PYTHON_VERSION}")
endif()

pybind11_add_module(emuwrap MODULE
    src/emu_wrap.cpp
)

target_include_directories(emuwrap PUBLIC include)

target_link_libraries(emuwrap PRIVATE emu::emu)

install(TARGETS emuwrap LIBRARY DESTINATION .)
