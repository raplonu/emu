cmake_minimum_required(VERSION 3.18)

enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(emucoretest
    src/core/test.cpp
    src/core/capsule_test.cpp
    src/core/container_test.cpp
    src/core/mdcontainer_test.cpp
    src/core/math_test.cpp
    src/core/functor_test.cpp
    src/core/range_test.cpp
    src/core/expected_test.cpp
    src/core/optional_test.cpp
    src/core/scoped_test.cpp
    src/core/mdalgo_test.cpp
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
        src/cuda/iterator/coordinate_test.cu
        src/cuda/iterator/function_test.cu
    )

    target_link_libraries(emucudatest PRIVATE
        GTest::gtest GTest::gtest_main emucuda
    )

    target_include_directories(emucudatest PRIVATE include)

    gtest_discover_tests(emucudatest)
endif()
