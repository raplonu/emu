cmake_minimum_required(VERSION 3.18)

project(emu VERSION 0.2.0 LANGUAGES CXX)

option(emu_build_cuda "Build cuda module" ON)

if (emu_build_cuda)
    enable_language(CUDA)
endif()

########################################################################
#
# Emu Core
#

add_library(emucore
    src/core/fun.cpp
)

target_include_directories(emucore PUBLIC include/core)

target_compile_features(emucore PUBLIC cxx_std_20)

if(emu_build_cuda)
    target_compile_definitions(emucore PUBLIC EMU_CUDA)
endif()

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(Microsoft.GSL REQUIRED)
find_package(mdspan REQUIRED)
find_package(tl-expected REQUIRED)
find_package(tl-optional REQUIRED)
find_package(half REQUIRED)

target_link_libraries(emucore PUBLIC
    Boost::boost
    fmt::fmt
    Microsoft.GSL::GSL
    tl::expected
    tl::optional
    std::mdspan
    half::half
)

add_library(emu::core ALIAS emucore)

install(TARGETS emucore)
install(DIRECTORY include/core/emu DESTINATION include)

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

    target_include_directories(emucuda PUBLIC include/cuda)

    target_compile_options(emucuda PUBLIC
        $<$<COMPILE_LANGUAGE:CUDA>:
            --extended-lambda
            --expt-relaxed-constexpr
        >
    )
    # Only needed for CUDA source file but it's easier to just set it
    target_compile_definitions(emucuda PUBLIC FMT_USE_CONSTEXPR=1)

    find_package(CUDAToolkit REQUIRED)
    find_package(cuda-api-wrappers REQUIRED)

    target_link_libraries(emucuda PUBLIC
        emucore
        CUDA::cublas
        CUDA::cudart
        CUDA::cuda_driver
        cuda-api-wrappers::cuda-api-wrappers
    )

    add_library(emu::cuda ALIAS emucuda)

    install(TARGETS emucuda)
    install(DIRECTORY include/cuda/emu DESTINATION include)

    message(CHECK_PASS "Done")
endif()

########################################################################
#
# Emu python
#

# if (emu_build_python)
#     message(CHECK_START "Emu - Configuring python support")

#     add_library(emupython INTERFACE
#         # src/python/cast/common.cpp
#     )

#     target_include_directories(emupython INTERFACE include/python)

#     find_package(pybind11 REQUIRED)

#     target_link_libraries(emupython INTERFACE
#         emucore
#         pybind11::pybind11
#     )

#     if (emu_build_cuda)
#         target_link_libraries(emupython INTERFACE emucuda)
#     endif()

#     add_library(emu::python ALIAS emupython)

#     # install(TARGETS emupython)
#     install(DIRECTORY include/python/emu DESTINATION include)

#     message(CHECK_PASS "Done")
# endif()

########################################################################
#
# Test
#
include(CTest)
if (BUILD_TESTING)
    # This must be set in the root directory for the tests to be run by
    # 'make test' or ctest.
    message(CHECK_START "Emu - Configuring test")
    add_subdirectory(test)
    message(CHECK_PASS "Done")
endif()

