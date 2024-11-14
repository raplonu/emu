cmake_minimum_required(VERSION 3.18)

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(emucoretest
    src/core/concepts_test.cpp
    src/core/capsule_test.cpp
    src/core/cstring_view_test.cpp
    src/core/const_string_test.cpp
    src/core/container_test.cpp
    src/core/host/container_test.cpp
    src/core/mdcontainer_test.cpp
    src/core/math_test.cpp
    src/core/dlpack_test.cpp
    src/core/pointer_test.cpp
    src/core/functor_test.cpp
    src/core/range_test.cpp
    src/core/expected_test.cpp
    src/core/optional_test.cpp
    src/core/scoped_test.cpp
    src/core/managed_object_test.cpp
    src/core/mdalgo_test.cpp
    src/core/detail/dlpack_types_test.cpp
)

target_link_libraries(emucoretest PRIVATE
    GTest::gtest GTest::gtest_main emucore
)

target_include_directories(emucoretest PRIVATE include)

gtest_discover_tests(emucoretest)

if (emu_build_cuda)

    add_executable(emucudatest
        src/cuda/test.cu
        src/cuda/math_test.cu
        src/cuda/pointer_test.cpp
        src/cuda/iterator/coordinate_test.cu
        src/cuda/iterator/function_test.cu
    )

    target_link_libraries(emucudatest PRIVATE
        GTest::gtest GTest::gtest_main emucuda
    )

    target_include_directories(emucudatest PRIVATE include)

    gtest_discover_tests(emucudatest)
endif()

if (emu_build_python_test)

    add_executable(emupythontest
        src/python/main.cpp
        src/python/span_test.cpp
        src/python/container_test.cpp
        src/python/mdspan_test.cpp
        src/python/mdcontainer_test.cpp
        src/python/cstring_view_test.cpp
    )

    if(emu_build_cuda)
        target_sources(emupythontest PRIVATE
            src/python/cuda_span_test.cpp
        )
    endif()

    find_package(pybind11 REQUIRED)

    target_link_libraries(emupythontest PRIVATE
        GTest::gtest emupython pybind11::embed
    )

    target_include_directories(emupythontest PRIVATE include)

    gtest_discover_tests(emupythontest)
endif()
