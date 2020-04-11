cmake_minimum_required(VERSION 3.11)

option(emu_use_cuda   "Use cuda module" OFF)
option(emu_verbose    "Print emu info"  ON)
option(emu_build_test "Build unit test" OFF)

if (emu_verbose)
    message(STATUS "Emu - Configuring.")
endif()

if (emu_use_cuda)
    set(emu_language CUDA CXX)
else()
    set(emu_language CXX)
endif()

project(emu LANGUAGES ${emu_language})

if (emu_verbose)
    message(STATUS "Emu - Conan setup.")
endif()
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

########################################################################
#
# Emu Core.
#

add_library(emu_core SHARED
    src/fun.cpp)

target_include_directories(emu_core PUBLIC include)

target_compile_features(emu_core PUBLIC cxx_std_14)

target_link_libraries(emu_core PUBLIC ${CONAN_LIBS})

add_library(emu::core ALIAS emu_core)

########################################################################
#
# Emu CUDA.
#

if (emu_use_cuda)
    if (emu_verbose)
        message(STATUS "Emu - CUDA support enabled.")
    endif()
    find_package(CUDA REQUIRED)

    if (NOT emu_cuda_sm)
        set(emu_cuda_sm "Auto")
    endif()

    cuda_select_nvcc_arch_flags(ARCH_FLAGS ${emu_cuda_sm})
    if (emu_verbose)
        message(STATUS "Emu - cuda sm : ${ARCH_FLAGS_readable}")
    endif()

    # target_compile_options remove repeated arguments.
    # Since `-gencode` appears multiple times,
    # we replace it by `--generate-code=` and merge it
    # will the next argument since there are related.
    # This way it became unique and avoids difficulties using spaces.
    string(REPLACE "-gencode;" "--generate-code=" ARCH_FLAGS "${ARCH_FLAGS}")

    add_library(emu_cuda SHARED
        srcu/funu.cu
    )

    target_include_directories(emu_cuda PUBLIC include)

    target_compile_features(emu_cuda PUBLIC cxx_std_14)

    target_link_libraries(emu_cuda PUBLIC ${CUDA_LIBRARIES} ${CONAN_LIBS})

    target_compile_options(emu_cuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            ${ARCH_FLAGS}
            --expt-extended-lambda
            --expt-relaxed-constexpr
        >)

    add_library(emu::cuda ALIAS emu_cuda)
endif()

########################################################################
#
# Google Test.
#
# The tests are not built by default.  To build them, set the
# emu_build_test option to ON.  You can do it by running ccmake
# or specifying the -Demu_build_test=ON flag when running cmake.

if (${emu_build_test})
    if (emu_verbose)
        message(STATUS "Emu - Building test.")
    endif()
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    enable_testing()
    add_subdirectory(test)
endif()