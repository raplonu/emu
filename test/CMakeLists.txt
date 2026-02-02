cmake_minimum_required(VERSION 3.25)

project(emutesting VERSION 0.1)

if(PROJECT_IS_TOP_LEVEL)
    message(STATUS "Configuring emu unit tests on installed library")
    include(CTest)
    find_package(emu CONFIG REQUIRED)
endif()

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(emucoretest
    src/core/concepts_test.cpp
    src/core/capsule_test.cpp
    src/core/cstring_view_test.cpp
    # src/core/container_test.cpp
    # src/core/host/container_test.cpp
    # src/core/mdcontainer_test.cpp
    src/core/math_test.cpp
    src/core/pointer_test.cpp
    src/core/functor_test.cpp
    src/core/expected_test.cpp
    src/core/optional_test.cpp
    src/core/ostream_test.cpp
    src/core/scoped_test.cpp
    src/core/managed_object_test.cpp
    src/core/detail/dlpack_types_test.cpp
    src/core/string_test.cpp
)

target_link_libraries(emucoretest PRIVATE
    GTest::gtest GTest::gtest_main emu::core
)

target_include_directories(emucoretest PRIVATE include)

gtest_discover_tests(emucoretest)

if (emu_build_cuda)

    add_executable(emucudatest
        src/cuda/test.cu
        src/cuda/math_test.cu
        src/cuda/memory_test.cpp
        src/cuda/pointer_test.cpp
    )

    target_link_libraries(emucudatest PRIVATE
        GTest::gtest GTest::gtest_main emu::cuda
    )

    target_include_directories(emucudatest PRIVATE include)

    gtest_discover_tests(emucudatest)
endif()

if (emu_build_python_test)

    add_executable(emupythontest
        src/python/main.cpp
        src/python/pybind11/cast/span_test.cpp
        src/python/pybind11/cast/container_test.cpp
        src/python/pybind11/cast/mdspan_test.cpp
        src/python/pybind11/cast/mdcontainer_test.cpp
        src/python/pybind11/cast/cstring_view_test.cpp
        src/python/pybind11/format_test.cpp
    )

    if(emu_build_cuda)
        target_sources(emupythontest PRIVATE
            src/python/pybind11/cast/cuda_device_span_test.cpp
            src/python/pybind11/cast/cuda_device_mdspan_test.cpp
            # src/python/pybind11/cast/cuda/stream_test.cpp
        )
    endif()

    find_package(Python COMPONENTS Interpreter Development)
    find_package(pybind11 REQUIRED)

    target_link_libraries(emupythontest PRIVATE
        GTest::gtest emu::python pybind11::embed
    )

    target_include_directories(emupythontest PRIVATE include)

    gtest_discover_tests(emupythontest)
endif()
