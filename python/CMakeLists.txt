cmake_minimum_required(VERSION 3.18)

message(CHECK_START "Emu - Configuring")

project(emu VERSION 0.2.0 LANGUAGES CXX)

message(CHECK_PASS "Done")

########################################################################
#
# Emu Core
#

add_library(emupython INTERFACE)

target_include_directories(emupython INTERFACE include)

target_compile_features(emupython INTERFACE cxx_std_20)

find_package(emu REQUIRED)
find_package(pybind11 REQUIRED)

target_link_libraries(emupython INTERFACE
    emu::emu
    pybind11::pybind11
)

add_library(emu::python ALIAS emupython)

install(DIRECTORY include/emu DESTINATION include)

########################################################################
#
# Test
#
include(CTest)
if (BUILD_TESTING)
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    message(CHECK_START "Emu - Configuring test")
    add_subdirectory(test)
    message(CHECK_PASS "Done")
endif()

