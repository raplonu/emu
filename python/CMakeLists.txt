cmake_minimum_required(VERSION 3.11)

message(STATUS "EmuWrap - Configuring")

project(emuwrap VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

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
