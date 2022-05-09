cmake_minimum_required(VERSION 3.18)

message(CHECK_START "Emu - Configuring")

project(emu VERSION 0.2.0 LANGUAGES CXX)

option(emu_string_util  "emu string utilies"   OFF)
option(emu_half         "emu half utilies"     OFF)
option(emu_build_cuda   "Build cuda module"    ON)
option(emu_build_python "Build python module"  ON)
option(emu_build_test   "Build unit test"      OFF)
option(emu_export_flags "Export compile flags" OFF)

if (emu_build_cuda)
    enable_language(CUDA)
endif()


message(CHECK_PASS "Done")

########################################################################
#
# Emu Core
#

add_library(emucore
    src/core/fun.cpp
)

target_include_directories(emucore PUBLIC include/core)

target_compile_features(emucore PUBLIC cxx_std_17)

if(emu_build_cuda)
    target_compile_definitions(emucore PUBLIC EMU_CUDA)
endif()

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(mdspan REQUIRED)
find_package(range-v3 REQUIRED)
find_package(tl-expected REQUIRED)
find_package(tl-optional REQUIRED)

target_link_libraries(emucore PUBLIC
    Boost::boost
    fmt::fmt
    Microsoft.GSL::GSL
    range-v3::range-v3
    tl::expected
    tl::optional
    std::mdspan
)

if (emu_string_util)
    find_package(absl REQUIRED CONFIG)

    target_compile_definitions(emucore PUBLIC EMU_STRING_UTIL)
    target_link_libraries(emucore PUBLIC absl::absl)
endif()

if (emu_half)
    find_package(half REQUIRED)

    target_compile_definitions(emucore PUBLIC EMU_HALF)
    target_link_libraries(emucore PUBLIC half::half)
endif()

add_library(emu::core ALIAS emucore)
set(emu_targets emucore)

########################################################################
#
# Emu CUDA
#

if (emu_build_cuda)
    message(CHECK_START "Emu - Configuring CUDA support")

    add_library(emucuda
        src/cuda/cublas/handle.cpp
        src/cuda/cublas/error.cpp
        src/cuda/cublas.cpp

        # src/cuda/cufft/handle.cpp
        # src/cuda/cufft/error.cpp
        # src/cuda/cufft.cpp

        src/cuda/cusolver/handle.cpp
        src/cuda/cusolver/error.cpp
        src/cuda/cusolver.cpp

        src/cuda/thrust/execution_policy.cu
    )

    target_include_directories(emucuda PUBLIC
        $<INSTALL_INTERFACE:include/cuda>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/cuda>)

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            --extended-lambda
            --expt-relaxed-constexpr>
    )

    find_package(CUDAToolkit REQUIRED)
    find_package(cuda-api-wrappers REQUIRED)

    target_link_libraries(emucuda PUBLIC
        emucore
        CUDA::cublas
        CUDA::cudart
        cuda-api-wrappers::cuda-api-wrappers
    )

    add_library(emu::cuda ALIAS emucuda)
    list(APPEND emu_targets emucuda)

    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Emu python
#

if (emu_build_python)
    message(CHECK_START "Emu - Configuring python support")

    add_library(emupython
        INTERFACE
    )

    target_include_directories(emupython INTERFACE
        $<INSTALL_INTERFACE:include/python>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/python>)

    if (emu_build_cuda)
        target_link_libraries(emupython INTERFACE emucuda)
    endif()

    find_package(pybind11 REQUIRED)

    target_link_libraries(emupython INTERFACE
        emucore
        pybind11::pybind11
    )

    add_library(emu::python ALIAS emupython)
    list(APPEND emu_targets emupython)

    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Test
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

message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
message("CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

# Exporting COMPILE_OPTIONS if exists for emucore and emucuda
if(emu_export_flags)
    message(CHECK_START "Exporting compile flags")

    foreach(target IN LISTS emu_targets)
        get_target_property(TMP_CXX_FLAGS ${target} COMPILE_OPTIONS)

        if(TMP_CXX_FLAGS)
            message(STATUS "Exporting ${target} compile flags in ${target}_flags.txt")

            configure_file(${CMAKE_SOURCE_DIR}/tools/flags.txt.in
                ${CMAKE_CURRENT_BINARY_DIR}/${target}_flags.txt @ONLY)
        endif()
    endforeach()

    message(CHECK_PASS "Done")
endif()
