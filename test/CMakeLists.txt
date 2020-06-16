    project(emu_test LANGUAGES ${emu_language})

find_package(GTest REQUIRED)

add_executable(emucoretest
    src/core/test.cpp
    src/core/byte_test.cpp
    src/core/span_test.cpp
    src/core/range_test.cpp
    src/core/expected_test.cpp
)

target_link_libraries(emucoretest PRIVATE
    GTest::GTest GTest::Main emucore
)

target_include_directories(emucoretest PRIVATE include)

add_test(NAME emucoretest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    COMMAND emucoretest
)

if (emu_use_cuda)
    add_executable(emucudatest
        src/cuda/test.cu
        src/cuda/iterator_function_test.cu
    )

    # Since cxx_std_17 is not supported for cuda project and do not produce
    # any flag during compilation, we set the flag manually.
    target_compile_options(emucudatest PRIVATE "-std=c++17")

    target_link_libraries(emucudatest PRIVATE
        GTest::GTest GTest::Main emucuda
    )

    target_include_directories(emucudatest PRIVATE include)

    add_test(NAME emucudatest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        COMMAND emucudatest
    )
endif()