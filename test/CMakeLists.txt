project(emu_test LANGUAGES ${emu_language})

find_package(GTest REQUIRED)

add_executable(emucoretest
    src/core/test.cpp
    src/core/math_test.cpp
    src/core/byte_test.cpp
    src/core/span_test.cpp
    src/core/span_create_test.cpp
    src/core/mdspan_test.cpp
    src/core/range_test.cpp
    src/core/expected_test.cpp
    src/core/optional_test.cpp
    src/core/scoped_test.cpp
)

# Only test string utilities if enable.
if (emu_string_util)
    target_sources(emucoretest PRIVATE src/core/string_test.cpp)
endif()

target_link_libraries(emucoretest PRIVATE
    GTest::gtest GTest::gtest_main emucore
)

target_include_directories(emucoretest PRIVATE include)

add_test(NAME emucoretest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
    COMMAND emucoretest
)

if (emu_build_cuda)
    add_executable(emucudatest
        src/cuda/test.cu
        src/cuda/math_test.cu
        src/cuda/device_span_test.cu
        src/cuda/device_mdspan_test.cu
        src/cuda/iterator/coordinate_test.cu
        src/cuda/iterator/function_test.cu
    )

    target_link_libraries(emucudatest PRIVATE
        GTest::gtest GTest::gtest_main emucuda
    )

    target_include_directories(emucudatest PRIVATE include)

    add_test(NAME emucudatest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
        COMMAND emucudatest
    )
endif()

if (emu_build_python)
    add_executable(emupythontest
        src/python/toto_test.cpp
    )

    target_link_libraries(emupythontest PRIVATE
        GTest::gtest GTest::gtest_main emupython
    )

    target_include_directories(emupythontest PRIVATE include)

    add_test(NAME emupythontest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
        COMMAND emupythontest
    )
endif()