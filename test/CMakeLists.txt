project(emu_test LANGUAGES ${emu_language})

find_package(GTest REQUIRED)

add_executable(emucoretest
    src/core/test.cpp
    src/core/span_test.cpp
)

target_link_libraries(emucoretest PRIVATE
        GTest::GTest GTest::Main emucore
)

add_test(NAME emucoretest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        COMMAND emucoretest)

if (emu_use_cuda)
    add_executable(emucudatest
        src/cuda/test.cu
        src/cuda/iterator_function_test.cu
    )

    target_link_libraries(emucudatest PRIVATE
            GTest::GTest GTest::Main emucuda
    )

    add_test(NAME emucudatest
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
            COMMAND emucudatest)
endif()