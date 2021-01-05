cmake_minimum_required(VERSION 3.17)

option(emu_build_cuda   "Build cuda module"     ON)
option(emu_verbose      "Print emu info"        OFF)
option(emu_build_test   "Build unit test"       OFF)

message(STATUS "Emu - Configuring")

set(emu_language CXX)

if (emu_build_cuda)
    list(APPEND emu_language CUDA)
endif()

project(emu VERSION ${emu_version} LANGUAGES ${emu_language})

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

message(CHECK_START "Emu - Conan setup")

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

message(CHECK_PASS "Done")

########################################################################
#
# Emu Core.
#

add_library(emucore
    src/core/fun.cpp)

target_include_directories(emucore PUBLIC
    $<INSTALL_INTERFACE:include/core>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/core>
    ${CUDA_INCLUDE_DIRS}
)

target_compile_features(emucore PUBLIC cxx_std_14)

target_link_libraries(emucore PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::boost
    CONAN_PKG::half
    CONAN_PKG::ms-gsl
    # CONAN_PKG::abseil
    CONAN_PKG::tl-expected
    CONAN_PKG::tl-optional
)

add_library(emu::core ALIAS emucore)
set(emu_targets emucore)

########################################################################
#
# Emu CUDA.
#

if (emu_build_cuda)
    message(CHECK_START "Emu - Configuring CUDA support")

    # Only rely on CUDA package for `cuda_select_nvcc_arch_flags`
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

    find_package(CUDAToolkit REQUIRED)

    add_library(emucuda
        src/cuda/cublas/handle.cpp
        src/cuda/cublas/error.cpp
        src/cuda/cublas.cpp

        src/cuda/cusolver/handle.cpp
        src/cuda/cusolver/error.cpp
        src/cuda/cusolver.cpp

        src/cuda/thrust/execution_policy.cu
    )

    target_include_directories(emucuda PUBLIC
        $<INSTALL_INTERFACE:include/cuda>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cuda>)

    target_compile_features(emucuda PUBLIC cxx_std_14)

    target_link_libraries(emucuda PUBLIC
        emucore
        CUDA::cublas
        CUDA::cudart
        CONAN_PKG::cuda-api-wrappers
    )

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            ${ARCH_FLAGS}
            --extended-lambda
            --expt-relaxed-constexpr
        >)

    add_library(emu::cuda ALIAS emucuda)
    list(APPEND emu_targets emucuda)

    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Google Test.
#
# The tests are not built by default.  To build them, set the
# emu_build_test option to ON.  You can do it by running ccmake
# or specifying the -Demu_build_test=ON flag when running cmake.
if (emu_build_test)
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    message(CHECK_START "Emu - Configuring test")
    enable_testing()
    add_subdirectory(test)
    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Export public compile flags.
#

# Exporting COMPILE_OPTIONS if exists for emucore and emucuda
if(emu_export_flags)
    message(CHECK_START "Exporting compile flags")

    foreach(target IN LISTS emu_targets)
        get_target_property(TMP_CXX_FLAGS ${target} COMPILE_OPTIONS)

        if(TMP_CXX_FLAGS)
            message(STATUS "Exporting ${target} compile flags in ${target}_flags.txt")

            configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/flags.txt.in
                ${CMAKE_CURRENT_BINARY_DIR}/${target}_flags.txt @ONLY)
        endif()
    endforeach()

    message(CHECK_PASS "Done")
endif()
