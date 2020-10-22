cmake_minimum_required(VERSION 3.17)

message(STATUS "EmuWrap - Configuring")

project(emuwrap VERSION ${emuwrap_version} LANGUAGES CXX)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

message(CHECK_START "Emu - Conan setup")

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

message(CHECK_PASS "Done")

find_package(pybind11 REQUIRED)

if (PYBIND11_PYTHON_VERSION)
    message(STATUS "Python version: ${PYBIND11_PYTHON_VERSION}")
endif()

pybind11_add_module(emuwrap
    src/emu_wrap.cpp)

target_link_libraries(emuwrap PRIVATE CONAN_PKG::emu)
