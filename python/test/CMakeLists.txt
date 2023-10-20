cmake_minimum_required(VERSION 3.18)

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(emupythontest
    src/span_test.cpp
    src/pybind11_test.cpp
)

target_link_libraries(emupythontest PRIVATE
    GTest::gtest GTest::gtest_main emupython pybind11::embed
)

target_include_directories(emupythontest PRIVATE include)

gtest_discover_tests(emupythontest)